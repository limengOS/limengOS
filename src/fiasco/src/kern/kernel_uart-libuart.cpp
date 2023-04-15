IMPLEMENTATION [libuart]:

#include "kmem.h"
#include "io_regblock.h"

//------------------------------------------------------------------------
IMPLEMENTATION [libuart && serial && io]:

#include "io_regblock_port.h"
#include "types.h"

namespace {

union Regs
{
  Static_object<L4::Io_register_block_port> io;
  Static_object<L4::Io_register_block_mmio> mem;
  Static_object<L4::Io_register_block_mmio_fixed_width<Unsigned64> > mem64;
  Static_object<L4::Io_register_block_mmio_fixed_width<Unsigned32> > mem32;
  Static_object<L4::Io_register_block_mmio_fixed_width<Unsigned16> > mem16;
};

}

PRIVATE bool
Kernel_uart::setup_uart_io_port(void *r, Address base, int irq)
{
  Regs *regs = static_cast<Regs *>(r);
  regs->io.construct(base);
  return this->Uart::startup(regs->io.get(), irq,
                             Koptions::o()->uart.base_baud);
}


//------------------------------------------------------------------------
IMPLEMENTATION [libuart && serial && !io]:

#include "types.h"

namespace {

union Regs
{
  Static_object<L4::Io_register_block_mmio> mem;
  Static_object<L4::Io_register_block_mmio_fixed_width<Unsigned32> > mem32;
  Static_object<L4::Io_register_block_mmio_fixed_width<Unsigned16> > mem16;
};

}

PRIVATE bool
Kernel_uart::setup_uart_io_port(void *, Address, int)
{
  panic ("cannot use IO-Port based uart\n");
}

//------------------------------------------------------------------------
IMPLEMENTATION [libuart && serial]:

IMPLEMENT_OVERRIDE
bool
Kernel_uart::init_for_mode(Init_mode init_mode)
{
  if (Koptions::o()->uart.access_type == Koptions::Uart_type_ioport)
    return init_mode == Init_before_mmu;
  else
    return init_mode == Init_after_mmu;
}

IMPLEMENT
bool Kernel_uart::startup(unsigned, int irq)
{
  static Regs regs;

  if (Koptions::o()->opt(Koptions::F_uart_base))
    {
      Address base = Koptions::o()->uart.base_address;
      switch (Koptions::o()->uart.access_type)
        {
        case Koptions::Uart_type_ioport:
          return setup_uart_io_port(&regs, base, irq);

        case Koptions::Uart_type_mmio:
            {
              L4::Io_register_block *r = 0;

              // Koptions doesn't pass the UART size so take a sound guess.
              Address const size = 0x1000 - (base & 0xfff);
              switch (Koptions::o()->uart.reg_shift)
                {
                case 0: // no shift use natural access width
                  r = regs.mem.construct(Kmem::mmio_remap(base, size),
                                         Koptions::o()->uart.reg_shift);
                  break;
                case 1: // 1 bit shift, assume fixed 16bit access width
                  r = regs.mem16.construct(Kmem::mmio_remap(base, size),
                                           Koptions::o()->uart.reg_shift);
                  break;
                case 2: // 2 bit shift, assume fixed 32bit access width
                  r = regs.mem32.construct(Kmem::mmio_remap(base, size),
                                           Koptions::o()->uart.reg_shift);
                  break;
                case 3: // 3 bit shift, assume fixed 64bit access width
                  r = regs.mem32.construct(Kmem::mmio_remap(base, size),
                                           Koptions::o()->uart.reg_shift);
                  break;
                default:
                  panic("UART: illegal reg shift value: %d",
                        Koptions::o()->uart.reg_shift);
                  break;
                }
              return this->Uart::startup(r, irq, Koptions::o()->uart.base_baud);
            }
        default:
          return false;
        }
    }

  return false;
}
