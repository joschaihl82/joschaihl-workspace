#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xba1b7dc1, "filp_open" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x1c489eb6, "register_kprobe" },
	{ 0x7a8e92c6, "unregister_kprobe" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x888b8f57, "strcmp" },
	{ 0x7a6661ca, "ktime_get_real_seconds" },
	{ 0x7fd36f2e, "time64_to_tm" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0x78339609, "kmalloc_caches" },
	{ 0x957c6137, "__kmalloc_cache_noprof" },
	{ 0x6848eb64, "const_current_task" },
	{ 0x40a621c5, "snprintf" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xf46d5bf3, "mutex_lock" },
	{ 0x9479a1e8, "strnlen" },
	{ 0xe59ceead, "kernel_write" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0xee139a2f, "strncpy_from_user" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xe54e0a6b, "__fortify_panic" },
	{ 0xd272d446, "__fentry__" },
	{ 0xc1616ce7, "filp_close" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xe8213e80, "_printk" },
	{ 0x984622ae, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xba1b7dc1,
	0x90a48d82,
	0x1c489eb6,
	0x7a8e92c6,
	0xbd03ed67,
	0x888b8f57,
	0x7a6661ca,
	0x7fd36f2e,
	0xbd03ed67,
	0x78339609,
	0x957c6137,
	0x6848eb64,
	0x40a621c5,
	0xcb8b6ec6,
	0xf46d5bf3,
	0x9479a1e8,
	0xe59ceead,
	0xf46d5bf3,
	0xee139a2f,
	0xd272d446,
	0xe54e0a6b,
	0xd272d446,
	0xc1616ce7,
	0xd272d446,
	0xe8213e80,
	0x984622ae,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"filp_open\0"
	"__ubsan_handle_out_of_bounds\0"
	"register_kprobe\0"
	"unregister_kprobe\0"
	"__ref_stack_chk_guard\0"
	"strcmp\0"
	"ktime_get_real_seconds\0"
	"time64_to_tm\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"const_current_task\0"
	"snprintf\0"
	"kfree\0"
	"mutex_lock\0"
	"strnlen\0"
	"kernel_write\0"
	"mutex_unlock\0"
	"strncpy_from_user\0"
	"__stack_chk_fail\0"
	"__fortify_panic\0"
	"__fentry__\0"
	"filp_close\0"
	"__x86_return_thunk\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "F290172CCADB7216A7666C7");
