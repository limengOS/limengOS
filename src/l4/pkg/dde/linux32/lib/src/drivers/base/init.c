/*
 * Copyright (c) 2002-3 Patrick Mochel
 * Copyright (c) 2002-3 Open Source Development Labs
 *
 * This file is released under the GPLv2
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/memory.h>

#include "base.h"

void rcu_init(void);
void __init vfs_caches_init_early(void);
void __init vfs_caches_init(unsigned long);
void __init free_area_init(unsigned long * zones_size);
int __init early_irq_init(void);
void build_all_zonelists(void *data);
void __init setup_per_cpu_pageset(void);
void __init setup_per_cpu_areas(void);
void __init percpu_init_late(void);
void __init buffer_init(void);
void __init kmem_cache_init(void);
void __init kmem_cache_init_late(void);

void free_area_zonelists_mem_init(void)
{
    unsigned long zones_size[] = {(200*1024*1024)/4096, 0};

    setup_per_cpu_areas();
    free_area_init(zones_size);                //MMMadd for init zone's locks
    build_all_zonelists(NULL);
    kmem_cache_init();                          //slab
    percpu_init_late();
    setup_per_cpu_pageset();

    kmem_cache_init_late();
}

/**
 * driver_init - initialize driver model.
 *
 * Call the driver model init functions to initialize their
 * subsystems. Called early from init/main.c.
 */
void __init driver_init(void)
{
    buffer_init();

	/* These are the core pieces */
	devices_init();
	buses_init();
	classes_init();
#ifndef DDE_LINUX
	firmware_init();
	hypervisor_init();
#endif

	/* These are also core pieces, but must come after the
	 * core core pieces.
	 */
	platform_bus_init();
	system_bus_init();
	cpu_dev_init();
#ifndef DDE_LINUX
	memory_dev_init();
	attribute_container_init();
#endif
    rcu_init();
    vfs_caches_init_early();
    vfs_caches_init(32*1024*1024/4096);        //MMMadd temp value
    early_irq_init();

}
