INTERFACE [vmx]:

#include "vm_vmx.h"
#include "ptab_base.h"
#include "tlbs.h"

class Vm_vmx_ept_tlb : public Tlb
{
public:
  enum Context
  {
    Single = 0x1,
    Global = 0x2,
  };

  static void invept(Context type, Mword eptp = 0)
  {
    struct Op
    {
      Unsigned64 eptp, resv;
    } op = {eptp, 0};
    asm volatile ("invept %0, %[type]" : : "m"(op), [type] "r"((Mword)type));
  }

  /**
   * Flush only the TLB entries corresponding to the given EPT.
   */
  static void flush_single(Mword ept_phys)
  {
    if (Vmx::cpus.current().info.has_invept_single())
      invept(Single, ept_phys);
    else
      invept(Global);
  }

  /**
   * Flush the complete EPT TLB.
   */
  static void flush()
  {
    invept(Global);
  }


  explicit Vm_vmx_ept_tlb(Cpu_number cpu)
  {
    auto const &vmx = Vmx::cpus.cpu(cpu);
    if (!vmx.info.procbased_ctls2.allowed(Vmx_info::PRB2_enable_ept))
      // EPT is not enabled, no need for EPT TLB maintenance.
      return;

    // Clean out residual EPT TLB entries during boot.
    Vm_vmx_ept_tlb::flush();

    register_cpu_tlb(cpu);
  }

  inline void tlb_flush() override
  {
    Vm_vmx_ept_tlb::flush();
  }

private:
  static Per_cpu<Vm_vmx_ept_tlb> cpu_tlbs;
};

class Vm_vmx_ept : public Vm_vmx_t<Vm_vmx_ept>
{
private:
  //typedef Mem_space::Attr Attr;
  //typedef Mem_space::Vaddr Vaddr;
  //typedef Mem_space::Vsize Vsize;
  //using Mem_space::Phys_addr;
  //using Mem_space::Page_order;

  class Epte_ptr
  {
    Unsigned64 *e;

    void set(Unsigned64 v);

  public:
    typedef Mem_space::Attr Attr;

    unsigned char level;
    Epte_ptr() = default;
    Epte_ptr(Unsigned64 *e, unsigned char level) : e(e), level(level) {}

    bool is_valid() const { return *e & 7; }
    bool is_leaf() const { return (*e & (1 << 7)) || level == 3; }
    Unsigned64 next_level() const
    { return cxx::get_lsb(cxx::mask_lsb(*e, 12), 52); }

    void clear() { set(0); }

    unsigned char page_order() const;
    Unsigned64 page_addr() const;
    Attr attribs() const
    {
      typedef L4_fpage::Rights R;
      typedef Page::Type T;

      auto raw = access_once(e);

      R r = R::UR();
      if (raw & 2) r |= R::W();
      if (raw & 4) r |= R::X();

      T t;
      switch (raw & 0x38)
        {
        case (0 << 3): t = T::Uncached(); break;
        case (1 << 3): t = T::Buffered(); break;
        default:
        case (6 << 3): t = T::Normal(); break;
        }

      return Attr(r, t);
    }

    Unsigned64 entry() const { return *e; }

    void set_next_level(Unsigned64 phys)
    { set(phys | 7); }

    void write_back_if(bool) const {}
    static void write_back(void *, void*) {}

    L4_fpage::Rights access_flags() const
    {
      return L4_fpage::Rights(0);
    }

    void del_rights(L4_fpage::Rights r)
    {
      Unsigned64 dr = 0;
      if (r & L4_fpage::Rights::W())
        dr = 2;

      if (r & L4_fpage::Rights::X())
        dr |= 4;

      if (dr)
        *e &= ~dr;
    }

    Unsigned64 make_page(Phys_mem_addr addr, Page::Attr attr)
    {
      typedef L4_fpage::Rights R;
      typedef Page::Type T;

      Unsigned64 r = (level < 3) ? (Unsigned64)(1 << 7) : 0;
      r |= 1; // R
      if (attr.rights & R::W()) r |= 2;
      if (attr.rights & R::X()) r |= 4;

      if (attr.type == T::Normal())   r |= 6 << 3;
      if (attr.type == T::Buffered()) r |= 1 << 3;
      if (attr.type == T::Uncached()) r |= 0;

      return cxx::int_value<Phys_mem_addr>(addr) | r;
    }

    void set_page(Unsigned64 p)
    {
      set(p);
    }

  };

  typedef Ptab::Tupel< Ptab::Traits<Unsigned64, 39, 9, false>,
                       Ptab::Traits<Unsigned64, 30, 9, true>,
                       Ptab::Traits<Unsigned64, 21, 9, true>,
                       Ptab::Traits<Unsigned64, 12, 9, true> >::List Ept_traits;

  typedef Ptab::Shift<Ept_traits, 12>::List Ept_traits_vpn;
  typedef Ptab::Page_addr_wrap<Page_number, 12> Ept_va_vpn;
  typedef Ptab::Base<Epte_ptr, Ept_traits_vpn, Ept_va_vpn, Mem_layout> Ept;

  Mword _ept_phys;
  Ept *_ept;
};

// -------------------------------------------------------------------------
IMPLEMENTATION [vmx && 64bit]:

IMPLEMENT inline
void
Vm_vmx_ept::Epte_ptr::set(Unsigned64 v) { write_now(e, v); }

// -------------------------------------------------------------------------
IMPLEMENTATION [vmx && !64bit]:

IMPLEMENT inline
void
Vm_vmx_ept::Epte_ptr::set(Unsigned64 v)
{
  // this assumes little endian!
  union T
  {
    Unsigned64 l;
    Unsigned32 u[2];
  };

  T *t = (T*)e;

  write_now(&t->u[0], Unsigned32(0));
  write_now(&t->u[1], Unsigned32(v >> 32));
  write_now(&t->u[0], Unsigned32(v));
}

// -------------------------------------------------------------------------
IMPLEMENTATION [vmx]:

IMPLEMENT inline
unsigned char
Vm_vmx_ept::Epte_ptr::page_order() const
{ return Vm_vmx_ept::Ept::page_order_for_level(level); }

IMPLEMENT inline
Unsigned64
Vm_vmx_ept::Epte_ptr::page_addr() const
{ return cxx::get_lsb(cxx::mask_lsb(*e, Vm_vmx_ept::Ept::page_order_for_level(level)), 52); }

static Mem_space::Fit_size __ept_ps;

PUBLIC
Mem_space::Fit_size const &
Vm_vmx_ept::mem_space_fitting_sizes() const override
{ return __ept_ps; }

PUBLIC static
void
Vm_vmx_ept::add_page_size(Mem_space::Page_order o)
{
  add_global_page_size(o);
  __ept_ps.add_page_size(o);
}

PUBLIC
void
Vm_vmx_ept::tlb_flush(bool) override
{
  Vm_vmx_ept_tlb::flush_single(_ept_phys);
  tlb_mark_unused();
}

PUBLIC
bool
Vm_vmx_ept::v_lookup(Mem_space::Vaddr virt, Mem_space::Phys_addr *phys,
                     Mem_space::Page_order *order,
                     Mem_space::Attr *page_attribs) override
{
  auto i = _ept->walk(virt);
  if (order) *order = Mem_space::Page_order(i.page_order());

  if (!i.is_valid())
    return false;

  // FIXME: we may get a problem on 32 bit systems and using more than 4G ram
  if (phys) *phys = Mem_space::Phys_addr(i.page_addr());
  if (page_attribs) *page_attribs = i.attribs();

  return true;
}
PUBLIC
Mem_space::Status
Vm_vmx_ept::v_insert(Mem_space::Phys_addr phys, Mem_space::Vaddr virt,
                     Mem_space::Page_order size,
                     Mem_space::Attr page_attribs) override
{
  // insert page into page table

  // XXX should modify page table using compare-and-swap

  assert (cxx::is_zero(cxx::get_lsb(Mem_space::Phys_addr(phys), size)));
  assert (cxx::is_zero(cxx::get_lsb(Virt_addr(virt), size)));

  int level;
  for (level = 0; level <= Ept::Depth; ++level)
    if (Mem_space::Page_order(Ept::page_order_for_level(level)) <= size)
      break;

  auto i = _ept->walk(virt, level, false,
                      Kmem_alloc::q_allocator(ram_quota()));

  if (EXPECT_FALSE(!i.is_valid() && i.level != level))
    return Mem_space::Insert_err_nomem;

  if (EXPECT_FALSE(i.is_valid()
                   && (i.level != level || Mem_space::Phys_addr(i.page_addr()) != phys)))
    return Mem_space::Insert_err_exists;

  auto entry = i.make_page(phys, page_attribs);

  if (i.is_valid())
    {
      if (EXPECT_FALSE(i.entry() == entry))
        return Mem_space::Insert_warn_exists;

      i.set_page(entry);
      return Mem_space::Insert_warn_attrib_upgrade;
    }
  else
    {
      i.set_page(entry);
      return Mem_space::Insert_ok;
    }

}

PUBLIC
L4_fpage::Rights
Vm_vmx_ept::v_delete(Mem_space::Vaddr virt, Mem_space::Page_order size,
                     L4_fpage::Rights page_attribs) override
{
  (void)size;
  assert (cxx::is_zero(cxx::get_lsb(Virt_addr(virt), size)));

  auto i = _ept->walk(virt);

  if (EXPECT_FALSE (! i.is_valid()))
    return L4_fpage::Rights(0);

  L4_fpage::Rights ret = i.access_flags();

  if (! (page_attribs & L4_fpage::Rights::R()))
    {
      // downgrade PDE (superpage) rights
      i.del_rights(page_attribs);
    }
  else
    {
      // delete PDE (superpage)
      i.clear();
    }

  return ret;
}

PUBLIC
void
Vm_vmx_ept::v_set_access_flags(Mem_space::Vaddr, L4_fpage::Rights) override
{}

PUBLIC inline
void *
Vm_vmx_ept::operator new (size_t size, void *p) throw()
{
  (void)size;
  assert (size == sizeof (Vm_vmx_ept));
  return p;
}

PUBLIC
void
Vm_vmx_ept::operator delete (void *ptr)
{
  Vm_vmx_ept *t = reinterpret_cast<Vm_vmx_ept*>(ptr);
  Kmem_slab_t<Vm_vmx_ept>::q_free(t->ram_quota(), ptr);
}

PUBLIC inline
Vm_vmx_ept::Vm_vmx_ept(Ram_quota *q) : Vm_vmx_t<Vm_vmx_ept>(q)
{
  _tlb_type = Tlb_per_cpu_asid;
}

PUBLIC
Vm_vmx_ept::~Vm_vmx_ept()
{
  if (_ept)
    {
      _ept->destroy(Virt_addr(0UL), Virt_addr(~0UL), 0, Ept::Depth,
                    Kmem_alloc::q_allocator(ram_quota()));
      Kmem_alloc::allocator()->q_free(ram_quota(), Config::page_order(), _ept);
      _ept = 0;
      _ept_phys = 0;
    }
}

PUBLIC inline
bool
Vm_vmx_ept::initialize()
{
  void *b;
  if (EXPECT_FALSE(!(b = Kmem_alloc::allocator()
	  ->q_alloc(ram_quota(), Config::page_order()))))
    return false;

  _ept = static_cast<Ept*>(b);
  _ept->clear(false);	// initialize to zero
  _ept_phys = Mem_layout::pmem_to_phys(_ept);
  return true; // success

}

PUBLIC inline
void
Vm_vmx_ept::load_vm_memory(void *src)
{
  load(Vmx::F_guest_cr3, src);
  Vmx::vmwrite(Vmx::F_ept_ptr, _ept_phys | 6 | (3 << 3));

  tlb_mark_used();
}

PUBLIC inline
void
Vm_vmx_ept::store_vm_memory(void *dest)
{
  store(Vmx::F_guest_cr3, dest);
}

PUBLIC static
void
Vm_vmx_ept::init()
{
  auto const &vmx = Vmx::cpus.cpu(Cpu_number::boot_cpu());
  if (!vmx.vmx_enabled())
    return;

  if (!vmx.info.procbased_ctls2.allowed(Vmx_info::PRB2_enable_ept))
    {
      Kobject_iface::set_factory(L4_msg_tag::Label_vm,
                                 Task::generic_factory<Vm_vmx>);
      return;
    }

  Kobject_iface::set_factory(L4_msg_tag::Label_vm,
                             Task::generic_factory<Vm_vmx_ept>);

  printf("VMX: init page sizes\n");
  add_page_size(Mem_space::Page_order(12));
  add_page_size(Mem_space::Page_order(21));
  add_page_size(Mem_space::Page_order(30));
}

STATIC_INITIALIZE(Vm_vmx_ept);

// -------------------------------------------------------------------------
IMPLEMENTATION[vmx]:

// Must be constructed after Vmx::cpus.
DEFINE_PER_CPU_LATE Per_cpu<Vm_vmx_ept_tlb>
  Vm_vmx_ept_tlb::cpu_tlbs(Per_cpu_data::Cpu_num);
