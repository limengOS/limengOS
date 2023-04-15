INTERFACE [arm && pic_gic && pf_lx2160]:

#include "gic.h"
#include "initcalls.h"

// ------------------------------------------------------------------------
IMPLEMENTATION [arm && pic_gic && pf_lx2160]:

#include "gic_v3.h"
#include "irq_mgr_multi_chip.h"
#include "kmem.h"

PUBLIC static FIASCO_INIT
void
Pic::init()
{
  typedef Irq_mgr_multi_chip<9> M;

  M *m = new Boot_object<M>(1);

  gic = new Boot_object<Gic_v3>(Kmem::mmio_remap(Mem_layout::Gic_dist_phys_base,
                                                 Gic_dist::Size),
                                Kmem::mmio_remap(Mem_layout::Gic_redist_phys_base,
                                                 Mem_layout::Gic_redist_size));

  m->add_chip(0, gic, gic->nr_irqs());

  Irq_mgr::mgr = m;
}

// ------------------------------------------------------------------------
IMPLEMENTATION [arm && pic_gic && mp && pf_lx2160]:

PUBLIC static
void Pic::init_ap(Cpu_number cpu, bool resume)
{
  gic->init_ap(cpu, resume);
}
