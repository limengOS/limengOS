INTERFACE[arm && 64bit && !cpu_virt]:

EXTENSION class Proc
{
public:
  enum : unsigned
  {
    Status_mode_user      = 0x00,
    Status_mode_always_on = 0x00,
  };
};

//--------------------------------------------------------------------
INTERFACE[arm && 64bit && cpu_virt]:

EXTENSION class Proc
{
public:
  enum : unsigned
  {
    // user threads on a hyp kernel run in system mode
    Status_mode_user      = 0x04, // EL1t
    Status_mode_always_on = 0x300,
  };
};

//--------------------------------------------------------------------
IMPLEMENTATION [arm && 64bit]:

IMPLEMENT static inline
Mword Proc::program_counter()
{
  Mword pc;
  asm ("ldr %0, =1f; 1:" : "=r" (pc));
  return pc;
}

IMPLEMENT static inline
Proc::Status Proc::interrupts()
{
  Status ret;
  asm volatile("mrs %0, DAIF" : "=r" (ret));
  return !(ret & Sti_mask);
}

IMPLEMENT static inline
void Proc::cli()
{
  asm volatile("msr DAIFSet, %0" : : "i"(Cli_mask >> 6) : "memory");
}

IMPLEMENT static inline ALWAYS_INLINE
void Proc::sti()
{
  asm volatile("msr DAIFClr, %0" : : "i"(Cli_mask >> 6) : "memory");
}

IMPLEMENT static inline ALWAYS_INLINE
Proc::Status Proc::cli_save()
{
  Status prev;
  asm volatile("mrs %0, DAIF \n"
               "msr DAIFSet, %1"
               : "=r" (prev)
               : "i"(Cli_mask >> 6)
               : "memory");
  return prev;
}

//----------------------------------------------------------------
IMPLEMENTATION[arm && 64bit]:

IMPLEMENT static inline
Cpu_phys_id Proc::cpu_id()
{
  Mword mpidr;
  __asm__("mrs %0, MPIDR_EL1" : "=r" (mpidr));
  return Cpu_phys_id(mpidr & 0xffffff);
}

//--------------------------------------------------------------------
IMPLEMENTATION [arm && 64bit && cpu_virt]:

PUBLIC static inline
Unsigned32
Proc::sctlr_el1()
{
  Mword v;
  asm volatile ("mrs %0, SCTLR_EL1" : "=r"(v));
  return v;
}

PUBLIC static inline
Unsigned32
Proc::sctlr()
{
  Mword v;
  asm volatile ("mrs %0, SCTLR_EL2" : "=r"(v));
  return v;
}

//--------------------------------------------------------------------
IMPLEMENTATION [arm && 64bit && !cpu_virt]:

PUBLIC static inline
Unsigned32
Proc::sctlr()
{
  Mword v;
  asm volatile ("mrs %0, SCTLR_EL1" : "=r"(v));
  return v;
}

