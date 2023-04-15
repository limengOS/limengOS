#include <linux/init.h>

extern int __init davinci_mdio_init(void);
int __init cpsw_init(void);
int __init mck_omap_dm_timer_driver_init(void);
int rand_initialize(void);
int __init omap2_dm_timer_init(void);
void __init lim_init(void);
int __init phy_init(void);
int __init smsc_init(void);
int __init tps65217_regulator_init(void);
int __init tps65910_i2c_init(void);
int __init tps65910_init(void);
int __init omap_i2c_init_driver(void);
int __init tps65217_init(void);
int __init tsl2550_init(void);
int __init at24_init(void);
int __init omap_register_i2c_bus_cmdline(void);
void __iomem *am33xx_get_ram_base(void);
int __init omap2_mbox_init(void);
int __init omap_mbox_init(void);
int __init omap_cpufreq_init(void);
int __init cpufreq_core_init(void);
int __init cpufreq_gov_userspace_init(void);
int __init omap2_common_pm_init(void);
int __init twl_init(void);
void __init__nand_base_init(void);
int __init mtd_omap_nand_init(void);
int __init __init__ubifs_init(void);
int __init lzo_mod_init(void);
int __init init_mtdblock(void);
int __init omap2_onenand_init(void);
int __init __init__crypto_algapi_init(void);
int __init init_mtdchar(void);
int __init mtdoops_init(void);
void __init__default_rootfs(void);
void __init__mmc_init(void);
int __init __init__init_ext3_fs(void);
int __init __init__dio_init(void);
int __init __init__ext4_init_fs(void);
int __init __init__init_v2_quota_format(void);
int __init__omap_hsmmc_init(void);
int __init__omap2_gpio_init(void);
int __init__mmc_blk_init(void);

/* ankh_dummy_init : for ld link call funcs files */
void ankh_dummy_init(int dummy);
void ankh_dummy_init(int dummy)
{
if (dummy){
lim_init();
phy_init();
omap2_dm_timer_init();
am33xx_get_ram_base();
davinci_mdio_init();
mck_omap_dm_timer_driver_init();
cpsw_init();
smsc_init();
tps65217_regulator_init();
tps65910_i2c_init();
tps65910_init();
omap_i2c_init_driver();
tps65217_init();
tsl2550_init();
at24_init();
omap_register_i2c_bus_cmdline();
omap2_mbox_init();
omap_mbox_init();
omap_cpufreq_init();
cpufreq_core_init();
cpufreq_gov_userspace_init();
omap2_common_pm_init();
twl_init();
__init__nand_base_init();
mtd_omap_nand_init();
__init__ubifs_init();
lzo_mod_init();
init_mtdblock();
omap2_onenand_init();
__init__crypto_algapi_init();
init_mtdchar();
mtdoops_init();
__init__default_rootfs();
__init__mmc_init();
__init__init_ext3_fs();
__init__dio_init();
__init__ext4_init_fs();
__init__init_v2_quota_format();
__init__omap_hsmmc_init();
__init__omap2_gpio_init();
__init__mmc_blk_init();
}
}
