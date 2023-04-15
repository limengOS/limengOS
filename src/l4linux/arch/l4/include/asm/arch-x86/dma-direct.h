/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ASM_L4_X86_DMA_DIRECT_H
#define ASM_L4_X86_DMA_DIRECT_H 1

#include <asm/io.h>

static inline dma_addr_t phys_to_dma(struct device *dev, phys_addr_t paddr)
{
	if (dev->dma_range_map)
		return translate_phys_to_dma(dev, paddr);
	return l4x_virt_to_phys((void *)__va(paddr));
}

static inline phys_addr_t dma_to_phys(struct device *dev, dma_addr_t dev_addr)
{
	phys_addr_t paddr;

	if (dev->dma_range_map)
		paddr = translate_dma_to_phys(dev, dev_addr);
	else if (dev_addr == ~0ULL)
		paddr = (phys_addr_t)~0ULL;
	else if (dev_addr == ~0U)
		paddr = ~0U;
	else
		paddr = (phys_addr_t)__pa(l4x_phys_to_virt(dev_addr));

	return paddr;
}

#endif /* ASM_L4_X86_DMA_DIRECT_H */
