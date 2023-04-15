/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PGTABLE_32_H
#define _ASM_X86_PGTABLE_32_H

#include <asm/pgtable_32_types.h>

/*
 * The Linux memory management assumes a three-level page table setup. On
 * the i386, we use that, but "fold" the mid level into the top-level page
 * table, so that we physically have the same two-level page table as the
 * i386 mmu expects.
 *
 * This file contains the functions and defines necessary to modify and use
 * the i386 page table tree.
 */
#ifndef __ASSEMBLY__
#include <asm/processor.h>
#include <linux/threads.h>
#include <asm/paravirt.h>

#include <linux/bitops.h>
#include <linux/list.h>
#include <linux/spinlock.h>

#include <asm/api/api.h>

struct mm_struct;
struct vm_area_struct;

extern pgd_t swapper_pg_dir[1024];
extern pgd_t initial_page_table[1024];
extern pmd_t initial_pg_pmd[];

void paging_init(void);
void sync_initial_page_table(void);

#ifdef CONFIG_X86_PAE
# include <asm/pgtable-3level.h>
#else
# include <asm/pgtable-2level.h>
#endif

#undef set_pte
#undef set_pte_at
#undef pte_clear

/*
 * L4Linux uses this hook to synchronize the Linux page tables with
 * the real hardware page tables kept by the L4 kernel.
 */
unsigned long l4x_set_pte(struct mm_struct *mm, unsigned long addr, pte_t pteptr, pte_t pteval);
void          l4x_pte_clear(struct mm_struct *mm, unsigned long addr, pte_t ptep);

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
unsigned long l4x_set_pmd(struct mm_struct *mm, unsigned long addr, pmd_t pmdptr, pmd_t pmdval);
void          l4x_pmd_clear(struct mm_struct *mm, unsigned long addr, pmd_t pmdp);
#endif

static inline void __l4x_set_pte(struct mm_struct *mm, unsigned long addr,
                                 pte_t *pteptr, pte_t pteval)
{
	if (pte_val(*pteptr) & _PAGE_PRESENT)
		pteval.pte_low = l4x_set_pte(mm, addr, *pteptr, pteval);
	*pteptr = pteval;
}

static inline int
l4x_pmd_present(pmd_t *pmdptr)
{
	return (pmd_val(*pmdptr) & (_PAGE_PRESENT | _PAGE_PSE)) == (_PAGE_PRESENT | _PAGE_PSE);
}

static inline void __l4x_set_pmd(struct mm_struct *mm, unsigned long addr,
                                 pmd_t *pmdptr, pmd_t pmdval)
{
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	if (l4x_pmd_present(pmdptr) && pmd_large(*pmdptr))
		pmdval.pud.pgd = native_make_pgd(l4x_set_pmd(mm, addr, *pmdptr, pmdval));
#endif
	*pmdptr = pmdval;
}

static inline void __l4x_set_pud(struct mm_struct *mm, unsigned long addr,
                                 pud_t *pudptr, pud_t pudval)
{
	*pudptr = pudval;
}

static inline void set_pte(pte_t *pteptr, pte_t pteval)
{
	__l4x_set_pte(NULL, 0, pteptr, pteval);
}

static inline void pte_clear(struct mm_struct *mm, unsigned long addr, pte_t *ptep)
{
	if (pte_val(*ptep) & _PAGE_PRESENT)
		l4x_pte_clear(mm, addr, *ptep);
	ptep->pte_low = 0;
}

#ifdef CONFIG_L4
static inline void native_set_pmd(pmd_t *pmdp, pmd_t pmd)
{
	__l4x_set_pmd(NULL, 0, pmdp, pmd);
}

static inline void native_pmd_clear(pmd_t *pmd)
{
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	if (l4x_pmd_present(pmd))
		l4x_pmd_clear(NULL, 0, *pmd);
#endif
	native_set_pmd(pmd, (pmd_t) { { { { 0 } } } });
}
#endif /* L4 */

/* Clear a kernel PTE and flush it from the TLB */
#define kpte_clear_flush(ptep, vaddr)		\
do {						\
	pte_clear(&init_mm, (vaddr), (ptep));	\
	flush_tlb_one_kernel((vaddr));		\
} while (0)

#endif /* !__ASSEMBLY__ */

/*
 * kern_addr_valid() is (1) for FLATMEM and (0) for SPARSEMEM
 */
#ifdef CONFIG_FLATMEM
#ifdef CONFIG_L4
#define kern_addr_valid(addr)	((addr) >= PAGE_SIZE)
#else /* L4 */
#define kern_addr_valid(addr)	(1)
#endif /* L4 */
#else
#define kern_addr_valid(kaddr)	(0)
#endif

/*
 * This is used to calculate the .brk reservation for initial pagetables.
 * Enough space is reserved to allocate pagetables sufficient to cover all
 * of LOWMEM_PAGES, which is an upper bound on the size of the direct map of
 * lowmem.
 *
 * With PAE paging (PTRS_PER_PMD > 1), we allocate PTRS_PER_PGD == 4 pages for
 * the PMD's in addition to the pages required for the last level pagetables.
 */
#if PTRS_PER_PMD > 1
#define PAGE_TABLE_SIZE(pages) (((pages) / PTRS_PER_PMD) + PTRS_PER_PGD)
#else
#define PAGE_TABLE_SIZE(pages) ((pages) / PTRS_PER_PGD)
#endif

/*
 * Number of possible pages in the lowmem region.
 *
 * We shift 2 by 31 instead of 1 by 32 to the left in order to avoid a
 * gas warning about overflowing shift count when gas has been compiled
 * with only a host target support using a 32-bit type for internal
 * representation.
 */
#define LOWMEM_PAGES ((((_ULL(2)<<31) - __PAGE_OFFSET) >> PAGE_SHIFT))

#endif /* _ASM_X86_PGTABLE_32_H */
