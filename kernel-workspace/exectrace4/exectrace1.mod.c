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
	{ 0x1c489eb6, "register_kprobe" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x7a8e92c6, "unregister_kprobe" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x888b8f57, "strcmp" },
	{ 0xee139a2f, "strncpy_from_user" },
	{ 0x6848eb64, "const_current_task" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xd272d446, "__fentry__" },
	{ 0xe8213e80, "_printk" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x984622ae, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x1c489eb6,
	0xd272d446,
	0x7a8e92c6,
	0xbd03ed67,
	0x888b8f57,
	0xee139a2f,
	0x6848eb64,
	0xd272d446,
	0xd272d446,
	0xe8213e80,
	0x90a48d82,
	0x984622ae,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"register_kprobe\0"
	"__x86_return_thunk\0"
	"unregister_kprobe\0"
	"__ref_stack_chk_guard\0"
	"strcmp\0"
	"strncpy_from_user\0"
	"const_current_task\0"
	"__stack_chk_fail\0"
	"__fentry__\0"
	"_printk\0"
	"__ubsan_handle_out_of_bounds\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "449B7EB478C4338A650FEDE");
