PKGDIR?= ../..
L4DIR ?= $(PKGDIR)/../..
OMAP  ?= $(PKGDIR)/lib/ti-omap-335x/mach-omap2

KBUILD_DEFINES = -D"KBUILD_STR(s)=\#s" \
                 -D"KBUILD_BASENAME=KBUILD_STR($(patsubst %.o,%,$(notdir $@)))" \
                 -D"KBUILD_MODNAME=KBUILD_STR($(patsubst %.o,%,$@))"

DEFINES +=  $(KBUILD_DEFINES) -DCONFIG_ARCH_OMAP3

PC_FILENAME   = ti-omap-335x
TARGET        = libti-omap-335x.a #libti-omap-335x.so
SYSTEMS       = x86-l4f arm-l4f

SRC_C += mach-omap2/omap_device.c mach-omap2/devices.c  mach-omap2/cm2xxx_3xxx.c mach-omap2/cminst44xx.c  mach-omap2/clockevents.c   \
         mach-omap2/clocksource.c mach-omap2/prm2xxx_3xxx.c mach-omap2/prminst44xx.c  mach-omap2/counter_32k.c ./mach-omap2/omap_hwmod.c \
         mach-omap2/omap_hwmod_common_data.c  mach-omap2/omap_hwmod_2xxx_3xxx_ipblock_data.c mach-omap2/display.c mach-omap2/i2c.c \
         mach-omap2/voltagedomains3xxx_data.c mach-omap2/omap_hwmod_33xx_data.c mach-omap2/clock.c  mach-omap2/prm_common.c \
         mach-omap2/mck-board-am335xevm.c   mach-omap2/mux33xx.c mach-omap2/mux.c mach-omap2/prcm.c mach-omap2/powerdomain.c  \
         mach-omap2/clock3517.c mach-omap2/clock34xx.c mach-omap2/clock3xxx.c mach-omap2/clkt34xx_dpll3m2.c mach-omap2/clock36xx.c  \
         mach-omap2/clock_common_data.c mach-omap2/clock3xxx_data.c mach-omap2/clock33xx_data.c mach-omap2/sdrc.c mach-omap2/sram.c \
         mach-omap2/clkt_clksel.c mach-omap2/dpll3xxx.c mach-omap2/clkt_dpll.c mach-omap2/clockdomain.c mach-omap2/id.c  \
         mach-omap2/clkt_iclk.c mach-omap2/io.c mach-omap2/common.c mach-omap2/timer.c mach-omap2/control.c mach-omap2/pm-debug.c \
         mach-omap2/clockdomains33xx_data.c  mach-omap2/clockdomain44xx.c mach-omap2/voltagedomains33xx_data.c mach-omap2/powerdomains33xx_data.c \
         mach-omap2/powerdomain44xx.c mach-omap2/powerdomain-common.c mach-omap2/voltage.c mach-omap2/serial.c mach-omap2/opp3xxx_data.c \
         mach-omap2/opp.c mach-omap2/board-flash.c mach-omap2/hsmmc.c mach-omap2/mailbox.c mach-omap2/pm.c mach-omap2/twl-common.c \
         mach-omap2/omap_twl.c mach-omap2/vp.c mach-omap2/vc.c mach-omap2/pm33xx.c mach-omap2/gpmc.c mach-omap2/gpmc-nand.c \
         mach-omap2/gpio.c mach-omap2/irq.c

SRC_C += kernel/manage.c kernel/cpu.c kernel/resend.c kernel/dummychip.c kernel/chip.c \
         kernel/fork.c  kernel/handle.c kernel/kallsyms.c \
         kernel/kfifo.c

SRC_C += arch/arm/kernel/irq.c drivers/mfd/tps65910-irq.c drivers/mfd/tps65910.c drivers/mfd/tps65217.c \
         drivers/regulator/tps65910-regulator.c drivers/regulator/tps65217-regulator.c \
         drivers/i2c/busses/i2c-omap.c drivers/misc/tsl2550.c drivers/misc/eeprom/at24.c  \
         drivers/base/power/opp.c arch/arm/plat-omap/i2c.c arch/arm/plat-omap/common.c \
         drivers/cpufreq/omap-cpufreq.c drivers/cpufreq/freq_table.c drivers/cpufreq/cpufreq.c drivers/cpufreq/cpufreq_performance.c \
         drivers/cpufreq/cpufreq_userspace.c drivers/mfd/twl-core.c drivers/mfd/twl6030-irq.c drivers/mfd/twl4030-irq.c \
         drivers/mfd/twl4030-power.c arch/arm/plat-omap/mailbox.c arch/arm/plat-omap/sdma2edma.c arch/arm/common/edma.c
            
SRC_C += ti/cpsw.c ti/cpsw_ale.c ti/davinci_cpdma.c ti/davinci_mdio.c ti/smsc.c  ti/phy_device.c ti/mdio_bus.c 

SRC_C += lib/kasprintf.c  lib/halfmd4.c \
          init/main.c init/calibrate.c init/do_mounts.c  init/noinitramfs.c

SRC_C += generic-chip.c  clock.c  phy.c    \
         dmtimer.c irqdesc.c   lim_region.c omap-pm-noop.c 

SRC_C +=  drivers/mtd/nand/omap2.c drivers/mtd/nand/nand_base.c drivers/mtd/nand/nand_ecc.c drivers/mtd/nand/nand_bbt.c drivers/mtd/nand/nand_ids.c \
          drivers/mtd/mtd_blkdevs.c drivers/mtd/mtdblock.c drivers/mtd/mtdchar.c drivers/mtd/mtdconcat.c drivers/mtd/mtdcore.c drivers/mtd/mtdoops.c \
          drivers/mtd/mtdpart.c drivers/mtd/mtdsuper.c drivers/mtd/devices/omap2_elm.c \
          drivers/mtd/ubi/kapi.c drivers/mtd/ubi/io.c drivers/mtd/ubi/cdev.c drivers/mtd/ubi/eba.c drivers/mtd/ubi/misc.c drivers/mtd/ubi/scan.c \
          drivers/mtd/ubi/upd.c drivers/mtd/ubi/vmt.c drivers/mtd/ubi/vtbl.c drivers/mtd/ubi/wl.c \
          drivers/mtd/ubi/build.c crypto/api.c crypto/compress.c crypto/cipher.c crypto/algapi.c crypto/lzo.c \
          drivers/mtd/onenand/omap2.c drivers/mtd/onenand/onenand_base.c drivers/mtd/onenand/onenand_bbt.c 

SRC_C +=  drivers/mmc/core/core.c  drivers/mmc/core/bus.c drivers/mmc/core/debugfs.c drivers/mmc/core/host.c drivers/mmc/core/mmc.c \
          drivers/mmc/core/mmc_ops.c drivers/mmc/core/quirks.c drivers/mmc/core/sd.c drivers/mmc/core/sdio_bus.c drivers/mmc/core/sdio.c \
          drivers/mmc/core/sdio_cis.c drivers/mmc/core/sdio_io.c drivers/mmc/core/sdio_irq.c drivers/mmc/core/sdio_ops.c drivers/mmc/core/sd_ops.c \
          drivers/mmc/host/omap_hsmmc.c drivers/mmc/card/block.c  drivers/mmc/card/queue.c
          
SRC_C +=  fs/ubifs/budget.c fs/ubifs/commit.c fs/ubifs/compress.c fs/ubifs/dir.c fs/ubifs/file.c fs/ubifs/find.c fs/ubifs/gc.c fs/ubifs/io.c \
          fs/ubifs/ioctl.c fs/ubifs/journal.c fs/ubifs/log.c fs/ubifs/lprops.c fs/ubifs/lpt.c fs/ubifs/lpt_commit.c fs/ubifs/master.c \
          fs/ubifs/orphan.c fs/ubifs/recovery.c fs/ubifs/replay.c fs/ubifs/sb.c fs/ubifs/scan.c fs/ubifs/shrinker.c fs/ubifs/super.c \
          fs/ubifs/tnc.c fs/ubifs/tnc_commit.c fs/ubifs/tnc_misc.c  

SRC_C +=  fs/direct-io.c fs/ioctl.c fs/pipe.c  fs/xattr.c fs/mbcache.c
SRC_C +=  fs/jbd/transaction.c fs/jbd/journal.c fs/jbd/recovery.c fs/jbd/checkpoint.c fs/jbd/commit.c fs/jbd/revoke.c
SRC_C +=  fs/quota/dquot.c fs/quota/quota_tree.c fs/quota/quota_v2.c

SRC_C +=  fs/ext3/balloc.c fs/ext3/bitmap.c fs/ext3/dir.c fs/ext3/file.c fs/ext3/fsync.c fs/ext3/ialloc.c fs/ext3/inode.c fs/ext3/ioctl.c \
          fs/ext3/namei.c fs/ext3/super.c fs/ext3/symlink.c fs/ext3/hash.c fs/ext3/resize.c fs/ext3/ext3_jbd.c 

SRC_C +=  fs/jbd2/transaction.c fs/jbd2/journal.c fs/jbd2/recovery.c fs/jbd2/checkpoint.c fs/jbd2/commit.c fs/jbd2/revoke.c

SRC_C +=  fs/ext4/balloc.c fs/ext4/bitmap.c fs/ext4/block_validity.c fs/ext4/dir.c fs/ext4/file.c fs/ext4/fsync.c fs/ext4/ialloc.c fs/ext4/inode.c fs/ext4/ioctl.c \
          fs/ext4/extents.c fs/ext4/indirect.c fs/ext4/mballoc.c fs/ext4/migrate.c fs/ext4/mmp.c fs/ext4/move_extent.c fs/ext4/page-io.c fs/ext4/xattr.c \
          fs/ext4/xattr_trusted.c  fs/ext4/xattr_user.c \
          fs/ext4/namei.c fs/ext4/super.c fs/ext4/symlink.c fs/ext4/hash.c fs/ext4/resize.c fs/ext4/ext4_jbd2.c 


ifeq ($(ARCH), arm)
ASFLAGS += -D__ASSEMBLY__ -include asm/unified.h
SRC_S :=  setbit.S clearbit.S mach-omap2/sleep33xx.S
endif

#SRC_C := $(wildcard *.c)
REQUIRES_LIBS = libio libio-vbus

ifeq ($(ARCH), arm)
DEFINES += -D__LINUX_ARM_ARCH__=7
ARCH_DIR = arch/arm
endif

CFLAGS += -include $(SRC_DIR)/../../include/plat_ti-am335x/include/kconfig.h  -D__KERNEL__ -DMCK_LINUX -ffunction-sections -Wno-unused-parameter -Wno-sign-compare -Wno-attribute-warning -Wno-missing-prototypes -Wno-attributes -Wno-bad-function-cast -Wno-incompatible-pointer-types -Wno-implicit-fallthrough -Wno-type-limits

WARNINGS_MEDIUM  := -Wall
WARNINGS_FULL    := -Wextra -Wdouble-promotion

PRIVATE_INCDIR += $(SRC_DIR)/../../include/plat_ti-am335x/arch/arm/include  $(SRC_DIR)/../../include/plat_ti-am335x/arch/arm/include/generated $(SRC_DIR)/../../include/plat_ti-am335x/include
PRIVATE_INCDIR += $(SRC_DIR)/../../include/plat_ti-am335x/arch/arm/mach-omap2/include $(SRC_DIR)/../../include/plat_ti-am335x/arch/arm/plat-omap/include 


include $(L4DIR)/mk/lib.mk
include $(PKGDIR)/../dde/linux32/Makeconf
