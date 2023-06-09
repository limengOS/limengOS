# SPDX-License-Identifier: GPL-2.0
lib-y		:= delay.o \
		   copy_page.o            \
		   clear_page.o csum.o insn.o memchr.o memcpy.o		\
		   memset.o memcmp.o strcmp.o strncmp.o strlen.o	\
		   strnlen.o strchr.o strrchr.o tishift.o

ifeq ($(CONFIG_KERNEL_MODE_NEON), y)
obj-$(CONFIG_XOR_BLOCKS)	+= xor-neon.o
CFLAGS_REMOVE_xor-neon.o	+= -mgeneral-regs-only
CFLAGS_xor-neon.o		+= -ffreestanding
endif

lib-$(CONFIG_ARCH_HAS_UACCESS_FLUSHCACHE) += uaccess_flushcache.o

obj-$(CONFIG_CRC32) += crc32.o

obj-$(CONFIG_FUNCTION_ERROR_INJECTION) += error-inject.o

obj-$(CONFIG_ARM64_MTE) += mte.o

obj-$(CONFIG_KASAN_SW_TAGS) += kasan_sw_tags.o

include arch/l4/Makefile.lib

lib-y   += ../pte.o ../memcpy_user.o ../uaccess.o
obj-y   += ../proc.o
$(info must also not be compiled when !CONFIG_DEBUG_FS)
obj-$(CONFIG_L4_DEBUG) += ../debugfs.o

$(call L4X_FILEREF, \
       ../../../arm64/lib, \
       crc32 error-inject mte)

lib-y := $(call L4X_PREFIX_IF, \
	        ../../../arm64/lib, $(lib-y), \
                clear_page copy_page csum delay insn memchr memcmp \
		memcpy memset strchr strcmp strlen \
		strncmp strnlen strrchr tishift)
