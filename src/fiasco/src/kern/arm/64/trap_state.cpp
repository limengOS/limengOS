INTERFACE:

#include "entry_frame.h"

class Trap_state : public Entry_frame
{
public:
  typedef int (*Handler)(Trap_state*, Cpu_number cpu);

  Mword trapno() const { return esr.ec(); }
  Mword error() const { return esr.raw(); }

  void set_pagefault(Mword pfa, Mword esr)
  {
    this->pf_address = pfa;
    this->esr = Arm_esr(esr);
  }

  bool exclude_logging() { return false; }
};

struct Trex
{
  Trap_state s;
  Mword tpidruro;
  Mword tpidrurw;

  void set_ipc_upcall()
  { s.esr.ec() = 0x3f; }

  void dump() { s.dump(); }
};

IMPLEMENTATION:

#include "mem_layout.h"

#include <cstdio>

PUBLIC
void
Trap_state::dump()
{
  char const *excpts[] =
    {/*  0 */ "undef insn",  "WFx",        0,            "MCR (CP15)",
     /*  4 */ "MCRR (CP15)", "MCR (CP14)", "LDC (CP14)", "coproc trap",
     /*  8 */ "MRC (CP10)",  0,            "BXJ",        0,
     /*  C */ "MRRC (CP14)", 0,            0,            0,
     /* 10 */ 0,             "SVC",        "HVC",        "SMC",
     /* 14 */ 0, 0, 0, 0,
     /* 18 */ 0, 0, 0, 0,
     /* 1C */ 0, 0, 0, 0,
     /* 20 */ "prefetch abt (usr)", "prefetch abt (kernel)", 0, 0,
     /* 24 */ "data abt (user)",    "data abt (kernel)",     0, 0,
     /* 28 */ 0, 0, 0, 0,
     /* 2C */ 0, 0, 0, 0,
     /* 30 */ 0, 0, 0, 0,
     /* 34 */ 0, 0, 0, 0,
     /* 38 */ 0, 0, 0, 0,
     /* 3C */ 0, 0, "<TrExc>", "<IPC>"};

  printf("EXCEPTION: (%02x) %s pfa=%08lx, error=%08lx pstate=%08lx\n",
         (unsigned)esr.ec(), excpts[esr.ec()] ? excpts[esr.ec()] : "",
         pf_address, esr.raw(), pstate);

  printf("R[ 0]: %016lx %016lx %016lx %016lx\n"
         "R[ 4]: %016lx %016lx %016lx %016lx\n"
         "R[ 8]: %016lx %016lx %016lx %016lx\n"
         "R[12]: %016lx %016lx %016lx %016lx\n"
         "R[16]: %016lx %016lx %016lx %016lx\n"
         "R[20]: %016lx %016lx %016lx %016lx\n"
         "R[24]: %016lx %016lx %016lx %016lx\n"
         "R[28]: %016lx %016lx %016lx\n"
         "SP: %016lx  PC: %016lx\n",
         r[0], r[1], r[2], r[3],
         r[4], r[5], r[6], r[7],
         r[8], r[9], r[10], r[11],
         r[12], r[13], r[14], r[15],
         r[16], r[17], r[18], r[19],
         r[20], r[21], r[22], r[23],
         r[24], r[25], r[26], r[27],
         r[28], r[29], r[30],
         usp, pc);

  extern char virt_address[] asm ("virt_address");
  Mword lower_limit = (Mword)&virt_address;
  Mword upper_limit = (Mword)&Mem_layout::initcall_end;
  if (lower_limit <= pc && pc < upper_limit)
    {
      printf("Data around PC at 0x%lx:\n", pc);
      for (Mword d = pc - 24; d < pc + 28; d += 4)
        if (lower_limit <= d && d < upper_limit)
          printf("%s0x%012lx: %08x\n", d == pc ? "->" : "  ", d,
                                       *reinterpret_cast<unsigned *>(d));
    }
}
