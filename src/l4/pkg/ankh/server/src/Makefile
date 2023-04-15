PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

DDE_SYSTEMS = x86 arm

TARGET		= ankh
SRC_CC		= main.cc session.cc device.cc sysfs.cc

SRC_C  = linux_glue.c init.c

# switch network cards on/off here
NE2K    = n
LOOP    = y
RTL8139 = n
RTL8169 = n
PCNET   = n
E100    = n
E1000   = n
E1000E  = n
UX      = n
TI      = y

SRC_C-ARCH-x86         = ux.c
REQUIRES_LIBS-ARCH-x86 = lxfuxlibc

SRC_C-$(NE2K) += 8390.c ne2k-pci.c
SRC_C-$(LOOP) += loopback.c
SRC_C-$(RTL8139) += 8139cp.c
SRC_C-$(PCNET) += pcnet32.c
SRC_C-$(E100) += e100.c
SRC_C-$(RTL8169) += r8169.c
SRC_C-$(UX)   += $(SRC_C-ARCH-$(ARCH))

SRC_C-$(E1000) += $(addprefix e1000/, e1000_ethtool.c e1000_hw.c e1000_main.c e1000_param.c)
PRIVATE_INCDIR-$(E1000) += $(SRC_DIR)/../netlib/e1000

SRC_C-$(E1000E) += $(addprefix e1000e/, 82571.c es2lan.c ethtool.c ich8lan.c lib.c netdev.c param.c phy.c)
PRIVATE_INCDIR-$(E1000) += $(SRC_DIR)/../netlib/e1000e

SRC_C += $(SRC_C-y)

REQUIRES_LIBS += libstdc++ libsupc++ l4re-util l4re l4re_c \
				 l4re_c-util log slab libboost-lite ddekit dde-linux32_net dde-linux32_block dde-linux32_char dde-linux32_fs dde-linux32 shmc \
				 dde-linux32_pci shmc_ringbuf libankh_packet_analyzer \
				 $(REQUIRES_LIBS-ARCH-$(ARCH))


ifeq ($(TI), y)
REQUIRES_LIBS += ti-omap-335x

# for ld
LDFLAGS += -z muldefs 

# Discarded input sections
LDFLAGS += --gc-sections
endif
ifeq ($(ARCH), arm)
DEFINES += -D__LINUX_ARM_ARCH__=7   -DMCK_LINUX32
ARCH_DIR = arch/arm
endif

vpath %.c $(SRC_DIR)/../netlib

PRIVATE_INCDIR += $(SRC_DIR)/../netlib $(PRIVATE_INCDIR-y)
CONTRIB_INCIDR += boost-1_39-lite
CONTRIB_INCIDR += libio-io
CONTRIB_INCIDR += libstdc++-v3

include $(PKGDIR)/../dde/linux32/Makeconf
include $(L4DIR)/mk/prog.mk
