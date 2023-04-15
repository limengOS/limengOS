/*
 * Implementation of include/asm-l4/l4lxapi/memory.h
 */

#include <linux/bug.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>

#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/l4lxapi/memory.h>
#include <asm/api/api.h>
#include <asm/generic/memory.h>
#include <asm/generic/vcpu.h>
#include <asm/generic/log.h>
#include <asm/generic/setup.h>

#include <l4/re/c/rm.h>
#include <l4/sys/err.h>

#include <l4/sys/kdebug.h>

#include <l4/log/log.h>

struct reg_search_data {
	l4_cap_idx_t ds;
	l4_addr_t virt;
};

static void reg_cb(l4_addr_t phys, void *virt,
                   l4_size_t size, l4_cap_idx_t ds,
                   l4_addr_t ds_offset, void *data)
{
	struct reg_search_data *d = (struct reg_search_data *)data;

	if (d->ds == ds)
		d->virt = (l4_addr_t)virt;
}

int l4lx_memory_map_virtual_page(unsigned long address, pte_t pte)
{
	l4re_ds_t ds;
	l4re_rm_offset_t offset;
	l4_addr_t addr;
	unsigned flags;
	unsigned long size;
	int r;

	if (0)
		l4x_early_pr_err("%s: %lx %016llx.\n", __func__, address,
		                 (u64)(pte_val(pte) & L4X_PHYSICAL_PAGE_MASK));
	addr = pte_val(pte) & L4X_PHYSICAL_PAGE_MASK;
	size = 1;
	if (L4XV_FN_i(l4re_rm_find(&addr, &size, &offset, &flags, &ds))) {
		l4x_early_pr_err("%s: Cannot get dataspace of "
		                 "%016llx.\n", __func__,
		                 (u64)(pte_val(pte) & L4X_PHYSICAL_PAGE_MASK));
		WARN_ON(1);
		return -1;
	}

	BUG_ON(flags & L4RE_RM_F_IN_AREA);

	offset += (pte_val(pte) & L4X_PHYSICAL_PAGE_MASK) - addr;
	addr    = address & PAGE_MASK;
	flags   = L4RE_RM_F_IN_AREA | L4RE_RM_F_EAGER_MAP | L4RE_RM_F_R;

	if (pte_write(pte)) {
		ds |= L4_CAP_FPAGE_RW;
		flags |= L4RE_RM_F_W;
	}

#ifndef CONFIG_ARM64
	if (pte_exec(pte))
#endif
		flags |= L4RE_RM_F_X;

	if ((r = L4XV_FN_i(l4re_rm_attach((void **)&addr, PAGE_SIZE,
	                                  flags, ds,
	                                  offset, L4_PAGESHIFT)))) {

		if (r == -L4_EADDRNOTAVAIL) {
			l4_addr_t a = address & PAGE_MASK;
			l4_addr_t q;
			struct reg_search_data data = { .virt = ~0ul };

			size = 1;
			if (L4XV_FN_i(l4re_rm_find(&a, &size, &offset,
			                           &flags, &data.ds))) {
				l4x_early_pr_err("%s: Failed to query address %lx\n",
				                 __func__, a);
				return -1;
			}

			l4x_v2p_for_each(reg_cb, &data);

			q = data.virt + offset;

			/* Return all ok if it's the same address
			 * The cap check is not perfect but will do. */
			if (q == (pte_val(pte) & L4X_PHYSICAL_PAGE_MASK)
			    && (ds & L4_CAP_MASK) == (data.ds & L4_CAP_MASK))
				return 0;

			l4x_early_pr_err("%s: Already used, existing is %lx\n",
			                 __func__, q);
			WARN_ON(1);
		}

		l4x_early_pr_err("%s: cannot attach vpage "
				 "(%lx, %llx): %d\n",
		                 __func__, address, (u64)pte_val(pte), r);
		return -1;
	}
	return 0;
}

int l4lx_memory_map_virtual_range(unsigned long address, unsigned long size,
                                  unsigned long page, int map_rw)
{
	unsigned long end;
	l4_addr_t addr;
	l4re_rm_offset_t offset;
	l4re_ds_t ds;
	unsigned flags;
	unsigned long s;
	int r;

	address = address & PAGE_MASK;
	end     = address + size;
	while (address < end) {
		addr = page;
		s = 1;

		if (L4XV_FN_i(l4re_rm_find(&addr, &s, &offset, &flags, &ds))) {
			printk("%s: Cannot get dataspace of %08lx.\n",
			       __func__, page);
			return -1;
		}

		offset += (page & PAGE_MASK) - addr;
		addr = address;
		if (s > end - address)
			s = end - address;
		r = L4XV_FN_i(l4re_rm_attach((void **)&addr, s,
		                             L4RE_RM_F_IN_AREA | L4RE_RM_F_EAGER_MAP
		                             | (map_rw ? L4RE_RM_F_RWX : L4RE_RM_F_RX),
		                             ds | (map_rw ? L4_CAP_FPAGE_RW : 0),
		                             offset, L4_PAGESHIFT));
		if (r) {
			// FIXME wrt L4_EUSED?
			// see above...
			printk("%s: cannot attach vpage (v=%lx, from=%lx, sz=%lx): %d\n",
			       __func__, address, page, s, r);
			return -1;
		}

		address += s;
		page    += s;
	}

	return 0;
}

int l4lx_memory_unmap_virtual_page(unsigned long address)
{
	if (L4XV_FN_i(l4re_rm_detach((void *)address)))
		// Do not complain: someone might vfree a reserved area
		// that has not been completely filled
		return -1;
	return 0;
}
