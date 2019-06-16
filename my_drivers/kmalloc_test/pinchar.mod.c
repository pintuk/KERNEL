#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x5981f7cd, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xf2d54e34, __VMLINUX_SYMBOL_STR(misc_deregister) },
	{ 0x38e9f70a, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0x187ce45d, __VMLINUX_SYMBOL_STR(contig_page_data) },
	{ 0xd4577ce6, __VMLINUX_SYMBOL_STR(__alloc_pages_nodemask) },
	{ 0x973d0f9e, __VMLINUX_SYMBOL_STR(kstrtoul_from_user) },
	{ 0x4f68e5c9, __VMLINUX_SYMBOL_STR(do_gettimeofday) },
	{ 0x6ea156be, __VMLINUX_SYMBOL_STR(__free_pages) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

