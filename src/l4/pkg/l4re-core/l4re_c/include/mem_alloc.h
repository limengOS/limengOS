/**
 * \file
 * \brief   Memory allocator C interface
 */
/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
#pragma once

#include <l4/re/env.h>
#include <l4/sys/consts.h>

#include <l4/re/c/dataspace.h>

/**
 * \defgroup api_l4re_c_mem_alloc Memory allocator
 * \ingroup api_l4re_c
 * \brief Memory allocator C interface.
 */

EXTERN_C_BEGIN

/**
 * \brief Flags for requesting memory at the memory allocator.
 * \ingroup api_l4re_c_mem_alloc
 * \see L4Re::Mem_alloc::Mem_alloc_flags
 */
enum l4re_ma_flags {
  L4RE_MA_CONTINUOUS  = 0x01,
  L4RE_MA_PINNED      = 0x02,
  L4RE_MA_SUPER_PAGES = 0x04,
};


/**
 * \brief Allocate memory
 * \ingroup api_l4re_c_mem_alloc
 *
 * \param size    Size in bytes to be requested. Allocation
 *                granularity is (super)pages, however, the allocator
 *                will store the byte-granular given size as the size
 *                of the dataspace and consecutively will use this
 *                byte-granular size for servicing the dataspace.
 *                Allocators may optionally also implement a maximum allocation
 *                strategy: if `size` is a negative value and `flags`
 *                set the Mem_alloc_flags::Continuous bit, the
 *                allocator tries to allocate as much memory as possible
 *                leaving an amount of at least `-size` bytes within the
 *                associated quota.
 * \param  mem    Capability slot where the capability to the
 *                dataspace is received.
 * \param  flags  Special dataspace properties, see #l4re_ma_flags
 *
 * \retval 0           Success
 * \retval -L4_ERANGE  Given size not supported.
 * \retval -L4_ENOMEM  Not enough memory available.
 * \retval <0          IPC error
 *
 * \see L4Re::Mem_alloc::alloc
 *
 * The memory allocator returns a dataspace.
 *
 * \note This function is using the L4Re::Env::env()->mem_alloc() service.
 */
L4_CV L4_INLINE long
l4re_ma_alloc(long size, l4re_ds_t const mem,
              unsigned long flags) L4_NOTHROW;

/**
 * \brief Allocate memory
 * \ingroup api_l4re_c_mem_alloc
 *
 * \param size    Size in bytes to be requested. Allocation
 *                granularity is (super)pages, however, the allocator
 *                will store the byte-granular given size as the size
 *                of the dataspace and consecutively will use this
 *                byte-granular size for servicing the dataspace.
 *                Allocators may optionally also implement a maximum allocation
 *                strategy: if `size` is a negative value and `flags`
 *                set the Mem_alloc_flags::Continuous bit, the
 *                allocator tries to allocate as much memory as possible
 *                leaving an amount of at least `-size` bytes within the
 *                associated quota.
 * \param  mem    Capability slot where the capability to the
 *                dataspace is received.
 * \param  flags  Special dataspace properties, see #l4re_ma_flags
 * \param  align  Log2 alignment of dataspace if supported by allocator,
 *                will be at least L4_PAGESHIFT,
 *                with Super_pages flag set at least L4_SUPERPAGESHIFT
 *
 * \retval 0           Success
 * \retval -L4_ERANGE  Given size not supported.
 * \retval -L4_ENOMEM  Not enough memory available.
 * \retval <0          IPC error
 *
 * \see L4Re::Mem_alloc::alloc and \see l4re_ma_alloc
 *
 * The memory allocator returns a dataspace.
 *
 * \note This function is using the L4Re::Env::env()->mem_alloc() service.
 */
L4_CV L4_INLINE long
l4re_ma_alloc_align(long size, l4re_ds_t const mem,
                    unsigned long flags, unsigned long align) L4_NOTHROW;

/**
 * \brief Allocate memory.
 * \ingroup api_l4re_c_mem_alloc
 *
 * \param  srv    Memory allocator service.
 * \param  size   Size to be requested.
 * \param  mem    Capability slot to put the requested dataspace in
 * \param  flags  Flags, see #l4re_ma_flags
 * \param  align  Log2 alignment of dataspace if supported by allocator,
 *                will be at least L4_PAGESHIFT,
 *                with Super_pages flag set at least L4_SUPERPAGESHIFT,
 *                default 0
 * \return 0 on success, <0 on error
 *
 * \see L4Re::Mem_alloc::alloc
 *
 * The memory allocator returns a dataspace.
 */
L4_CV long
l4re_ma_alloc_align_srv(l4_cap_idx_t srv, long size,
                        l4re_ds_t const mem, unsigned long flags,
                        unsigned long align) L4_NOTHROW;

/***************** Implementation *****************/

L4_CV L4_INLINE long
l4re_ma_alloc(long size, l4re_ds_t const mem,
              unsigned long flags) L4_NOTHROW
{
  return l4re_ma_alloc_align_srv(l4re_global_env->mem_alloc, size, mem,
                                 flags, 0);
}

L4_CV L4_INLINE long
l4re_ma_alloc_align(long size, l4re_ds_t const mem,
                    unsigned long flags, unsigned long align) L4_NOTHROW
{
  return l4re_ma_alloc_align_srv(l4re_global_env->mem_alloc, size, mem,
                                 flags, align);
}

EXTERN_C_END
