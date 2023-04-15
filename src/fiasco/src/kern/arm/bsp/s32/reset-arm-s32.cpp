IMPLEMENTATION [arm && pf_s32 && !arm_psci]:

#include "mmio_register_block.h"
#include "kmem.h"

void __attribute__ ((noreturn))
platform_reset(void)
{
  Mmio_register_block rgm(Kmem::mmio_remap(0x40078000, 0x20));
  rgm.r<8>(0x18) = 0xf;

  Mmio_register_block me(Kmem::mmio_remap(0x40088000, 16));
  me.r<32>(4) = 2;
  me.r<32>(8) = 1;
  me.r<32>(0) = 0x5AF0;
  me.r<32>(0) = 0xA50F;

  for (;;)
    ;
}

// ------------------------------------------------------------------------
IMPLEMENTATION [arm && pf_s32 && arm_psci]:

#include "platform_control.h"

void __attribute__ ((noreturn))
platform_reset(void)
{
  Platform_control::system_reset();
  for (;;)
    ;
}
