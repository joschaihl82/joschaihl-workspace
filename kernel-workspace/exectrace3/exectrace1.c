// exectrace1.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Generated");
MODULE_DESCRIPTION("Portable exec logger via kprobe (tries many symbol names)");
MODULE_VERSION("0.2");

static struct kprobe kp;
static const char *used_symbol = NULL;

/* pre_handler: called just before the probed instruction is executed */
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    pr_info("exectrace1: Hello World! pid=%d comm=%s probe=%s\n",
            current->pid, current->comm, used_symbol ? used_symbol : "<unknown>");
    return 0;
}

static int try_register_symbol(const char *sym)
{
    int ret;

    /* reset kp for each attempt */
    memset(&kp, 0, sizeof(kp));
    kp.symbol_name = (char *)sym;
    kp.pre_handler = handler_pre;
    /* do not set nonexisting fields like fault_handler */

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
    int i, ret;
    static const char *candidates[] = {
        /* common syscall wrappers and arch variants */
        "__x64_sys_execve",
        "__x64_sys_execveat",
        "__ia32_sys_execve",
        "sys_execve",
        "sys_execveat",
        "SyS_execve",
        "SyS_execveat",
        /* internal exec implementations */
        "do_execveat_common",
        "do_execve",
        "do_execveat",
        /* possible compiler-suffixed variants */
        "do_execveat_common.isra.0",
        "do_execve.isra.0",
        "do_execveat.isra.0",
        /* other variants */
        "sys_execveat_common",
        NULL
    };

    pr_info("exectrace1: init, trying multiple candidate symbols\n");

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

