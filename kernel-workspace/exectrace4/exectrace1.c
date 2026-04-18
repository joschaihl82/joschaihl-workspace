/*
 * exectrace1.c
 *
 * Portable exec logger via kprobe.
 * Priorisiert kernel-seitige bprm-Hooks (zuverlässig: bprm_execve / do_execve*),
 * fällt zurück auf Syscall-Wrapper (__x64_sys_execve / __x64_sys_execveat) wenn nötig.
 *
 * Hinweise:
 * - Zielarchitektur: x86_64 (Registerzugriff auf rdi/rsi für Syscall-Fallback).
 * - Kandidatenliste wurde erweitert um die Varianten, die in /proc/kallsyms auftauchen.
 * - bprm->filename wird direkt aus Kernel-Speicher kopiert (kein userspace-read nötig).
 *
 * Build:
 * - obj-m += exectrace1.o
 * - make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
 * - sudo insmod exectrace1.ko
 * - sudo rmmod exectrace1
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/binfmts.h>    /* struct linux_binprm */
#include <linux/string.h>     /* memset */
#include <linux/uaccess.h>    /* strncpy_from_user */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("exectrace1 (patched)");
MODULE_DESCRIPTION("Portable exec logger via kprobe (reads bprm->filename when available)");
MODULE_VERSION("0.8");

#define FNAME_BUF 256

static struct kprobe kp;
static const char *used_symbol = NULL;

/* Safe kernel-string copy from kernel pointer (bprm->filename is kernel memory) */
static void copy_kstring(char *dst, const char *src, size_t dst_len)
{
    size_t i;

    if (!dst || dst_len == 0)
        return;
    if (!src) {
        dst[0] = '\0';
        return;
    }

    for (i = 0; i < dst_len - 1; ++i) {
        char c = src[i];
        dst[i] = c;
        if (c == '\0')
            break;
    }
    dst[dst_len - 1] = '\0';
}

/* Try to read filename from userspace pointer safely (fallback for syscall wrapper) */
static int copy_userspace_filename(char *dst, const char __user *uaddr, size_t dst_len)
{
    long ret;

    if (!dst || dst_len == 0)
        return -EINVAL;
    if (!uaddr) {
        dst[0] = '\0';
        return -EINVAL;
    }

    /* strncpy_from_user returns number of bytes copied (including NUL) or negative error */
    ret = strncpy_from_user(dst, uaddr, dst_len);
    if (ret <= 0)
        return -EFAULT;

    /* Ensure NUL termination if buffer filled without NUL */
    if (ret == dst_len)
        dst[dst_len - 1] = '\0';

    return 0;
}

/* pre_handler: called just before the probed instruction is executed */
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    char fname[FNAME_BUF];
    struct linux_binprm *bprm = NULL;
    int rc = -1;

    memset(fname, 0, sizeof(fname));

    /* If we hooked a kernel internal that receives struct linux_binprm * as first arg,
     * read bprm->filename (kernel memory) which is the most reliable source.
     */
    if (used_symbol &&
        (strcmp(used_symbol, "bprm_execve") == 0 ||
         strcmp(used_symbol, "bprm_execve.part.0") == 0 ||
         strcmp(used_symbol, "__pfx_bprm_execve") == 0 ||
         strcmp(used_symbol, "__pfx_bprm_execve.part.0") == 0 ||
         strcmp(used_symbol, "do_execveat_common.isra.0") == 0 ||
         strcmp(used_symbol, "do_execveat_common") == 0 ||
         strcmp(used_symbol, "kernel_execve") == 0)) {

        /* On x86_64 these internal functions take struct linux_binprm * as first arg (rdi) */
        bprm = (struct linux_binprm *)regs->di;
        if (bprm && bprm->filename) {
            copy_kstring(fname, bprm->filename, sizeof(fname));
            rc = 0;
        } else {
            rc = -EFAULT;
        }
    } else {
        /* Fallback: we hooked a syscall wrapper (e.g. __x64_sys_execve or __x64_sys_execveat)
         * Try to read userspace pointer from registers (x86_64: rdi = filename for execve,
         * rsi = pathname for execveat). This is less reliable but useful as fallback.
         */
        const char __user *u_fname = NULL;

        if (used_symbol &&
            (strcmp(used_symbol, "__x64_sys_execve") == 0 ||
             strcmp(used_symbol, "sys_execve") == 0 ||
             strcmp(used_symbol, "SyS_execve") == 0)) {
            u_fname = (const char __user *)regs->di;
        } else if (used_symbol &&
               (strcmp(used_symbol, "__x64_sys_execveat") == 0 ||
                strcmp(used_symbol, "sys_execveat") == 0 ||
                strcmp(used_symbol, "SyS_execveat") == 0)) {
            u_fname = (const char __user *)regs->si;
        } else {
            /* unknown symbol: try rdi then rsi */
            u_fname = (const char __user *)regs->di;
            if (!u_fname)
                u_fname = (const char __user *)regs->si;
        }

        if (u_fname) {
            rc = copy_userspace_filename(fname, u_fname, sizeof(fname));
        } else {
            rc = -EFAULT;
        }
    }

    if (rc == 0) {
        pr_info("exectrace1: pid=%d parent_comm=%s exec_filename=%s probe=%s\n",
            current->pid, current->comm, fname, used_symbol ? used_symbol : "<unknown>");
    } else {
        pr_info("exectrace1: pid=%d parent_comm=%s exec_filename=<unreadable> probe=%s\n",
            current->pid, current->comm, used_symbol ? used_symbol : "<unknown>");
    }

    return 0;
}

/* Try to register a kprobe on symbol 'sym' */
static int try_register_symbol(const char *sym)
{
    int ret;

    memset(&kp, 0, sizeof(kp));
    kp.symbol_name = (char *)sym;
    kp.pre_handler = handler_pre;

    ret = register_kprobe(&kp);
    if (ret == 0) {
        used_symbol = sym;
        pr_info("exectrace1: registered kprobe on %s\n", sym);
        return 0;
    }

    pr_info("exectrace1: register_kprobe on %s failed: %d\n", sym, ret);
    return ret;
}

static int __init exectrace1_init(void)
{
    /* Candidate list prioritized: kernel-side bprm functions first (reliable),
     * then do_execve* variants, then kernel_execve, then syscall wrappers as last resort.
     * These names reflect common variants found in /proc/kallsyms on many kernels.
     */
    static const char *candidates[] = {
        /* bprm variants observed in your kallsyms */
        "bprm_execve",
        "bprm_execve.part.0",
        "__pfx_bprm_execve",
        "__pfx_bprm_execve.part.0",
        /* internal exec implementations */
        "do_execveat_common.isra.0",
        "do_execveat_common",
        /* kernel_execve as another internal hook */
        "kernel_execve",
        /* syscall wrappers (fallback) */
        "__x64_sys_execve",
        "__x64_sys_execveat",
        "sys_execve",
        "sys_execveat",
        NULL
    };
    int i, ret;

    pr_info("exectrace1: init, trying candidate symbols for exec logging\n");

    for (i = 0; candidates[i] != NULL; ++i) {
        ret = try_register_symbol(candidates[i]);
        if (ret == 0)
            return 0;
    }

    pr_err("exectrace1: could not register kprobe on any candidate\n");
    return -ENODEV;
}

static void __exit exectrace1_exit(void)
{
    if (used_symbol) {
        unregister_kprobe(&kp);
        pr_info("exectrace1: unregistered kprobe from %s\n", used_symbol);
    } else {
        pr_info("exectrace1: nothing to unregister\n");
    }
    pr_info("exectrace1: module unloaded\n");
}

module_init(exectrace1_init);
module_exit(exectrace1_exit);

