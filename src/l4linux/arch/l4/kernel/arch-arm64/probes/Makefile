# SPDX-License-Identifier: GPL-2.0
obj-$(CONFIG_KPROBES)		+= kprobes.o decode-insn.o	\
				   kprobes_trampoline.o		\
				   simulate-insn.o
obj-$(CONFIG_UPROBES)		+= uprobes.o decode-insn.o	\
				   simulate-insn.o

include arch/l4/Makefile.lib

$(call L4X_FILEREF, ../../../../arm64/kernel/probes, \
                    kprobes decode-insn kprobes_trampoline \
                    uprobes simulate-insn)
