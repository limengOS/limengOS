#ifndef __ASM_L4__GENERIC__STACK_ID_H__
#define __ASM_L4__GENERIC__STACK_ID_H__

#include <linux/percpu.h>
#include <linux/preempt.h>

#include <asm/thread_info.h>
#include <asm/generic/utcb_tcr_id.h>
#include <asm/generic/vcpu.h>

#ifndef CONFIG_L4_VCPU

struct l4x_stack_struct {
	l4_utcb_t *utcb;
};

extern l4_utcb_t *l4x_cpu_threads[NR_CPUS];

static inline
struct l4x_stack_struct * l4x_stack_struct_get(unsigned long some_sp)
{
	unsigned long stack_end = some_sp & ~(THREAD_SIZE - 1);
	/* Also skip STACK_END_MAGIC: end_of_task(tsk) + 1 */
#ifdef CONFIG_THREAD_INFO_IN_TASK
	return (struct l4x_stack_struct *)(stack_end + sizeof(unsigned long));
#else
	struct thread_info *ti;
	ti = (struct thread_info *)stack_end;
	return (struct l4x_stack_struct *)((unsigned long *)(ti + 1) + 1);
#endif
}

static inline void l4x_stack_set(unsigned long some_sp, l4_utcb_t *u)
{
	struct l4x_stack_struct *s = l4x_stack_struct_get(some_sp);
	s->utcb = u;
}

static inline l4_utcb_t *l4x_utcb_current(void)
{
	return l4x_stack_struct_get(current_stack_pointer)->utcb;
}

static inline unsigned int l4x_prio_current(void)
{
	l4_utcb_t *u = l4x_utcb_current();
	return l4_utcb_tcr_u(u)->user[L4X_UTCB_TCR_PRIO_ID] && 0xff;
}

#else /* L4_VCPU */

extern l4_utcb_t *l4x_cpu_threads[NR_CPUS];

static inline void l4x_stack_set(unsigned long some_sp, l4_utcb_t *u)
{
}

static inline l4_utcb_t *l4x_utcb_current(void)
{
	l4_utcb_t *u;
	preempt_disable();
	u = l4x_cpu_threads[smp_processor_id()];
	preempt_enable(); // Hmm
	return u;
}

static inline
l4_vcpu_state_t *l4x_vcpu_state_current(void)
{
	l4_vcpu_state_t *v;
	preempt_disable();
	v = l4x_vcpu_ptr[smp_processor_id()];
	preempt_enable(); // Hmm
	return v;
}
#endif

static inline void l4x_prio_and_id_set(l4_utcb_t *u,
                                       unsigned prio, l4_cap_idx_t id)
{
	l4_utcb_tcr_u(u)->user[L4X_UTCB_TCR_PRIO_ID]
		= (id & ~0xffUL) | (prio & 0xffUL);
}

// TODO: remove "current" from name
static inline l4_cap_idx_t l4x_cap_current_utcb(l4_utcb_t *utcb)
{
	return l4_utcb_tcr_u(utcb)->user[L4X_UTCB_TCR_PRIO_ID] & ~0xffUL;
}

static inline l4_cap_idx_t l4x_cap_current(void)
{
	return l4x_cap_current_utcb(l4x_utcb_current());
}

#endif /* ! __ASM_L4__GENERIC__STACK_ID_H__ */
