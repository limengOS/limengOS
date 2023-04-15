PKGDIR        ?= ../../..
L4DIR         ?= $(PKGDIR)/../..

DDE_SYSTEMS    = x86


-include $(PKGDIR_OBJ)/Makeconf

ifeq ($(CONFIG_DDE32_COMMON),y)
TARGET         = dde32_unit_test
endif

SRC_C          = main.c

LIBS          += -ldde_linux32 -lddekit -lio -llist_alloc -lparsecmdline -lcunit_dde

# DDE configuration
include $(PKGDIR)/linux32/Makeconf

include $(L4DIR)/mk/prog.mk
