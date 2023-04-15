PKGDIR  ?= ../..
L4DIR   ?= $(PKGDIR)/../..

TARGET	      = libl4revfs-fs-sysfs.a libl4revfs-fs-sysfs.so
LINK_INCR     = libl4revfs-fs-sysfs.a libdde-linux32.a
PC_FILENAME   = libl4revfs-fs-sysfs
SRC_CC        = fs.cc

include $(L4DIR)/mk/lib.mk

CXXFLAGS += -fvisibility=hidden
CFLAGS   += -fvisibility=hidden
CXXFLAGS += -Wno-unused-function

