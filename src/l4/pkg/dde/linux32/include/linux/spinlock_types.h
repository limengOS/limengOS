#ifndef __LINUX_SPINLOCK_TYPES_H
#define __LINUX_SPINLOCK_TYPES_H

/*
 * include/linux/spinlock_types.h - generic spinlock type definitions
 *                                  and initializers
 *
 * portions Copyright 2005, Red Hat, Inc., Ingo Molnar
 * Released under the General Public License (GPL).
 */

#ifndef DDE_LINUX

#if defined(CONFIG_SMP)
# include <asm/spinlock_types.h>
#else
# include <linux/spinlock_types_up.h>
#endif

#include <linux/lockdep.h>

typedef struct {
	raw_spinlock_t raw_lock;
#ifdef CONFIG_GENERIC_LOCKBREAK
	unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
} spinlock_t;

#define SPINLOCK_MAGIC		0xdead4ead

typedef struct {
	raw_rwlock_t raw_lock;
#ifdef CONFIG_GENERIC_LOCKBREAK
	unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
} rwlock_t;

#define RWLOCK_MAGIC		0xdeaf1eed

#define SPINLOCK_OWNER_INIT	((void *)-1L)

#ifdef CONFIG_DEBUG_LOCK_ALLOC
# define SPIN_DEP_MAP_INIT(lockname)	.dep_map = { .name = #lockname }
#else
# define SPIN_DEP_MAP_INIT(lockname)
#endif

#ifdef CONFIG_DEBUG_LOCK_ALLOC
# define RW_DEP_MAP_INIT(lockname)	.dep_map = { .name = #lockname }
#else
# define RW_DEP_MAP_INIT(lockname)
#endif

#ifdef CONFIG_DEBUG_SPINLOCK
# define __SPIN_LOCK_UNLOCKED(lockname)					\
	(spinlock_t)	{	.raw_lock = __RAW_SPIN_LOCK_UNLOCKED,	\
				.magic = SPINLOCK_MAGIC,		\
				.owner = SPINLOCK_OWNER_INIT,		\
				.owner_cpu = -1,			\
				SPIN_DEP_MAP_INIT(lockname) }
#define __RW_LOCK_UNLOCKED(lockname)					\
	(rwlock_t)	{	.raw_lock = __RAW_RW_LOCK_UNLOCKED,	\
				.magic = RWLOCK_MAGIC,			\
				.owner = SPINLOCK_OWNER_INIT,		\
				.owner_cpu = -1,			\
				RW_DEP_MAP_INIT(lockname) }
#else
# define __SPIN_LOCK_UNLOCKED(lockname) \
	(spinlock_t)	{	.raw_lock = __RAW_SPIN_LOCK_UNLOCKED,	\
				SPIN_DEP_MAP_INIT(lockname) }
#define __RW_LOCK_UNLOCKED(lockname) \
	(rwlock_t)	{	.raw_lock = __RAW_RW_LOCK_UNLOCKED,	\
				RW_DEP_MAP_INIT(lockname) }
#endif

/*
 * SPIN_LOCK_UNLOCKED and RW_LOCK_UNLOCKED defeat lockdep state tracking and
 * are hence deprecated.
 * Please use DEFINE_SPINLOCK()/DEFINE_RWLOCK() or
 * __SPIN_LOCK_UNLOCKED()/__RW_LOCK_UNLOCKED() as appropriate.
 */
#define SPIN_LOCK_UNLOCKED	__SPIN_LOCK_UNLOCKED(old_style_spin_init)
#define RW_LOCK_UNLOCKED	__RW_LOCK_UNLOCKED(old_style_rw_init)

#define DEFINE_SPINLOCK(x)	spinlock_t x = __SPIN_LOCK_UNLOCKED(x)
#define DEFINE_RWLOCK(x)	rwlock_t x = __RW_LOCK_UNLOCKED(x)

#else

#include <l4/dde/ddekit/lock.h>

typedef struct {
	ddekit_lock_t ddekit_lock;
	char          need_init;
} spinlock_t;

typedef spinlock_t rwlock_t;

#define SPIN_LOCK_UNLOCKED { .ddekit_lock = NULL, .need_init = 1 }
#define RW_LOCK_UNLOCKED   { .ddekit_lock = NULL, .need_init = 1 }

#define __SPIN_LOCK_UNLOCKED(name)   SPIN_LOCK_UNLOCKED
#define __RW_LOCK_UNLOCKED(name)     RW_LOCK_UNLOCKED

#define DEFINE_SPINLOCK(x)	spinlock_t x = __SPIN_LOCK_UNLOCKED(x)
#define DEFINE_RWLOCK(x)	rwlock_t x = __RW_LOCK_UNLOCKED(x)

#define DEFINE_RAW_SPINLOCK(x)  DEFINE_SPINLOCK(x)

#include <linux/lockdep.h>

/* MMMadd start */
#ifdef CONFIG_DEBUG_SPINLOCK

typedef struct {
	volatile unsigned int lock;
} arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED { 1 }

#else

//typedef struct { } arch_spinlock_t;
typedef struct {
	volatile unsigned int lock;
} arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED { }

#endif

typedef struct {
	/* no debug version on UP */
	volatile unsigned int lock;
} arch_rwlock_t;

#if 0
typedef struct raw_spinlock {
	arch_spinlock_t raw_lock;
#ifdef CONFIG_GENERIC_LOCKBREAK
	unsigned int break_lock;
#endif
#ifdef CONFIG_DEBUG_SPINLOCK
	unsigned int magic, owner_cpu;
	void *owner;
#endif
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
} raw_spinlock_t; 
#endif

typedef  spinlock_t raw_spinlock_t;

#ifdef CONFIG_DEBUG_SPINLOCK
# define SPIN_DEBUG_INIT(lockname)      \
    .magic = SPINLOCK_MAGIC,        \
    .owner_cpu = -1,            \
    .owner = SPINLOCK_OWNER_INIT,
#else
# define SPIN_DEBUG_INIT(lockname)
#endif

#ifdef CONFIG_DEBUG_LOCK_ALLOC
# define SPIN_DEP_MAP_INIT(lockname)    .dep_map = { .name = #lockname }
#else
# define SPIN_DEP_MAP_INIT(lockname)
#endif

#if 0
#define __RAW_SPIN_LOCK_INITIALIZER(lockname)   \
    {                   \
    .raw_lock = __ARCH_SPIN_LOCK_UNLOCKED,  \
    SPIN_DEBUG_INIT(lockname)       \
    SPIN_DEP_MAP_INIT(lockname) }
#endif

#define __RAW_SPIN_LOCK_INITIALIZER(lockname) __SPIN_LOCK_UNLOCKED(lockname)

#define __RAW_SPIN_LOCK_UNLOCKED(lockname)  \
    (raw_spinlock_t) __RAW_SPIN_LOCK_INITIALIZER(lockname)


/* MMMadd end */
#endif /* DDE_LINUX */

#endif /* __LINUX_SPINLOCK_TYPES_H */
