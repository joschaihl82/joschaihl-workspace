/*
 * exectrace1.c
 *
 * Exec logger via kprobe that appends entries to /exectrace.log
 * - prefers kernel-side bprm hooks (bprm_execve / do_execve*),
 *   falls nicht verfügbar fällt es auf syscall-wrapper zurück.
 * - writes lines like:
 *   [unix_seconds] pid=1234 parent_comm=bash exec_filename=/bin/ls probe=bprm_execve
 *
 * Build:
 *   obj-m += exectrace1.o
 *   make -C /lib/modules/$(uname -r)/build M=$(PWD) modules
 *   sudo insmod exectrace1.ko
 *   sudo rmmod exectrace1
 *
 * WARNING: writing from kernel to filesystem has security/stability implications.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/binfmts.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/timekeeping.h>
#include <linux/time.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("exectrace1 (patched)");
MODULE_DESCRIPTION("Exec logger via kprobe that appends to /exectrace.log");
MODULE_VERSION("0.9");

#define FNAME_BUF 256
#define LOG_PATH "/exectrace.log"
#define LOG_MODE 0644

static struct kprobe kp;
static const char *used_symbol = NULL;

/* file handle for log */
static struct file *logf = NULL;
/* mutex to serialize writes */
static DEFINE_MUTEX(log_mutex);

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

    ret = strncpy_from_user(dst, uaddr, dst_len);
    if (ret <= 0)
        return -EFAULT;

    if (ret == dst_len)
        dst[dst_len - 1] = '\0';

    return 0;
}

/* Kernel write wrapper: use kernel_write if available, else vfs_write */
static ssize_t exectrace_kernel_write(struct file *file, const char *buf, size_t len, loff_t *pos)
{
    ssize_t written = -EIO;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
    /* kernel_write exists on many kernels */
    written = kernel_write(file, buf, len, pos);
#else
    /* older kernels: use vfs_write (deprecated on newer kernels) */
#if defined(CONFIG_SMP) || defined(CONFIG_PREEMPT)
    /* vfs_write signature: ssize_t vfs_write(struct file *, const char __user *, size_t, loff_t *); */
    /* But vfs_write expects user-space pointer; use kernel_write wrapper if not available */
    written = vfs_write(file, buf, len, pos);
#else
    written = vfs_write(file, buf, len, pos);
#endif
#endif

    return written;
}

/* Append a single log line to LOG_PATH (logf must be opened) */
static void write_log_line(const char *line)
{
    loff_t pos;
    ssize_t ret;

    if (!logf) {
        pr_warn("exectrace1: log file not open\n");
        return;
    }

    mutex_lock(&log_mutex);

    /* append: set pos to file size */
    pos = i_size_read(file_inode(logf));

    ret = exectrace_kernel_write(logf, line, strlen(line), &pos);
    if (ret < 0) {
        pr_warn("exectrace1: write failed: %zd\n", ret);
        /* fallback to pr_info so we don't lose the event entirely */
        pr_info("%s", line);
    } else {
        /* update f_pos for consistency */
        logf->f_pos = pos;
    }

    mutex_unlock(&log_mutex);
}

/* pre_handler: called just before the probed instruction is executed */
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    char fname[FNAME_BUF];
    struct linux_binprm *bprm = NULL;
    int rc = -1;
    char *line = NULL;
    size_t line_len;
    time64_t now;
    struct tm tm;
    int printed;

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

    /* Build log line: timestamp + info */
    now = ktime_get_real_seconds();
    time64_to_tm(now, 0, &tm);

    /* allocate line buffer on kernel heap */
    /* Format: [YYYY-MM-DD HH:MM:SS] pid=... parent_comm=... exec_filename=... probe=...\n */
    line_len = 512;
    line = kmalloc(line_len, GFP_ATOMIC);
    if (!line) {
        /* fallback to pr_info */
        if (rc == 0)
            pr_info("exectrace1: pid=%d parent_comm=%s exec_filename=%s probe=%s\n",
                current->pid, current->comm, fname, used_symbol ? used_symbol : "<unknown>");
        else
            pr_info("exectrace1: pid=%d parent_comm=%s exec_filename=<unreadable> probe=%s\n",
                current->pid, current->comm, used_symbol ? used_symbol : "<unknown>");
        return 0;
    }

    if (rc == 0) {
        printed = snprintf(line, line_len, "[%04ld-%02d-%02d %02d:%02d:%02d] pid=%d parent_comm=%s exec_filename=%s probe=%s\n",
            (long)tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            current->pid, current->comm, fname, used_symbol ? used_symbol : "<unknown>");
    } else {
        printed = snprintf(line, line_len, "[%04ld-%02d-%02d %02d:%02d:%02d] pid=%d parent_comm=%s exec_filename=<unreadable> probe=%s\n",
            (long)tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            current->pid, current->comm, used_symbol ? used_symbol : "<unknown>");
    }

    if (printed > 0)
        write_log_line(line);

    kfree(line);
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

/* Open or create the log file and keep handle in logf */
static int open_log_file(void)
{
    /* open with O_WRONLY|O_CREAT|O_APPEND */
    logf = filp_open(LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, LOG_MODE);
    if (IS_ERR(logf)) {
        pr_err("exectrace1: filp_open(%s) failed: %ld\n", LOG_PATH, PTR_ERR(logf));
        logf = NULL;
        return -ENOENT;
    }

    /* ensure f_pos is at end for append */
    logf->f_pos = i_size_read(file_inode(logf));
    return 0;
}

/* Close log file */
static void close_log_file(void)
{
    if (logf) {
        filp_close(logf, NULL);
        logf = NULL;
    }
}

static int __init exectrace1_init(void)
{
    /* Candidate list prioritized: kernel-side bprm functions first (reliable),
     * then do_execve* variants, then kernel_execve, then syscall wrappers as last resort.
     */
    static const char *candidates[] = {
        "bprm_execve",
        "bprm_execve.part.0",
        "__pfx_bprm_execve",
        "__pfx_bprm_execve.part.0",
        "do_execveat_common.isra.0",
        "do_execveat_common",
        "kernel_execve",
        /* syscall wrappers (fallback) */
        "__x64_sys_execve",
        "__x64_sys_execveat",
        "sys_execve",
        "sys_execveat",
        NULL
    };
    int i, ret;

    pr_info("exectrace1: init, opening log file %s\n", LOG_PATH);

    ret = open_log_file();
    if (ret) {
        pr_err("exectrace1: could not open log file, aborting init\n");
        /* continue init attempt for kprobe registration? better to abort */
        return ret;
    }

    pr_info("exectrace1: log file opened\n");

    pr_info("exectrace1: trying candidate symbols for exec logging\n");

    for (i = 0; candidates[i] != NULL; ++i) {
        ret = try_register_symbol(candidates[i]);
        if (ret == 0)
            return 0;
    }

    pr_err("exectrace1: could not register kprobe on any candidate\n");
    /* close file on failure */
    close_log_file();
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

    close_log_file();
    pr_info("exectrace1: module unloaded\n");
}

module_init(exectrace1_init);
module_exit(exectrace1_exit);

