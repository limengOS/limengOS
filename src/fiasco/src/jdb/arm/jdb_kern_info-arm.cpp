IMPLEMENTATION [arm]:

#include <cstdio>
#include <cstring>

#include "config.h"
#include "globals.h"
#include "space.h"

class Jdb_kern_info_misc : public Jdb_kern_info_module
{
};

static Jdb_kern_info_misc k_i INIT_PRIORITY(JDB_MODULE_INIT_PRIO+1);

PUBLIC
Jdb_kern_info_misc::Jdb_kern_info_misc()
  : Jdb_kern_info_module('i', "Miscellaneous info")
{
  Jdb_kern_info::register_subcmd(this);
}

PUBLIC
void
Jdb_kern_info_misc::show() override
{
  // FIXME: assume UP here (current_mem_space(0))
  Cpu_time clock = Kip::k()->clock();
  printf("clck: %08x.%08x\n"
	 "pdir: %08lx\n",
	 (unsigned)(clock >> 32), (unsigned)clock,
         (unsigned long)Mem_space::current_mem_space(Cpu_number::boot_cpu())->dir());


}


