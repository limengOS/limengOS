PKGDIR     ?= ../../..
L4DIR      ?= $(PKGDIR)/../..
CONTRIB    ?= $(PKGDIR)/linux32/contrib

-include $(PKGDIR_OBJ)/Makeconf

# Building the main DDE library takes place incrementally. First, we build a temporary
# libdde_linux32_DONT_USE.o.a -- .o.a is used so that no functions are omitted by the
# linker. Otherwise it would remove certain Linux initcalls from the library which we
# rely on.
#
# After building the Linux support lib, we need to take care of Linux symbols that may
# also be defined in other libraries, such as LibC's str*() functions. To make sure that
# clients can use libdde along with those other libs, we use objcopy to make all symbols
# be defined weakly. Only those functions that are needed by drivers outside libdde_linux
# are explicitly made global. (To add a function to be exported, put it into one of the
# global-symbols.{all, arm, x86} files in the dde/linux32/lib/src directory.)

TARGET_DDE       = libdde-linux32.a
TARGET_DDE_NET   = libdde-linux32_net.a
TARGET_DDE_CHAR  = libdde-linux32_char.a

#ifeq ($(CONFIG_DDE32_BLOCK),y)
TARGET_DDE_BLOCK = libdde-linux32_block.a
TARGET_DDE_PCI = libdde-linux32_pci.a
#endif

ifeq ($(CONFIG_DDE32_SOUND),y)
TARGET_DDE_SOUND = libdde-linux32_sound.a
endif

TARGET_DDE_FS = libdde-linux32_fs.a

DDE_SYSTEMS = x86 arm

ifeq ($(ARCH), x86)
ARCH_DIR = arch/x86
endif

ifeq ($(ARCH), arm)
ARCH_DIR = arch/arm
MARCH = mach-omap2
DEFINES += -D__LINUX_ARM_ARCH__=7
endif

PC_FILENAME = dde-linux32
PC_FILENAMES                 := $(PC_FILENAME) $(PC_FILENAME)_net $(PC_FILENAME)_block $(PC_FILENAME)_char $(PC_FILENAME)_fs $(PC_FILENAME)_pci
PC_LIBS_$(PC_FILENAME)       := $(patsubst lib%.a,-l%,$(TARGET_DDE))
PC_LIBS_$(PC_FILENAME)_net   := $(patsubst lib%.a,-l%,$(TARGET_DDE_NET))
PC_LIBS_$(PC_FILENAME)_block := $(patsubst lib%.a,-l%,$(TARGET_DDE_BLOCK))
PC_LIBS_$(PC_FILENAME)_pci   := $(patsubst lib%.a,-l%,$(TARGET_DDE_PCI))
PC_LIBS_$(PC_FILENAME)_sound := $(patsubst lib%.a,-l%,$(TARGET_DDE_SOUND))
PC_LIBS_$(PC_FILENAME)_char  := $(patsubst lib%.a,-l%,$(TARGET_DDE_CHAR))
PC_LIBS_$(PC_FILENAME)_fs := $(patsubst lib%.a,-l%,$(TARGET_DDE_FS))
REQUIRES_LIBS := ddekit

TARGET     = $(TARGET_DDE) $(TARGET_DDE_NET) $(TARGET_DDE_BLOCK) $(TARGET_DDE_SOUND) $(TARGET_DDE_CHAR) $(TARGET_DDE_FS) $(TARGET_DDE_PCI)
LINK_INCR  = $(TARGET_DDE) $(TARGET_DDE_BLOCK)
LINK_INCR_ONLYGLOBSYMFILE_$(TARGET_DDE) = $(SRC_DIR)/global-symbols.all \
                                          $(SRC_DIR)/global-symbols.$(ARCH)

# contrib sources are in $(CONTRIB)
vpath %.c $(CONTRIB)
vpath %.S $(CONTRIB)

PRIVATE_INCDIR += $(CONTRIB)/drivers/pci $(PKGDIR)/linux32/lib/src/arch/l4 \
                  $(CONTRIB)/$(ARCH_DIR)/pci $(CONTRIB)/drivers/base/ \
                  $(CONTRIB)/lib $(PKGDIR_OBJ) $(CONTRIB)/net/core $(CONTRIB)/fs \
                  $(CONTRIB)/$(ARCH_DIR)/mach-omap2/include  $(CONTRIB)/$(ARCH_DIR)/plat-omap/include


##################################################################
# Sources for libdde_linux.a                                     #
##################################################################
SRC_DDE = cli_sti.c fs.c hw-helpers.c init_task.c init.c power.c \
          process.c res.c sched.c signal.c smp.c softirq.c timer.c \
          page_alloc.c  kmalloc.c irq.c param.c \
          vmalloc.c vmstat.c mm-helper.c dummy.c

# our implementation
SRC_C_$(TARGET_DDE) = $(addprefix arch/l4/, $(SRC_DDE))

ifeq ($(ARCH), x86)
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/semaphore_32.S
SRC_C_$(TARGET_DDE) += lib/rwsem.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/kernel/pci-dma.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/kernel/pci-nommu.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/lib/string_32.c
SRC_S_$(TARGET_DDE_NET) += $(ARCH_DIR)/lib/checksum_32.S
endif

ifeq ($(ARCH), arm)
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/changebit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/clearbit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/div64.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/findbit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/memzero.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/setbit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/testclearbit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/testchangebit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/testsetbit.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/io-readsb.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/io-readsl.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/io-readsw-armv4.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/io-writesb.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/io-writesl.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/io-writesw-armv4.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/csumpartial.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/putuser.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/getuser.S
SRC_S_$(TARGET_DDE) += $(ARCH_DIR)/lib/delay.S
#SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/kernel/semaphore.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/kernel/traps.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/kernel/io.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/mm/flush.c
SRC_C_$(TARGET_DDE) += $(ARCH_DIR)/mm/dma-mapping.c
SRC_C_$(TARGET_DDE) += lib/rwsem-spinlock.c
SRC_C_$(TARGET_DDE) += drivers/amba/bus.c
endif

# + contrib stuff / slightly modified stuff
SRC_C_$(TARGET_DDE) += \
                          kernel/exit.c \
                          kernel/kthread.c \
                          kernel/mutex.c \
                          kernel/rcutiny.c \
                          kernel/rcupdate.c \
                          kernel/srcu.c \
                          kernel/notifier.c \
                          kernel/resource.c \
                          kernel/rwsem.c \
                          kernel/sched.c \
                          kernel/semaphore.c \
                          kernel/sys.c \
                          kernel/time.c \
                          kernel/time/timekeeping.c \
                          kernel/timer.c \
                          kernel/wait.c \
                          kernel/workqueue.c \
                          kernel/capability.c \
                          kernel/cred.c \
                          kernel/power/main.c \
                          kernel/panic.c \
                          kernel/rtmutex.c \
                          kernel/freezer.c \
                          kernel/params.c \
                          kernel/acct.c   \
                          lib/bitmap.c \
                          lib/bitrev.c \
                          lib/crc32.c \
                          lib/crc16.c \
                          lib/ctype.c \
                          lib/cpumask.c \
                          lib/dec_and_lock.c \
                          lib/find_next_bit.c \
                          lib/hexdump.c \
                          lib/idr.c \
                          lib/iomap.c \
                          lib/hweight.c \
                          lib/kasprintf.c \
                          lib/klist.c \
                          lib/kobject.c \
                          lib/kref.c \
                          lib/parser.c \
                          lib/proportions.c \
                          lib/radix-tree.c \
                          lib/rbtree.c \
                          lib/scatterlist.c \
                          lib/sha1.c \
                          lib/string.c \
                          lib/vsprintf.c \
                          lib/kstrtox.c \
                          lib/nlattr.c \
                          lib/dynamic_debug.c \
                          lib/lzo/lzo1x_decompress.c \
                          lib/lzo/lzo1x_compress.c \
                          lib/sort.c \
                          lib/int_sqrt.c \
                          lib/div64.c  \
                          lib/reciprocal_div.c \
                          lib/string_helpers.c \
                          mm/dmapool.c \
                          mm/mempool.c \
                          mm/swap.c \
                          mm/util.c \
                          mm/percpu.c \
                          mm/truncate.c \
                          mm/vmscan.c \
                          mm/page_alloc.c \
                          mm/mmzone.c \
                          mm/rmap.c \
                          mm/slab.c \
                          drivers/base/attribute_container.c \
                          drivers/base/bus.c \
                          drivers/base/class.c \
                          drivers/base/core.c \
                          drivers/base/cpu.c \
                          drivers/base/dd.c \
                          drivers/base/devres.c \
                          drivers/base/driver.c \
                          drivers/base/init.c \
                          drivers/base/map.c \
                          drivers/base/platform.c \
                          drivers/base/sys.c \
                          drivers/base/syscore.c \
                          drivers/base/power/main.c \
                          drivers/base/power/wakeup.c \
                          drivers/base/power/runtime.c \
                          drivers/base/power/generic_ops.c \
                          drivers/base/regmap/regcache.c    \
                          drivers/base/regmap/regcache-indexed.c    \
                          drivers/base/regmap/regcache-lzo.c    \
                          drivers/base/regmap/regcache-rbtree.c \
                          drivers/base/regmap/regmap.c  \
                          drivers/base/regmap/regmap-i2c.c  \
                          drivers/char/random.c \
                          drivers/clk/clkdev.c \
                          drivers/char/misc.c \
                          drivers/of/device.c \
                          drivers/of/base.c \
                          drivers/i2c/i2c-core.c \
                          drivers/i2c/i2c-boardinfo.c \
                          drivers/regulator/core.c \
                          drivers/regulator/dummy.c \
                          drivers/regulator/fixed.c \
                          drivers/gpio/gpiolib.c \
                          drivers/mfd/mfd-core.c \
                          drivers/gpio/gpio-tps65910.c \
                          drivers/gpio/gpio-omap.c \
                          drivers/of/gpio.c 
##################################################################
# Sources for libdde_linux_net.a                                 #
##################################################################
SRC_C_$(TARGET_DDE_NET) += \
                              arch/l4/net.c \
                              drivers/net/mii.c \
                              net/core/dev.c \
                              net/core/dev_addr_lists.c \
                              net/core/ethtool.c \
                              net/core/link_watch.c \
                              net/core/neighbour.c \
                              net/core/netevent.c \
                              net/core/net-sysfs.c \
                              net/core/net_namespace.c \
                              net/core/rtnetlink.c \
                              net/core/skbuff.c \
                              net/core/utils.c \
                              net/core/dst.c \
                              net/ethernet/eth.c \
                              net/sched/sch_generic.c \
                              net/sched/sch_mq.c \
                              net/sched/gen_stats.c 

##################################################################
# Sources for libdde_linux_sound.a                               #
##################################################################
SRC_C_$(TARGET_DDE_SOUND) += \
                                sound/sound_core.c \
                                arch/l4/sound.c

##################################################################
# Sources for libdde_linux_block.a                               #
##################################################################
SRC_C_$(TARGET_DDE_BLOCK) += \
                                fs/inode.c               \
                                block/blk-flush.c   \
                                block/blk-lib.c   \
                                block/blk-core.c      \
                                block/blk-exec.c      \
                                block/blk-ioc.c       \
                                block/blk-iopoll.c       \
                                block/blk-map.c              \
                                block/blk-merge.c      \
                                block/blk-settings.c      \
                                block/blk-softirq.c \
                                block/blk-sysfs.c      \
                                block/blk-tag.c      \
                                block/blk-timeout.c      \
                                block/elevator.c      \
                                block/genhd.c         \
                                block/noop-iosched.c  \
                                block/cfq-iosched.c  \
                                block/ioctl.c         \
                                block/scsi_ioctl.c    \
                                lib/hexdump.c          \
                                lib/bcd.c             \
                                lib/list_sort.c       \
                                mm/backing-dev.c      \
                                mm/bounce.c           \
                                mm/page-writeback.c   \
                                mm/filemap.c          \
                                mm/readahead.c        \
                                fs/bio.c              \
                                fs/block_dev.c        \
                                fs/buffer.c           \
                                fs/filesystems.c      \
                                fs/partitions/check.c \
                                fs/file_table.c       \
                                fs/ramfs/inode.c      \
                                fs/ramfs/file-mmu.c   \
                                fs/fs_struct.c        \
                                fs/file.c             \
                                fs/seq_file.c         \
                                fs/posix_acl.c        \
                                fs/locks.c          \
                                fs/fcntl.c          \
                                security/commoncap.c    \
                                fs/pnode.c \
                                fs/ioprio.c
#endif

#XXX lib/vsprintf.c is already declared in TARGET_DDE. Don't understand why
# should it be declared in TARGET_DDE_BLOCK

#ifeq ($(ARCH), x86)
SRC_C_$(TARGET_DDE_BLOCK) += \
                                fs/partitions/msdos.c
#endif

##################################################################
# Sources for libdde_linux_char.a                                #
##################################################################
SRC_C_$(TARGET_DDE_CHAR) += \
                                fs/char_dev.c
                                #arch/l4/inodes.c      \

##################################################################
# Sources for libdde_linux_pci.a                                 #
##################################################################
SRC_C_$(TARGET_DDE_PCI) +=  \
                          arch/l4/pci.c \
                          drivers/pci/access.c \
                          drivers/pci/bus.c \
                          drivers/pci/hotplug-pci.c \
                          drivers/pci/pci.c \
                          drivers/pci/pci-driver.c \
                          drivers/pci/probe.c \
                          drivers/pci/search.c \
                          drivers/pci/setup-bus.c \
                           #drivers/pci/setup-res.c


##################################################################
# Sources for libdde_linux_fs.a                                  #
##################################################################
SRC_C_$(TARGET_DDE_FS) += \
                                fs/attr.c \
                                fs/bad_inode.c \
                                fs/dcache.c \
                                fs/libfs.c \
                                fs/mpage.c \
                                fs/namei.c \
                                fs/namespace.c \
                                fs/open.c \
                                fs/read_write.c \
                                fs/super.c \
                                fs/splice.c \
                                fs/stat.c \
                                fs/fs-writeback.c \
                                fs/sync.c
#                                fs/direct-io.c

# # Warning! Following files are also included in _DDE_BLOCK
# SRC_C_$(TARGET_DDE_FS) += \
#                                 fs/block_dev.c

##################################################################
# DDE specific build rules                                       #
##################################################################

all::

lib/crc32.o : crc32table.h
lib/crc32.o : PRIVATE_INCDIR += .
kernel/time.o : timeconst.h
kernel/time.o : PRIVATE_INCDIR += .

timeconst.h : $(SRC_DIR)/kernel/timeconst.pl
	@$(GEN_MESSAGE)
	$(VERBOSE)$< 250 >$@

crc32table.h : gen_crc32table
	@$(GEN_MESSAGE)
	$(VERBOSE)./$< >$@

gen_crc32table : lib/gen_crc32table.c
	@$(GEN_MESSAGE)
	$(VERBOSE)$(HOST_CC) -O2 -o $@ $<

WARNINGS_MEDIUM  := -Wall
WARNINGS_FULL    := -Wextra -Wdouble-promotion
CFLAGS := $(filter-out -std=gnu99,$(CFLAGS))
CFLAGS += -Wno-unused-parameter -Wno-sign-compare -Wno-attribute-warning -Wno-missing-prototypes -Wno-attributes -Wno-bad-function-cast -Wno-incompatible-pointer-types -Wno-implicit-fallthrough -Wno-type-limits

include $(PKGDIR)/linux32/Makeconf

include $(L4DIR)/mk/lib.mk

