/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2012 Calxeda, Inc.
 */
#ifndef _ASM_ARM_PERCPU_H_
#define _ASM_ARM_PERCPU_H_

register unsigned long current_stack_pointer asm ("sp");

/*
 * Same as asm-generic/percpu.h, except that we store the per cpu offset
 * in the TPIDRPRW. TPIDRPRW only exists on V6K and V7
 */
#if defined(CONFIG_SMP) && !defined(CONFIG_CPU_V6)

#ifdef CONFIG_L4
#include <l4/sys/thread.h>
#endif

static inline void set_my_cpu_offset(unsigned long off)
{
	/* Set TPIDRPRW */
#ifdef CONFIG_L4
	l4_thread_arm_set_tpidruro(L4_INVALID_CAP, off);
#else
	asm volatile("mcr p15, 0, %0, c13, c0, 4" : : "r" (off) : "memory");
#endif
}

static inline unsigned long __my_cpu_offset(void)
{
	unsigned long off;

	/*
	 * Read TPIDRPRW.
	 * We want to allow caching the value, so avoid using volatile and
	 * instead use a fake stack read to hazard against barrier().
	 */
#ifdef CONFIG_L4
	asm("mrc p15, 0, %0, c13, c0, 3" : "=r" (off)
		: "Q" (*(const unsigned long *)current_stack_pointer));
#else
	asm("mrc p15, 0, %0, c13, c0, 4" : "=r" (off)
		: "Q" (*(const unsigned long *)current_stack_pointer));
#endif
	return off;
}
#define __my_cpu_offset __my_cpu_offset()
#else
#define set_my_cpu_offset(x)	do {} while(0)

#endif /* CONFIG_SMP */

#include <asm-generic/percpu.h>

#endif /* _ASM_ARM_PERCPU_H_ */
