/**
 * \file
 * Common task related definitions.
 */
/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>,
 *               Björn Döbel <doebel@os.inf.tu-dresden.de>,
 *               Torsten Frenzel <frenzel@os.inf.tu-dresden.de>
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
#include <l4/sys/types.h>
#include <l4/sys/utcb.h>

/**
 * \defgroup l4_task_api Task
 * \ingroup  l4_kernel_object_api
 * C interface of the Task kernel object, see L4::Task for the C++ interface.
 *
 * A task represents a combination of the address spaces provided
 * by the L4Re micro kernel. A task consists of at least a memory address space
 * and an object address space. On IA32 there is also an IO-port address space.
 *
 * Task objects are created using the \ref l4_factory_api interface.
 *
 * \includefile{l4/sys/task.h}
 */

/**
 * Map resources available in the source task to a destination task.
 * \ingroup l4_task_api
 *
 * \param dst_task   Capability selector of the destination task.
 * \param src_task   Capability selector of the source task.
 * \param snd_fpage  Send flexpage that describes an area in the address
 *                   space or object space of the source task.
 * \param snd_base   Send base that describes an offset in the receive window
 *                   of the destination task. The lower bits contain additional
 *                   map control flags (see #l4_fpage_cacheability_opt_t for
 *                   memory mappings, #L4_obj_fpage_ctl for object mappings,
 *                   and #L4_MAP_ITEM_GRANT; also see l4_map_control() and
 *                   l4_map_obj_control()).
 *
 * \return Syscall return tag.
 *
 * This method allows for asynchronous transfer of capabilities, memory
 * mappings, and IO-port mappings (on IA32) from one task to another.
 * The receive window is the whole address space of `dst_task`.
 *
 * For more information on spaces and mappings, see
 * \ref l4re_concepts_mapping. The flexpage API is described in more detail at
 * \ref l4_fpage_api.
 */
L4_INLINE l4_msgtag_t
l4_task_map(l4_cap_idx_t dst_task, l4_cap_idx_t src_task,
            l4_fpage_t snd_fpage, l4_umword_t snd_base) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_map_u(l4_cap_idx_t dst_task, l4_cap_idx_t src_task,
              l4_fpage_t snd_fpage, l4_umword_t snd_base, l4_utcb_t *utcb) L4_NOTHROW;

/**
 * Revoke rights from the task.
 * \ingroup l4_task_api
 *
 * \param task          Capability selector of destination task
 * \param fpage         Flexpage that describes an area in one capability space
 *                      of the destination task
 * \param map_mask      Unmap mask, see #l4_unmap_flags_t
 *
 * \return Syscall return tag
 *
 * This method allows to revoke rights from the destination task. For a flex
 * page describing IO ports or memory, it also revokes rights from all the
 * tasks that got the rights delegated from the destination task (i.e., this
 * operation does a recursive rights revocation). If the set of rights is empty
 * after the revocation, the capability is unmapped.
 *
 * \note If the reference counter of a kernel object referenced in `fpage`
 *       goes down to zero (as a result of deleting capabilities), the deletion
 *       of the object is initiated. Objects are not destroyed until all other
 *       kernel objects holding a reference to it drop the reference.
 */
L4_INLINE l4_msgtag_t
l4_task_unmap(l4_cap_idx_t task, l4_fpage_t fpage,
              l4_umword_t map_mask) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_unmap_u(l4_cap_idx_t task, l4_fpage_t fpage,
                l4_umword_t map_mask, l4_utcb_t *utcb) L4_NOTHROW;

/**
 * Revoke rights from a task.
 * \ingroup l4_task_api
 *
 * \param task          Capability selector of destination task
 * \param fpages        An array of flexpages. Each item describes an area in
 *                      one capability space of the destination task.
 * \param num_fpages    The size of the fpages array in elements (number of
 *                      fpages sent).
 * \param map_mask      Unmap mask, see #l4_unmap_flags_t
 *
 * \return Syscall return tag
 *
 * Revoke rights specified in an array of flexpages, see #l4_task_unmap for
 * details.
 *
 * \pre The caller needs to take care that num_fpages is not bigger
 *      than L4_UTCB_GENERIC_DATA_SIZE - 2.
 */
L4_INLINE l4_msgtag_t
l4_task_unmap_batch(l4_cap_idx_t task, l4_fpage_t const *fpages,
                    unsigned num_fpages, l4_umword_t map_mask) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_unmap_batch_u(l4_cap_idx_t task, l4_fpage_t const *fpages,
                      unsigned num_fpages, l4_umword_t map_mask,
                      l4_utcb_t *u) L4_NOTHROW;

/**
 * Release capability and delete object.
 * \ingroup l4_task_api
 *
 * \param task          Capability selector of destination task
 * \param obj           Capability selector of object to delete
 *
 * \return Syscall return tag
 *
 * Initiates the deletion of the object if `obj` has the delete permission.
 * Objects are not destroyed until all other kernel objects holding a reference
 * to it drop the reference. No error will be reported if the rights are
 * insufficient, however, the capability is removed in all cases from the
 * destination task.
 *
 * This operation calls l4_task_unmap() with #L4_FP_DELETE_OBJ.
 */
L4_INLINE l4_msgtag_t
l4_task_delete_obj(l4_cap_idx_t task, l4_cap_idx_t obj) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_delete_obj_u(l4_cap_idx_t task, l4_cap_idx_t obj,
                     l4_utcb_t *u) L4_NOTHROW;

/**
 * Release object capability.
 * \ingroup l4_task_api
 *
 * \param task          Capability selector of destination task
 * \param cap           Capability selector of object to release
 *
 * \return Syscall return tag
 *
 * This operation unmaps the capability from the specified task.
 *
 * \note If the reference counter of the kernel object referenced by `cap`
 *       goes down to zero, deletion of the object is initiated. Objects are not
 *       destroyed until all other kernel objects holding a reference to it drop
 *       the reference.
 */
L4_INLINE l4_msgtag_t
l4_task_release_cap(l4_cap_idx_t task, l4_cap_idx_t cap) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_release_cap_u(l4_cap_idx_t task, l4_cap_idx_t cap,
                      l4_utcb_t *u) L4_NOTHROW;


/**
 * Check whether a capability is present (refers to an object).
 * \ingroup l4_task_api
 *
 * \param task  Task to check the capability in.
 * \param cap   Valid capability to check for presence.
 *
 * \retval tag.label() > 0   Capability is present (refers to an object).
 * \retval tag.label() == 0  No capability present (void object).
 *
 * A capability is considered present when it refers to an existing
 * kernel object.
 *
 * \pre `cap` must be a valid capability index (i.e. not L4_INVALID_CAP or
 *      the like).
 */
L4_INLINE l4_msgtag_t
l4_task_cap_valid(l4_cap_idx_t task, l4_cap_idx_t cap) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_cap_valid_u(l4_cap_idx_t task, l4_cap_idx_t cap, l4_utcb_t *utcb) L4_NOTHROW;

/**
 * Test whether two capabilities point to the same object with the same
 *        rights.
 * \ingroup l4_task_api
 *
 * \param task         Capability selector of the destination task to do the
 *                     lookup in
 * \param cap_a        Capability selector to compare
 * \param cap_b        Capability selector to compare
 *
 * \return label contains 1 if equal, 0 if not equal
 */
L4_INLINE l4_msgtag_t
l4_task_cap_equal(l4_cap_idx_t task, l4_cap_idx_t cap_a,
                  l4_cap_idx_t cap_b) L4_NOTHROW;

/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_add_ku_mem_u(l4_cap_idx_t task, l4_fpage_t ku_mem,
                     l4_utcb_t *u) L4_NOTHROW;

/**
 * Add kernel-user memory.
 * \ingroup l4_task_api
 *
 * \param task    Capability selector of the task to add the memory to
 * \param ku_mem  Flexpage describing the virtual area the memory goes to.
 *
 * \return Syscall return tag
 */
L4_INLINE l4_msgtag_t
l4_task_add_ku_mem(l4_cap_idx_t task, l4_fpage_t ku_mem) L4_NOTHROW;


/**
 * \internal
 */
L4_INLINE l4_msgtag_t
l4_task_cap_equal_u(l4_cap_idx_t task, l4_cap_idx_t cap_a,
                    l4_cap_idx_t cap_b, l4_utcb_t *utcb) L4_NOTHROW;

/**
 * Operations on task objects.
 * \ingroup l4_protocol_ops
 */
enum L4_task_ops
{
  L4_TASK_MAP_OP           = 0UL,    /**< Map */
  L4_TASK_UNMAP_OP         = 1UL,    /**< Unmap */
  L4_TASK_CAP_INFO_OP      = 2UL,    /**< Cap info */
  L4_TASK_ADD_KU_MEM_OP    = 3UL,    /**< Add kernel-user memory */
  L4_TASK_LDT_SET_X86_OP   = 0x11UL, /**< x86: LDT set */
  L4_TASK_MAP_VGICC_ARM_OP = 0x12UL, /**< Arm: Map virtual GICC area */
};


/* IMPLEMENTATION -----------------------------------------------------------*/

#include <l4/sys/ipc.h>


L4_INLINE l4_msgtag_t
l4_task_map_u(l4_cap_idx_t dst_task, l4_cap_idx_t src_task,
              l4_fpage_t snd_fpage, l4_umword_t snd_base, l4_utcb_t *u) L4_NOTHROW
{
  l4_msg_regs_t *v = l4_utcb_mr_u(u);
  v->mr[0] = L4_TASK_MAP_OP;
  v->mr[3] = l4_map_obj_control(0,0);
  v->mr[4] = l4_obj_fpage(src_task, 0, L4_CAP_FPAGE_RWS).raw;
  v->mr[1] = snd_base;
  v->mr[2] = snd_fpage.raw;
  return l4_ipc_call(dst_task, u, l4_msgtag(L4_PROTO_TASK, 3, 1, 0), L4_IPC_NEVER);
}

L4_INLINE l4_msgtag_t
l4_task_unmap_u(l4_cap_idx_t task, l4_fpage_t fpage,
                l4_umword_t map_mask, l4_utcb_t *u) L4_NOTHROW
{
  l4_msg_regs_t *v = l4_utcb_mr_u(u);
  v->mr[0] = L4_TASK_UNMAP_OP;
  v->mr[1] = map_mask;
  v->mr[2] = fpage.raw;
  return l4_ipc_call(task, u, l4_msgtag(L4_PROTO_TASK, 3, 0, 0), L4_IPC_NEVER);
}

L4_INLINE l4_msgtag_t
l4_task_unmap_batch_u(l4_cap_idx_t task, l4_fpage_t const *fpages,
                      unsigned num_fpages, l4_umword_t map_mask,
                      l4_utcb_t *u) L4_NOTHROW
{
  l4_msg_regs_t *v = l4_utcb_mr_u(u);
  v->mr[0] = L4_TASK_UNMAP_OP;
  v->mr[1] = map_mask;
  __builtin_memcpy(&v->mr[2], fpages, num_fpages * sizeof(l4_fpage_t));
  return l4_ipc_call(task, u, l4_msgtag(L4_PROTO_TASK, 2 + num_fpages, 0, 0), L4_IPC_NEVER);
}

L4_INLINE l4_msgtag_t
l4_task_cap_valid_u(l4_cap_idx_t task, l4_cap_idx_t cap, l4_utcb_t *u) L4_NOTHROW
{
  l4_msg_regs_t *v = l4_utcb_mr_u(u);
  v->mr[0] = L4_TASK_CAP_INFO_OP;
  v->mr[1] = cap & ~1UL;
  return l4_ipc_call(task, u, l4_msgtag(L4_PROTO_TASK, 2, 0, 0), L4_IPC_NEVER);
}

L4_INLINE l4_msgtag_t
l4_task_cap_equal_u(l4_cap_idx_t task, l4_cap_idx_t cap_a,
                    l4_cap_idx_t cap_b, l4_utcb_t *u) L4_NOTHROW
{
  l4_msg_regs_t *v = l4_utcb_mr_u(u);
  v->mr[0] = L4_TASK_CAP_INFO_OP;
  v->mr[1] = cap_a;
  v->mr[2] = cap_b;
  return l4_ipc_call(task, u, l4_msgtag(L4_PROTO_TASK, 3, 0, 0), L4_IPC_NEVER);
}

L4_INLINE l4_msgtag_t
l4_task_add_ku_mem_u(l4_cap_idx_t task, l4_fpage_t ku_mem,
                     l4_utcb_t *u) L4_NOTHROW
{
  l4_msg_regs_t *v = l4_utcb_mr_u(u);
  v->mr[0] = L4_TASK_ADD_KU_MEM_OP;
  v->mr[1] = ku_mem.raw;
  return l4_ipc_call(task, u, l4_msgtag(L4_PROTO_TASK, 2, 0, 0), L4_IPC_NEVER);
}



L4_INLINE l4_msgtag_t
l4_task_map(l4_cap_idx_t dst_task, l4_cap_idx_t src_task,
            l4_fpage_t snd_fpage, l4_umword_t snd_base) L4_NOTHROW
{
  return l4_task_map_u(dst_task, src_task, snd_fpage, snd_base, l4_utcb());
}

L4_INLINE l4_msgtag_t
l4_task_unmap(l4_cap_idx_t task, l4_fpage_t fpage,
              l4_umword_t map_mask) L4_NOTHROW
{
  return l4_task_unmap_u(task, fpage, map_mask, l4_utcb());
}

L4_INLINE l4_msgtag_t
l4_task_unmap_batch(l4_cap_idx_t task, l4_fpage_t const *fpages,
                    unsigned num_fpages, l4_umword_t map_mask) L4_NOTHROW
{
  return l4_task_unmap_batch_u(task, fpages, num_fpages, map_mask,
                               l4_utcb());
}

L4_INLINE l4_msgtag_t
l4_task_delete_obj_u(l4_cap_idx_t task, l4_cap_idx_t obj,
                     l4_utcb_t *u) L4_NOTHROW
{
  return l4_task_unmap_u(task, l4_obj_fpage(obj, 0, L4_CAP_FPAGE_RWSD),
                         L4_FP_DELETE_OBJ, u);
}

L4_INLINE l4_msgtag_t
l4_task_delete_obj(l4_cap_idx_t task, l4_cap_idx_t obj) L4_NOTHROW
{
  return l4_task_delete_obj_u(task, obj, l4_utcb());
}


L4_INLINE l4_msgtag_t
l4_task_release_cap_u(l4_cap_idx_t task, l4_cap_idx_t cap,
                      l4_utcb_t *u) L4_NOTHROW
{
  return l4_task_unmap_u(task, l4_obj_fpage(cap, 0, L4_CAP_FPAGE_RWSD),
                         L4_FP_ALL_SPACES, u);
}

L4_INLINE l4_msgtag_t
l4_task_release_cap(l4_cap_idx_t task, l4_cap_idx_t cap) L4_NOTHROW
{
  return l4_task_release_cap_u(task, cap, l4_utcb());
}

L4_INLINE l4_msgtag_t
l4_task_cap_valid(l4_cap_idx_t task, l4_cap_idx_t cap) L4_NOTHROW
{
  return l4_task_cap_valid_u(task, cap, l4_utcb());
}

L4_INLINE l4_msgtag_t
l4_task_cap_equal(l4_cap_idx_t task, l4_cap_idx_t cap_a,
                  l4_cap_idx_t cap_b) L4_NOTHROW
{
  return l4_task_cap_equal_u(task, cap_a, cap_b, l4_utcb());
}

L4_INLINE l4_msgtag_t
l4_task_add_ku_mem(l4_cap_idx_t task, l4_fpage_t ku_mem) L4_NOTHROW
{
  return l4_task_add_ku_mem_u(task, ku_mem, l4_utcb());
}
