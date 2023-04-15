//----------------------------------------------------------------------------
IMPLEMENTATION [amd64 && !kernel_isolation]:

#define FIASCO_ASM_IRET "iretq \n\t"

PUBLIC template<typename T> inline
void FIASCO_NORETURN
Thread::fast_return_to_user(Mword ip, Mword sp, T arg)
{
  assert(cpu_lock.test());
  assert(current() == this);

  asm volatile
    ("mov %[sp], %%rsp \t\n"
     "mov %[flags], %%r11 \t\n"
     "sysretq \t\n"
     :
     : [flags] "i" (EFLAGS_IF), "c" (ip), [sp] "r" (sp), "D"(arg)
    );
  __builtin_trap();
}

//----------------------------------------------------------------------------
IMPLEMENTATION [amd64 && kernel_isolation]:

#define FIASCO_ASM_IRET "jmp safe_iret \n\t"

PUBLIC template<typename T> inline
void FIASCO_NORETURN
Thread::fast_return_to_user(Mword ip, Mword sp, T arg)
{
  assert(cpu_lock.test());
  assert(current() == this);

  Address *p = (Address *)Mem_layout::Kentry_cpu_page;

#ifdef CONFIG_INTEL_IA32_BRANCH_BARRIERS
  if (p[2] & 1)
    {
      p[2] &= ~1UL;
      Cpu::wrmsr(0, 0, 0x49);
    }
#endif

  asm volatile
    ("mov %[sp], %%rsp \t\n"
     "mov %[flags], %%r11 \t\n"
     "jmp safe_sysret \t\n"
     :
     // p[0] = CPU dir pa (if PCID: + bit63 + ASID 0)
     // p[1] = KSP
     // p[2] = EXIT flags
     // p[3] = CPU dir pa + 0x1000 (if PCID: + bit63 + ASID)
     // p[4] = kernel entry scratch register
     : [cr3] "a" (p[3]),
       [flags] "i" (EFLAGS_IF), "c" (ip), [sp] "r" (sp), "D"(arg)
    );
  __builtin_trap();
}

//----------------------------------------------------------------------------
IMPLEMENTATION [amd64]:

PROTECTED inline NEEDS[Thread::sys_gdt_x86]
L4_msg_tag
Thread::invoke_arch(L4_msg_tag tag, Utcb const *utcb, Utcb *out)
{
  switch (utcb->values[0] & Opcode_mask)
    {
    case Op_gdt_x86: return sys_gdt_x86(tag, utcb, out);
    case Op_set_segment_base_amd64:
      {
        if (tag.words() < 2)
          return commit_result(-L4_err::EMsgtooshort);

        Mword base = access_once(utcb->values + 1);
        if (!Cpu::is_canonical_address(base))
          return commit_result(-L4_err::EInval);

        switch (utcb->values[0] >> 16)
          {
          case 0:
            _fs = 0;
            _fs_base = base;
            if (current() == this)
              Cpu::set_fs_base(&_fs_base);
            break;

          case 1:
            _gs = 0;
            _gs_base = base;
            if (current() == this)
              Cpu::set_gs_base(&_gs_base);
            break;

          default: return commit_result(-L4_err::EInval);
          }
        return Kobject_iface::commit_result(0);
      }
    case Op_segment_info_amd64:
      out->values[0] = Gdt::gdt_data_user   | Gdt::Selector_user; // user_ds32
      out->values[1] = Gdt::gdt_code_user   | Gdt::Selector_user; // user_cs64
      out->values[2] = Gdt::gdt_code_user32 | Gdt::Selector_user; // user_cs32
      return Kobject_iface::commit_result(0, 3);
    default:
      return commit_result(-L4_err::ENosys);
    };
}

IMPLEMENT inline
Mword
Thread::user_sp() const
{ return exception_triggered()?_exc_cont.sp(regs()):regs()->sp(); }

IMPLEMENT inline
void
Thread::user_sp(Mword sp)
{
  if (exception_triggered())
    _exc_cont.sp(regs(), sp);
  else
    regs()->sp(sp);
}

PROTECTED inline
int
Thread::do_trigger_exception(Entry_frame *r, void *ret_handler)
{
  if (!exception_triggered())
    {
      _exc_cont.activate(r, ret_handler);
      return 1;
    }
  // else ignore change of IP because triggered exception already pending
  return 0;
}

PUBLIC inline
void
Thread::restore_exc_state()
{
  _exc_cont.restore(regs());
}

PRIVATE static inline
Return_frame *
Thread::trap_state_to_rf(Trap_state *ts)
{
  char *im = reinterpret_cast<char*>(ts + 1);
  return reinterpret_cast<Return_frame*>(im)-1;
}

PRIVATE static inline NEEDS[Thread::trap_state_to_rf, Thread::sanitize_user_flags]
bool FIASCO_WARN_RESULT
Thread::copy_utcb_to_ts(L4_msg_tag const &tag, Thread *snd, Thread *rcv,
                        L4_fpage::Rights rights)
{
  if (EXPECT_FALSE((tag.words() * sizeof(Mword)) < sizeof(Trex)))
    return true;

  Trap_state *ts = (Trap_state*)rcv->_utcb_handler;
  Unsigned32  cs = ts->cs();
  Utcb *snd_utcb = snd->utcb().access();
  Trex const *src = reinterpret_cast<Trex const *>(snd_utcb->values);

  if (EXPECT_FALSE(rcv->exception_triggered()))
    {
      // triggered exception pending
      Mem::memcpy_mwords(ts, &src->s, Ts::Reg_words);
      Continuation::User_return_frame const *urfp
        = reinterpret_cast<Continuation::User_return_frame const *>
            ((char*)&src->s._ip);

      Continuation::User_return_frame urf = access_once(urfp);

      // sanitize flags
      urf.flags(sanitize_user_flags(urf.flags()));
      rcv->_exc_cont.set(trap_state_to_rf(ts), &urf);
    }
  else
    {
      Mem::memcpy_mwords(ts, &src->s, Ts::Words);
      // sanitize flags
      ts->flags(sanitize_user_flags(ts->flags()));
      // don't allow to overwrite the code selector!
      ts->cs(cs & ~0x80);
    }

  rcv->_fs_base = access_once(&src->fs_base);
  rcv->_gs_base = access_once(&src->gs_base);

  rcv->_ds = access_once(&src->ds);
  rcv->_es = access_once(&src->es);
  rcv->_fs = access_once(&src->fs);
  rcv->_gs = access_once(&src->gs);

  if (rcv == current())
    rcv->load_gdt_user_entries(rcv);

  if (tag.transfer_fpu() && (rights & L4_fpage::Rights::CS()))
    snd->transfer_fpu(rcv);

  bool ret = transfer_msg_items(tag, snd, snd_utcb,
                                rcv, rcv->utcb().access(), rights);

  return ret;
}

PRIVATE static inline NEEDS[Thread::trap_state_to_rf]
bool FIASCO_WARN_RESULT
Thread::copy_ts_to_utcb(L4_msg_tag const &, Thread *snd, Thread *rcv,
                        L4_fpage::Rights rights)
{
  Trap_state *ts = (Trap_state*)snd->_utcb_handler;
  Utcb *rcv_utcb = rcv->utcb().access();
  Trex *dst = reinterpret_cast<Trex *>(rcv_utcb->values);
    {
      auto guard = lock_guard(cpu_lock);

      dst->ds = snd->_ds;
      dst->es = snd->_es;
      dst->fs = snd->_fs;
      dst->gs = snd->_gs;
      dst->fs_base = snd->_fs_base;
      dst->gs_base = snd->_gs_base;

      if (EXPECT_FALSE(snd->exception_triggered()))
        {
          Mem::memcpy_mwords(&dst->s, ts, Ts::Reg_words + Ts::Code_words);
          Continuation::User_return_frame *d
            = reinterpret_cast<Continuation::User_return_frame *>
            ((char*)&dst->s._ip);

          snd->_exc_cont.get(d, trap_state_to_rf(ts));
        }
      else
        Mem::memcpy_mwords(&dst->s, ts, Ts::Words);

      if (rcv_utcb->inherit_fpu() && (rights & L4_fpage::Rights::CS()))
        snd->transfer_fpu(rcv);
    }
  return true;
}


IMPLEMENT
void
Thread::user_invoke()
{
  user_invoke_generic();

  Mword di = 0;

  if (current()->space()->is_sigma0())
    di = Kmem::virt_to_phys(Kip::k());

  asm volatile
    ("  mov %%rax,%%rsp \n"    // set stack pointer to regs structure
     "  mov %%ecx,%%es   \n"
     "  mov %%ecx,%%ds   \n"
     "  xor %%rax,%%rax \n"
     "  xor %%rcx,%%rcx \n"     // clean out user regs
     "  xor %%rdx,%%rdx \n"
     "  xor %%rsi,%%rsi \n"
     "  xor %%rbx,%%rbx \n"
     "  xor %%rbp,%%rbp \n"
     "  xor %%r8,%%r8   \n"
     "  xor %%r9,%%r9   \n"
     "  xor %%r10,%%r10 \n"
     "  xor %%r11,%%r11 \n"
     "  xor %%r12,%%r12 \n"
     "  xor %%r13,%%r13 \n"
     "  xor %%r14,%%r14 \n"
     "  xor %%r15,%%r15 \n"
     FIASCO_ASM_IRET
     :                          // no output
     : "a" (nonull_static_cast<Return_frame*>(current()->regs())),
       "c" (Gdt::gdt_data_user | Gdt::Selector_user),
       "D" (di)
     );

  // never returns here
}

PRIVATE inline
int
Thread::check_trap13_kernel (Trap_state * /*ts*/)
{ return 1; }

//----------------------------------------------------------------------------
IMPLEMENTATION [amd64 & (debug | kdb)]:

#include "kernel_task.h"

/** Call the nested trap handler (either Jdb::enter_kdebugger() or the
 * gdb stub. Setup our own stack frame */
PRIVATE static
int
Thread::call_nested_trap_handler(Trap_state *ts)
{
  Proc::cli();

  Cpu_number log_cpu = dbg_find_cpu();
  unsigned long &ntr = nested_trap_recover.cpu(log_cpu);

#if 0
  printf("%s: lcpu%u sp=%p t=%u nested_trap_recover=%ld\n",
      __func__, log_cpu, (void*)Proc::stack_pointer(), ts->_trapno,
      ntr);
#endif

  Unsigned64 ret;
  void *stack = 0;
  if (!ntr)
    stack = dbg_stack.cpu(log_cpu).stack_top;

  Unsigned64 dummy1, dummy2, scratch1, scratch2;

  // don't set %esp if gdb fault recovery to ensure that exceptions inside
  // kdb/jdb don't overwrite the stack
  asm volatile
    ("mov    %%rsp,%[d2]	\n\t"	// save old stack pointer
     "cmpq   $0,%[recover]	\n\t"
     "jne    1f			\n\t"	// check trap within trap handler
     "mov    %[stack],%%rsp	\n\t"	// setup clean stack pointer
     "1:			\n\t"
     "incq   %[recover]		\n\t"
#ifndef CONFIG_CPU_LOCAL_MAP
     "mov    %%cr3, %[d1]	\n\t"
#endif
     "push   %[d2]		\n\t"	// save old stack pointer on new stack
     "push   %[d1]		\n\t"	// save old pdbr
#ifndef CONFIG_CPU_LOCAL_MAP
     "mov    %[pdbr], %%cr3	\n\t"
#endif
     "callq  *%[handler]	\n\t"
     "pop    %[d1]		\n\t"
#ifndef CONFIG_CPU_LOCAL_MAP
     "mov    %[d1], %%cr3	\n\t"
#endif
     "pop    %%rsp		\n\t"	// restore old stack pointer
     "cmpq   $0,%[recover]	\n\t"	// check trap within trap handler
     "je     1f			\n\t"
     "decq   %[recover]		\n\t"
     "1:			\n\t"
     : [ret] "=&a"(ret), [d2] "=&d"(dummy2), [d1] "=&c"(dummy1), "=D"(scratch1),
       "=S"(scratch2),
       [recover] "+m" (ntr)
     : [ts] "D" (ts),
#ifndef CONFIG_CPU_LOCAL_MAP
       [pdbr] "r" (Kernel_task::kernel_task()->virt_to_phys((Address)Kmem::dir())),
#endif
       [cpu] "S" (log_cpu),
       [stack] "r" (stack),
       [handler] "m" (nested_trap_handler)
     : "r8", "r9", "r10", "r11", "memory");

  if (!ntr)
    Cpu_call::handle_global_requests();

  return ret == 0 ? 0 : -1;
}

