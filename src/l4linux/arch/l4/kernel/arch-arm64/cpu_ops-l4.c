
#include <asm/cpu_ops.h>

#ifdef CONFIG_L4
#include <asm/generic/smp.h>
#include <l4/util/util.h>
#endif /* L4 */

#include <asm/generic/smp_ipi.h>

// TODO: move this out here
#ifdef CONFIG_CPU_IDLE
static int cpu_l4lx_init_idle(unsigned int cpu)
{
	return -1;
}
int cpu_l4lx_suspend(unsigned long index)
{
	return -1;
}
#endif

static int __init cpu_l4lx_cpu_init(unsigned int cpu)
{
	return 0;
}

static int __init cpu_l4lx_cpu_prepare(unsigned int cpu)
{
	return 0;
}

static int cpu_l4lx_cpu_boot(unsigned int cpu)
{
	l4x_cpu_spawn(cpu, secondary_data.task);
	l4x_cpu_release(cpu);
        return 0;
}

static void cpu_l4lx_cpu_postboot(void)
{
	l4x_setup_ipi_ap_cpus();
}

#ifdef CONFIG_HOTPLUG_CPU
static int cpu_l4lx_cpu_disable(unsigned int cpu)
{
	return 0;
}

static void cpu_l4lx_cpu_die(unsigned int cpu)
{
#ifdef CONFIG_L4
#ifndef CONFIG_L4_VCPU
	l4x_tamed_shutdown(cpu);
#endif
	l4x_shutdown_cpu(cpu);
	printk("CPU%d still alive...\n", raw_smp_processor_id());
	l4_sleep_forever();
#endif
}

static int cpu_l4lx_cpu_kill(unsigned int cpu)
{
	/* l4x_shutdown_cpu() is sync */
	return 0;
}
#endif


const struct cpu_operations l4linux_cpu_ops = {
	.name           = "l4lx",
#ifdef CONFIG_CPU_IDLE
	.cpu_init_idle  = cpu_l4lx_init_idle,
	.cpu_suspend    = cpu_l4lx_suspend,
#endif
	.cpu_init       = cpu_l4lx_cpu_init,
	.cpu_prepare    = cpu_l4lx_cpu_prepare,
	.cpu_boot       = cpu_l4lx_cpu_boot,
	.cpu_postboot   = cpu_l4lx_cpu_postboot,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_disable    = cpu_l4lx_cpu_disable,
	.cpu_die        = cpu_l4lx_cpu_die,
	.cpu_kill       = cpu_l4lx_cpu_kill,
#endif
};
