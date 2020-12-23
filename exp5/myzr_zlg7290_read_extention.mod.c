#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0xe8e9fb26, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xbf7c6ab1, __VMLINUX_SYMBOL_STR(i2c_del_driver) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x2f364ccf, __VMLINUX_SYMBOL_STR(i2c_register_driver) },
	{ 0x92e3cd47, __VMLINUX_SYMBOL_STR(i2c_transfer) },
	{ 0x9cec22c1, __VMLINUX_SYMBOL_STR(dev_driver_string) },
	{ 0xbf390e66, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xf08b36f4, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xd8e484f0, __VMLINUX_SYMBOL_STR(register_chrdev_region) },
	{ 0x8f98abaf, __VMLINUX_SYMBOL_STR(dev_set_drvdata) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xb23645cc, __VMLINUX_SYMBOL_STR(input_free_device) },
	{ 0x7233857, __VMLINUX_SYMBOL_STR(input_unregister_device) },
	{ 0x901dcb16, __VMLINUX_SYMBOL_STR(dev_err) },
	{ 0xa20680fb, __VMLINUX_SYMBOL_STR(input_register_device) },
	{ 0x66ce1684, __VMLINUX_SYMBOL_STR(input_allocate_device) },
	{ 0xfe4e8bab, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0xfdffe514, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x16e19558, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xaaf1a70a, __VMLINUX_SYMBOL_STR(dev_get_drvdata) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("of:N*T*Cmyzr,zlg7290*");
MODULE_ALIAS("i2c:zlg7290");

MODULE_INFO(srcversion, "F55AC57D80672F99F694DCF");
