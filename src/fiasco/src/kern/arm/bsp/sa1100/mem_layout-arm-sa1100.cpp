INTERFACE [arm && pf_sa1100]:

EXTENSION class Mem_layout
{
public:
  enum Phys_layout_sa1100 : Address {
    Timer_phys_base      = 0x90000000,
    Pic_phys_base        = 0x90050000,
    Flush_area_phys_base = 0xe0000000,
  };
};
