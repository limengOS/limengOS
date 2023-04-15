/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_MMU_H
#define _ASM_X86_MMU_H

#include <linux/spinlock.h>
#include <linux/rwsem.h>
#include <linux/mutex.h>
#include <linux/atomic.h>
#include <linux/bits.h>

#ifdef CONFIG_L4
#include <asm/generic/mmu.h>
#endif /* L4 */

/* Uprobes on this MM assume 32-bit code */
#define MM_CONTEXT_UPROBE_IA32	BIT(0)
/* vsyscall page is accessible on this MM */
#define MM_CONTEXT_HAS_VSYSCALL	BIT(1)

/*
 * x86 has arch-specific MMU state beyond what lives in mm_struct.
 */
typedef struct {
	/*
	 * ctx_id uniquely identifies this mm_struct.  A ctx_id will never
	 * be reused, and zero is not a valid ctx_id.
	 */
	u64 ctx_id;

	/*
	 * Any code that needs to do any sort of TLB flushing for this
	 * mm will first make its changes to the page tables, then
	 * increment tlb_gen, then flush.  This lets the low-level
	 * flushing code keep track of what needs flushing.
	 *
	 * This is not used on Xen PV.
	 */
	atomic64_t tlb_gen;

#ifdef CONFIG_MODIFY_LDT_SYSCALL
	struct rw_semaphore	ldt_usr_sem;
	struct ldt_struct	*ldt;
#endif

#ifdef CONFIG_X86_64
	unsigned short flags;
#endif

	struct mutex lock;
	void __user *vdso;			/* vdso base address */
	const struct vdso_image *vdso_image;	/* vdso image in use */

	atomic_t perf_rdpmc_allowed;	/* nonzero if rdpmc is allowed */
#ifdef CONFIG_X86_INTEL_MEMORY_PROTECTION_KEYS
	/*
	 * One bit per protection key says whether userspace can
	 * use it or not.  protected by mmap_lock.
	 */
	u16 pkey_allocation_map;
	s16 execute_only_pkey;
#endif
#ifdef CONFIG_L4
	l4_cap_idx_t task;
	enum l4x_unmap_mode_enum l4x_unmap_mode;
#endif /* L4 */
} mm_context_t;

#define INIT_MM_CONTEXT(mm)						\
	.context = {							\
		.ctx_id = 1,						\
		.lock = __MUTEX_INITIALIZER(mm.context.lock),		\
		.task = L4_INVALID_CAP,					\
	}

void leave_mm(int cpu);
#define leave_mm leave_mm

#ifdef CONFIG_L4
#define __HAVE_ARCH_ENTER_LAZY_MMU_MODE
static inline void arch_enter_lazy_mmu_mode(void) {}
static inline void arch_leave_lazy_mmu_mode(void) { l4x_unmap_log_flush(); }
static inline void arch_flush_lazy_mmu_mode(void) { l4x_unmap_log_flush(); }
#endif /* L4 */

#endif /* _ASM_X86_MMU_H */
