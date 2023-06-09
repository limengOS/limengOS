// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2006 Andi Kleen, SUSE Labs.
 *
 * Fast user context implementation of getcpu()
 */

#include <linux/kernel.h>
#include <linux/getcpu.h>
#include <linux/time.h>
#include <asm/vgtod.h>

#ifdef CONFIG_L4
#include <asm/unistd.h>
#endif

notrace long
__vdso_getcpu(unsigned *cpu, unsigned *node, struct getcpu_cache *unused)
{
#ifdef CONFIG_L4
	unsigned int p;
	asm("syscall"
	    : "=a" (p)
	    : "0" (__NR_getcpu), "D" (cpu), "S" (node)
	    : "cc", "r11", "cx", "memory");

	return p;
#endif /* L4 */

	vdso_read_cpunode(cpu, node);

	return 0;
}

long getcpu(unsigned *cpu, unsigned *node, struct getcpu_cache *tcache)
	__attribute__((weak, alias("__vdso_getcpu")));
