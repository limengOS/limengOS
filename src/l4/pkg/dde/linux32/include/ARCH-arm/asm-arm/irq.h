#ifndef __ASM_ARM_IRQ_H
#define __ASM_ARM_IRQ_H

#include <mach/irqs.h>

#ifndef irq_canonicalize
#define irq_canonicalize(i)	(i)
#endif

/*
 * Use this value to indicate lack of interrupt
 * capability
 */
#ifndef NO_IRQ
#define NO_IRQ	((unsigned int)(-1))
#endif

#ifndef NR_IRQS
#define NR_IRQS 32
#endif

#ifndef __ASSEMBLY__
struct irqaction;
extern void migrate_irqs(void);

extern void asm_do_IRQ(unsigned int, struct pt_regs *);
void init_IRQ(void);

#endif

#endif

