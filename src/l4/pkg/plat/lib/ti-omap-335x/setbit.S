/*
 *  linux/arch/arm/lib/setbit.S
 *
 *  Copyright (C) 1995-1996 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/linkage.h>
#include <asm/assembler.h>
#include "bitops.h"

        .data
        .globl  __pv_phys_offset
        .type   __pv_phys_offset, %object
__pv_phys_offset:
        .long   0
        .size   __pv_phys_offset, . - __pv_phys_offset

		.text

bitop	_set_bit, orr
