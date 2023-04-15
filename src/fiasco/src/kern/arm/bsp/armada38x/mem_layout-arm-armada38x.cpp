INTERFACE [arm && pf_armada38x]: //-------------------------------------------

EXTENSION class Mem_layout
{
public:
  enum Phys_layout_armada38x : Address
  {
    L2cxx0_phys_base     = 0xf1008000,
    Gic_cpu_phys_base    = 0xf100c100,
    Gic_dist_phys_base   = 0xf100d000,
    Mp_scu_phys_base     = 0xf100c000,
  };
};
