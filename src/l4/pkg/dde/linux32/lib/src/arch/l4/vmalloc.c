/*
 * This file is part of DDE/Linux2.6.
 *
 * (c) 2006-2012 Bjoern Doebel <doebel@os.inf.tu-dresden.de>
 *               Christian Helmuth <ch12@os.inf.tu-dresden.de>
 *     economic rights: Technische Universitaet Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */

/*
 * \brief    vmalloc implementation
 */

/* Linux */
#include <linux/vmalloc.h>
#include <linux/gfp.h>

/* DDEKit */
#include <l4/dde/ddekit/memory.h>
#include <l4/dde/ddekit/lock.h>

void *vmalloc(unsigned long size)
{
#ifndef DDE_LINUX32     //for use __kmalloc
	return ddekit_simple_malloc(size);
#else
	return  __kmalloc(size, GFP_KERNEL);
#endif
}

void vfree(const void *addr)
{
#ifndef DDE_LINUX32     //for use kfree
	ddekit_simple_free((void*)addr);
#else
	kfree((void*)addr);
#endif
}

void *vzalloc(unsigned long size)
{
        return __kmalloc(size, GFP_KERNEL | __GFP_ZERO);
}

