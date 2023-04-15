
INTERFACE [pf_omap3]: // ------------------------------------------------

#include "mmio_register_block.h"

class Priv_omap_ops : private Mmio_register_block
{
public:
    static void init();
private:
  enum {
   MOD_LEN      = 0x2400,
   Op_w_b       = 0,
   Op_w_w       = 1,
   Op_w_l       = 2,
  };

    static Static_object<Priv_omap_ops> _ops;
};

IMPLEMENTATION [pf_omap3]: // ------------------------------------------------

#include <cassert>
#include <cstdio>

#include "config.h"
#include "kmem.h"
#include "mem_layout.h"

Static_object<Priv_omap_ops> Priv_omap_ops::_ops;

PUBLIC static
void
Priv_omap_ops::write_ops(int op, Mword offs, Mword val)
{
    if (offs >= MOD_LEN)
        return;

    switch (op)
        {
        case Op_w_b:
            _ops->write<unsigned char>(val, offs);
            break;
        case Op_w_w:
            _ops->write<unsigned short>(val, offs);
            break;
        case Op_w_l:
            _ops->write<Mword>(val, offs);
            break;

        default:
            break;
        };
}

PUBLIC explicit
Priv_omap_ops::Priv_omap_ops()
: Mmio_register_block(Kmem::mmio_remap(Mem_layout::Control_module_base, 0x2400))
{ }

IMPLEMENT
void
Priv_omap_ops::init()
{
  _ops.construct();
}


#include "context.h"
#include "entry_frame.h"

extern "C" void sys_arm_priv_op()
{
  Entry_frame *e = current()->regs();
  Priv_omap_ops::write_ops(e->r[0], e->r[1], e->r[2]);
}

