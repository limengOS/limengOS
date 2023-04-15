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
 * \brief   Page allocation
 *
 * In Linux 2.6 this resides in mm/page_alloc.c.
 *
 * This implementation is far from complete as it does not cover "struct page"
 * emulation. In Linux, there's an array of structures for all pages. In
 * particular, iteration works for this array like:
 *
 *   struct page *p = alloc_pages(3); // p refers to first page of allocation
 *   ++p;                             // p refers to second page
 *
 * There may be more things to cover and we should have a deep look into the
 * kernel parts we want to reuse. Candidates for problems may be file systems,
 * storage (USB, IDE), and video (bttv).
 */

/* Linux */
#include <linux/bootmem.h>
#include <linux/gfp.h>
#include <linux/string.h>
#include <linux/pagevec.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <linux/slab.h>

/* DDEKit */
#include <l4/dde/ddekit/memory.h>
#include <l4/dde/ddekit/assert.h>
#include <l4/dde/ddekit/panic.h>

#include "local.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

/*******************
 ** Configuration **
 *******************/

#define DEBUG_PAGE_ALLOC 0
#define DEBUG_PAGE_ALLOC2 0


/*
 * DDE page cache
 *
 * We need to store all pages somewhere (which in the Linux kernel is
 * performed by the huge VM infrastructure. Purpose for us is:
 *   - make virt_to_phys() work
 *   - enable external clients to hand in memory (e.g., a dm_phys
 *     dataspace and make it accessible as Linux pages to the DDE)
 */

#define DDE_PAGE_CACHE_SHIFT 	10
#define DDE_PAGE_CACHE_SIZE     (1 << DDE_PAGE_CACHE_SHIFT)
#define DDE_PAGE_CACHE_MASK     (DDE_PAGE_CACHE_SIZE - 1)

typedef struct
{
	struct hlist_node list;
	struct page *page;
} page_cache_entry;

static struct hlist_head dde_page_cache[DDE_PAGE_CACHE_SIZE];

/** Hash function to map virtual addresses to page cache buckets. */
#define VIRT_TO_PAGEHASH(a)           ((((unsigned long)a) >> PAGE_SHIFT) & DDE_PAGE_CACHE_MASK)


void dde_page_cache_add(struct page *p)
{
	unsigned int hashval = VIRT_TO_PAGEHASH(p->virtual);

	page_cache_entry *e = ddekit_simple_malloc(sizeof(page_cache_entry));

#if DEBUG_PAGE_ALLOC
	DEBUG_MSG("virt %p, hash: %x", p->virtual, hashval);
#endif

	e->page = p;
	INIT_HLIST_NODE(&e->list);

	hlist_add_head(&e->list, &dde_page_cache[hashval]);
}


void dde_page_cache_remove(struct page *p)
{
	unsigned int hashval = VIRT_TO_PAGEHASH(p->virtual);
	struct hlist_node *hn = NULL;
	struct hlist_head *h  = &dde_page_cache[hashval];
	page_cache_entry *e   = NULL;
	struct hlist_node *v  = NULL;

	hlist_for_each_entry(e, hn, h, list) {
		if (((unsigned long)e->page->virtual) == ((unsigned long)p->virtual & PAGE_MASK)){
			v = hn;
			break;
                }
	}

	if (v) {
#if DEBUG_PAGE_ALLOC
		DEBUG_MSG("deleting node %p which contained page %p", v, p);
#endif
		hlist_del(v);
		ddekit_simple_free(e);
		ddekit_simple_free(p);
#if DEBUG_PAGE_ALLOC2
        ddekit_log(1," free mem page_cache_entry:%p, page:%p\n", e, p);
#endif
	}
}


struct page* dde_page_lookup(unsigned long va)
{
	unsigned int hashval = VIRT_TO_PAGEHASH(va);

	struct hlist_node *hn = NULL;
	struct hlist_head *h  = &dde_page_cache[hashval];
	page_cache_entry *e   = NULL;

	hlist_for_each_entry(e, hn, h, list) {
		if ((unsigned long)e->page->virtual == (va & PAGE_MASK))
			return e->page;
	}

	return NULL;
}

unsigned long get_free_pages_addr(gfp_t gfp_mask, unsigned int order)
{
	ddekit_log(DEBUG_PAGE_ALLOC, "gfp_mask=%x order=%d (%d bytes)",
	           gfp_mask, order, PAGE_SIZE << order);

	Assert(gfp_mask != GFP_DMA);
	void *p = ddekit_large_malloc(PAGE_SIZE << order);

	return (unsigned long)p;
}

struct page * ddekit_alloc_pages(gfp_t gfp_mask, unsigned int order)
{
	int nr_pages = 1 << order;
    struct page *pg = ddekit_simple_malloc(sizeof(*pg) * nr_pages);
	if (pg == NULL) {
	    ddekit_log(1, "ddekit_simple_malloc 0x%x failed.", sizeof(*pg) * nr_pages);
	    return pg;
	}
    memset((void*)pg, 0, sizeof(*pg) * nr_pages);
	
	INIT_LIST_HEAD(&pg->lru);
	pg->virtual = (void *)get_free_pages_addr(gfp_mask, order);
	if (pg->virtual == NULL) {
	    ddekit_log(1, "get_free_pages_addr order = %d, failed.", order);
	    return pg->virtual;
	}
	for (int i = 1; i < nr_pages; i++) {
		struct page *p = pg + i;
		p->virtual = (void *) ((unsigned long)pg->virtual + i * PAGE_SIZE);
		INIT_LIST_HEAD(&p->lru);
		reset_page_mapcount(p);
		__ClearPageBuddy(p);
		set_page_private(p, 0);
		dde_page_cache_add(p);
	}

	dde_page_cache_add(pg);
	reset_page_mapcount(pg);

#if DEBUG_PAGE_ALLOC2
    ddekit_log(1," alloc mem page:%p page->virtual:%p, order:%d\n", pg, pg->virtual, order);
#endif
	return pg;
}



int get_user_pages(struct task_struct *tsk, struct mm_struct *mm,
                   unsigned long start, int len, int write, int force,
                   struct page **pages, struct vm_area_struct **vmas)
{
	WARN_UNIMPL;
	return 0;
}

/**
 * ...
 *
 * XXX order may be larger than allocation at 'addr' - it may comprise several
 * allocation via get_free_pages_addr()!
 */
void __ddekit_free_pages(unsigned long addr, unsigned int order)
{
    if (addr != 0) {
	    ddekit_log(DEBUG_PAGE_ALLOC, "addr=%p order=%d", (void *)addr, order);

	    ddekit_large_free((void *)addr);
    }
}

/* 
 * XXX: If alloc_pages() gets fixed to allocate a page struct per page,
 *      this needs to be adapted, too.
 */
void ddekit_free_pages(struct page *page, unsigned int order)
{
	__ddekit_free_pages((unsigned long)page->virtual, order);
	dde_page_cache_remove(page);
#if DEBUG_PAGE_ALLOC2
        ddekit_log(1," page->virtual:%p, order:%d, page_private:%d\n", page->virtual, order, page_private(page));
#endif
}


unsigned long __pa(volatile const void *addr)
{
	return ddekit_pgtab_get_physaddr((void*)addr);
}

void *__va(unsigned long addr)
{
	return (void*)ddekit_pgtab_get_virtaddr((ddekit_addr_t) addr);
}


int set_page_dirty_lock(struct page *page)
{
	WARN_UNIMPL;
	return 0;
}

/*
 * basically copied from linux/mm/page_alloc.c
 */
void *__init alloc_large_system_hash(const char *tablename,
                                     unsigned long bucketsize,
                                     unsigned long numentries,
                                     int scale,
                                     int flags,
                                     unsigned int *_hash_shift,
                                     unsigned int *_hash_mask,
                                     unsigned long limit)
{
	void * table = NULL;
	unsigned long log2qty;
	unsigned long size;

	if (numentries == 0)
		numentries = 1024;

	log2qty = ilog2(numentries);
#if 0       //MMMadd fix org issue 
	size = bucketsize << log2qty;

	do {
		unsigned long order;
		for (order = 0; ((1UL << order) << PAGE_SHIFT) < size; order++)
			table = (void*) __get_free_pages(GFP_ATOMIC, order);
	} while (!table && size > PAGE_SIZE && --log2qty);
#else
	do {
        size = bucketsize << log2qty;
        unsigned long order = get_order(size);
        if (order < MAX_ORDER){
			table = (void*) get_free_pages_addr(GFP_ATOMIC, order);
        }
	} while (!table && size > PAGE_SIZE && --log2qty);
#endif

	if (!table)
		panic("Failed to allocate %s hash table\n", tablename);

	printk("%s hash table entries: %d (order: %d, %lu bytes)\n",
	       tablename,
	       (1U << log2qty),
	       ilog2(size) - PAGE_SHIFT,
	       size);

	if (_hash_shift)
		*_hash_shift = log2qty;
	if (_hash_mask)
		*_hash_mask = (1 << log2qty) - 1;

	return table;
}


void __init dde_page_cache_init(void)
{
	printk("Initializing DDE page cache\n");
	int i=0;

	for (i; i < DDE_PAGE_CACHE_SIZE; ++i)
		INIT_HLIST_HEAD(&dde_page_cache[i]);
}


