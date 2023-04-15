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

#include "local.h"

#include <linux/timer.h>
#include <linux/fs.h>
#include <asm/delay.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>

extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp);

DECLARE_INITVAR(dde26_timer);

/* Definitions from linux/kernel/timer.c */

/*
 * per-CPU timer vector definitions:
 */
#define TVN_BITS (CONFIG_BASE_SMALL ? 4 : 6)
#define TVR_BITS (CONFIG_BASE_SMALL ? 6 : 8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

typedef struct tvec_s {
	struct list_head vec[TVN_SIZE];
} tvec_t;

typedef struct tvec_root_s {
	struct list_head vec[TVR_SIZE];
} tvec_root_t;

struct tvec_base {
	spinlock_t lock;
	struct timer_list *running_timer;
	unsigned long timer_jiffies;
	tvec_root_t tv1;
	tvec_t tv2;
	tvec_t tv3;
	tvec_t tv4;
	tvec_t tv5;
} ____cacheline_aligned_in_smp;

typedef struct tvec_t_base_s tvec_base_t;

struct tvec_base boot_tvec_bases __attribute__((unused));

static DEFINE_PER_CPU(struct tvec_base *, tvec_bases) __attribute__((unused)) = &boot_tvec_bases;

void init_timer(struct timer_list *timer)
{
	timer->ddekit_timer_id = DDEKIT_INVALID_TIMER_ID;
}

void add_timer(struct timer_list *timer)
{
	CHECK_INITVAR(dde26_timer);
	/* DDE2.6 uses jiffies and HZ as exported from L4IO. Therefore
	 * we just need to hand over the timeout to DDEKit. */
	timer->ddekit_timer_id = ddekit_add_timer((void *)timer->function,
	                                          (void *)timer->data,
	                                          timer->expires);
}


void add_timer_on(struct timer_list *timer, int cpu)
{
	add_timer(timer);
}


int del_timer(struct timer_list * timer)
{
	int ret;
	CHECK_INITVAR(dde26_timer);
	ret = ddekit_del_timer(timer->ddekit_timer_id);
	timer->ddekit_timer_id = DDEKIT_INVALID_TIMER_ID;

	return ret >= 0;
}

int del_timer_sync(struct timer_list *timer)
{
	return del_timer(timer);
}


int __mod_timer(struct timer_list *timer, unsigned long expires)
{
	/* XXX: Naive implementation. If we really need to be fast with
	 *      this function, we can implement a faster version inside
	 *      the DDEKit. Bjoern just does not think that this is the
	 *      case.
	 */
	int r;
	
	CHECK_INITVAR(dde26_timer);
	r = del_timer(timer);

	timer->expires = expires;
	add_timer(timer);

	return (r > 0);
}


int mod_timer(struct timer_list *timer, unsigned long expires)
{
	return __mod_timer(timer, expires);
}


int timer_pending(const struct timer_list *timer)
{
	CHECK_INITVAR(dde26_timer);
	/* There must be a valid DDEKit timer ID in the timer field
	 * *AND* it must be pending in the DDEKit.
	 */
	return ((timer->ddekit_timer_id != DDEKIT_INVALID_TIMER_ID) 
	        && ddekit_timer_pending(timer->ddekit_timer_id));
}


/**
 * msleep - sleep safely even with waitqueue interruptions
 * @msecs: Time in milliseconds to sleep for
 */
void msleep(unsigned int msecs)
{
	ddekit_thread_msleep(msecs);
}


#if defined(ARCH_x68) || defined(ARCH_amd64)
void __const_udelay(unsigned long xloops)
{
	ddekit_thread_usleep(xloops);
}


void __udelay(unsigned long usecs)
{
	ddekit_thread_usleep(usecs);
}


void __ndelay(unsigned long nsecs)
{
	ddekit_thread_nsleep(nsecs);
}
#endif


void __init l4dde26_init_timers(void)
{
	ddekit_init_timers();

	l4dde26_process_from_ddekit(ddekit_get_timer_thread());

	INITIALIZE_INITVAR(dde26_timer);

    calibrate_delay();
}

core_initcall(l4dde26_init_timers);

extern unsigned long volatile __jiffy_data jiffies;

__attribute__((weak)) void do_gettimeofday (struct timeval *tv)
{
#ifndef DDE_LINUX32
	WARN_UNIMPL;
#else
  struct timespec ts;
  
  clock_gettime(0, &ts);

  tv->tv_sec  = ts.tv_sec;
  tv->tv_usec = ts.tv_nsec / 1000;
#endif

}

ktime_t ktime_get_real(void)
{
#ifndef DDE_LINUX32
	struct timespec now = {0,0};
	WARN_UNIMPL;
#else
	struct timespec now;
        clock_gettime(0, &now);
#endif
	return timespec_to_ktime(now);
}

void ktime_get_ts(struct timespec *ts)
{
    clock_gettime(0, ts);
}

void native_io_delay(void)
{
	udelay(2);
}

/* temp add hrtimer MMMadd */

ktime_t ktime_add_safe(const ktime_t lhs, const ktime_t rhs)
{
    ktime_t res = ktime_add(lhs, rhs);

    /*
     * We use KTIME_SEC_MAX here, the maximum timeout which we can
     * return to user space in a timespec:
     */
    if (res.tv64 < 0 || res.tv64 < lhs.tv64 || res.tv64 < rhs.tv64)
        res = ktime_set(KTIME_SEC_MAX, 0);

    return res;
}

void hrtimer_init(struct hrtimer *timer, clockid_t clock_id,
          enum hrtimer_mode mode)
{
    init_timer(&timer->timer);
    timer->timer.data = timer;          //MMMadd need data
}

int hrtimer_start_range_ns(struct hrtimer *timer, ktime_t tim,
        unsigned long delta_ns, const enum hrtimer_mode mode)
{
    ktime_t expire = ktime_add_safe(tim, ns_to_ktime(delta_ns));
    struct timer_list *t = &timer->timer;

    t->function = (void (*)(unsigned long))timer->function;
    t->expires = expire.tv64 / 1000000ULL; 
    if (t->expires != 0) {
        add_timer(t);
    }

	return 0;
}

int hrtimer_cancel(struct hrtimer *timer)
{
    return del_timer(&timer->timer);
}

void init_timer_deferrable_key(struct timer_list *timer,
                   const char *name,
                   struct lock_class_key *key)
{
    init_timer(timer);
    /* maybe need deferrable later */
}

#if BITS_PER_LONG < 64
# ifndef CONFIG_KTIME_SCALAR
/**
 * ktime_add_ns - Add a scalar nanoseconds value to a ktime_t variable
 * @kt:     addend
 * @nsec:   the scalar nsec value to add
 *
 * Returns the sum of kt and nsec in ktime_t format
 */
ktime_t ktime_add_ns(const ktime_t kt, u64 nsec)
{
    ktime_t tmp;

    if (likely(nsec < NSEC_PER_SEC)) {
        tmp.tv64 = nsec;
    } else {
        unsigned long rem = do_div(nsec, NSEC_PER_SEC);

        tmp = ktime_set((long)nsec, rem);
    }

    return ktime_add(kt, tmp);
}
#endif
#endif

