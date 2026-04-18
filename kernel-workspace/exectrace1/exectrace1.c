// exec_trace_fullpath.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/binfmts.h>
#include <linux/tracepoint.h>
#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/limits.h>
#include <linux/ratelimit.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Josh + Copilot");
MODULE_DESCRIPTION("Trace executed binaries and print resolved full path using sched_process_exec tracepoint");
MODULE_VERSION("0.2");

/*
 * Tracepoint callback signature:
 * void probe(void *data, struct task_struct *p, pid_t old_pid, struct linux_binprm *bprm)
 *
 * We resolve the path from bprm->file->f_path when available.
 */

static void probe_sched_process_exec(void *data,
                                     struct task_struct *p,
                                     pid_t old_pid,
                                     struct linux_binprm *bprm)
{
    char *buf = NULL;
    char *path = NULL;

    if (!p || !bprm)
        return;

    /* Use rate-limited logging to avoid flooding dmesg */
    if (!__ratelimit(&pr_err_ratelimit_state))
        return;

    /* If the kernel has opened the file for the new executable, try to resolve its full path */
    if (bprm->file) {
        struct path fpath;

        /* copy the path structure to avoid races */
        fpath = bprm->file->f_path;
        path_get(&fpath);

        buf = kmalloc(PATH_MAX, GFP_ATOMIC);
        if (!buf) {
            path_put(&fpath);
            pr_info_ratelimited("exec_trace: pid=%d comm=%s filename=%s (no mem for path)\n",
                        p->pid, p->comm, bprm->filename ? bprm->filename : "<null>");
            return;
        }

        path = d_path(&fpath, buf, PATH_MAX);
        if (IS_ERR(path)) {
            pr_info_ratelimited("exec_trace: pid=%d comm=%s filename=%s (d_path failed)\n",
                        p->pid, p->comm, bprm->filename ? bprm->filename : "<null>");
        } else {
            pr_info_ratelimited("exec_trace: pid=%d comm=%s path=%s\n",
                        p->pid, p->comm, path);
        }

        kfree(buf);
        path_put(&fpath);
    } else {
        /* Fall back to bprm->filename when file pointer is not available */
        pr_info_ratelimited("exec_trace: pid=%d comm=%s filename=%s\n",
                    p->pid, p->comm, bprm->filename ? bprm->filename : "<null>");
    }
}

/* Tracepoint registration functions exported by the kernel */
extern int register_trace_sched_process_exec(void (*probe)(void *data,
                                                           struct task_struct *p,
                                                           pid_t old_pid,
                                                           struct linux_binprm *bprm),
                                             void *data);
extern void unregister_trace_sched_process_exec(void (*probe)(void *data,
                                                               struct task_struct *p,
                                                               pid_t old_pid,
                                                               struct linux_binprm *bprm),
                                                void *data);

static int __init exec_trace_init(void)
{
    int ret;

    ret = register_trace_sched_process_exec(probe_sched_process_exec, NULL);
    if (ret) {
        pr_err("exec_trace: failed to register tracepoint (ret=%d)\n", ret);
        return ret;
    }

    pr_info("exec_trace: module loaded, tracing exec paths\n");
    return 0;
}

static void __exit exec_trace_exit(void)
{
    unregister_trace_sched_process_exec(probe_sched_process_exec, NULL);
    pr_info("exec_trace: module unloaded\n");
}

module_init(exec_trace_init);
module_exit(exec_trace_exit);
