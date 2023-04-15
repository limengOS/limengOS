# vim:set ts=4:
PKGDIR        ?= ../../..
L4DIR         ?= $(PKGDIR)/../..

DDE_SYSTEMS    = arm

REQUIRES_LIBS  = slab ddekit dde-linux32 dde-linux32_net libio l4util l4re_c

-include $(PKGDIR_OBJ)/Makeconf

#ifeq ($(CONFIG_DDE32_NET),y)
TARGET         = smc_dde32
#endif

SRC_C			=  smc911x.c smc91x.c main.c ../ne2k/arping.c

PRIVATE_INCDIR 	= $(PKGDIR_ABS)/linux32/include $(MY_DDE_INCDIR) $(MY_LINUX32_INCDIR)  \
				  $(OBJ_BASE)/include/uclibc $(PKGDIR_ABS)/linux32/examples/ne2k

LIBCINCDIR  = -nostdinc $(I_GCCINCDIR)
DEFINES     = -D__KERNEL__ -DDDE_LINUX\
			  $(KBUILD_DEFINES)
CPPFLAGS 	+= $(KBUILD_CPPFLAGS)

ifeq ($(ARCH), arm)
ARCH_DIR = arch/arm
DEFINES += -D__LINUX_ARM_ARCH__=6
endif

include $(PKGDIR)/linux32/Makeconf

include $(L4DIR)/mk/prog.mk

foo :
	@echo $(L4INCDIR)
	@echo $(OBJ_BASE)
