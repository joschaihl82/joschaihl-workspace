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
	{ 0xe1930a66, "tracepoint_probe_register" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xe1930a66, "tracepoint_probe_unregister" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0xff7fbdd1, "___ratelimit" },
	{ 0x3fc737ed, "path_get" },
	{ 0xbd03ed67, "random_kmalloc_seed" },
	{ 0x78339609, "kmalloc_caches" },
	{ 0x957c6137, "__kmalloc_cache_noprof" },
	{ 0xb3d105d8, "d_path" },
	{ 0x3fc737ed, "path_put" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0x984622ae, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xe1930a66,
	0xd272d446,
	0xe1930a66,
	0xbd03ed67,
	0xff7fbdd1,
	0x3fc737ed,
	0xbd03ed67,
	0x78339609,
	0x957c6137,
	0xb3d105d8,
	0x3fc737ed,
	0xd272d446,
	0xcb8b6ec6,
	0xd272d446,
	0xe8213e80,
	0x984622ae,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"tracepoint_probe_register\0"
	"__x86_return_thunk\0"
	"tracepoint_probe_unregister\0"
	"__ref_stack_chk_guard\0"
	"___ratelimit\0"
	"path_get\0"
	"random_kmalloc_seed\0"
	"kmalloc_caches\0"
	"__kmalloc_cache_noprof\0"
	"d_path\0"
	"path_put\0"
	"__stack_chk_fail\0"
	"kfree\0"
	"__fentry__\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "82694E73119CB2104063732");
