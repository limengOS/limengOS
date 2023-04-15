/*
 * (c) 2008-2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>,
 *               Alexander Warg <warg@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 */
#pragma once

#include "dataspace.h"

namespace Moe {

/**
 * Dataspace with dynamic backing of physical memory.
 *
 * The dataspace is created in an empty state. Memory pages are allocated
 * and freed dynamically as they are requested by the client.
 */
class Dataspace_noncont : public Dataspace
{
public:
  enum
  {
    Page_addr_mask = ~((1UL << 12)-1),
    Page_cow = 0x04UL,
  };

  class Page
  {
  private:
    unsigned long p;

  public:
    Page() throw() : p(0) {}
    void *operator * () const throw() { return (void*)(p & Page_addr_mask); }
    bool valid() const throw() { return p & Page_addr_mask;}
    unsigned long flags() const throw() { return p & ~Page_addr_mask; }

    void set(void *addr, unsigned long flags) throw()
    { p = ((unsigned long)addr & Page_addr_mask) | (flags & ~Page_addr_mask); }

    void flags(unsigned long add, unsigned long del = 0) throw()
    {
      p = (p & Page_addr_mask & ~(del & ~Page_addr_mask))
        | ((unsigned long)add & ~Page_addr_mask);
    }

    void addr(void *a) throw()
    { p = (p & ~Page_addr_mask) | ((unsigned long)a & Page_addr_mask); }
  };

  bool is_static() const throw() override { return false; }

  Dataspace_noncont(unsigned long size,
                    Flags flags = L4Re::Dataspace::F::RWX) throw()
  : Dataspace(size, flags | Flags(Cow_enabled), L4_LOG2_PAGESIZE), _pages(0)
  {}

  virtual ~Dataspace_noncont() {}

  Address address(l4_addr_t offset,
                  Flags flags = L4Re::Dataspace::F::RWX, l4_addr_t hot_spot = 0,
                  l4_addr_t min = 0, l4_addr_t max = ~0) const override;
  int copy_address(l4_addr_t offset, Flags flags, l4_addr_t *copy_addr,
                   unsigned long *copy_size) const override;

  int pre_allocate(l4_addr_t offset, l4_size_t size, unsigned rights) override;

  virtual Page &page(unsigned long offs) const throw() = 0;
  virtual Page &alloc_page(unsigned long offs) const = 0;

  unsigned long num_pages() const throw()
  { return (size()+page_size()-1) / page_size(); }

#if 0
private:
  unsigned idx_of(unsigned long offset) const { return offset >> 12; }

  void *page(unsigned idx) const
  { return (void*)(pages[idx] & Page_addr_mask); }
  unsigned flags(unsigned idx) const { return pages[idx] & ~Page_addr_mask; }
  void page(unsigned idx, void *page, unsigned flags = 0) const
  {
    pages[idx] = (unsigned long)page & Page_addr_mask | flags
      & ~Page_addr_mask;
  }
#endif

  void free_page(Page &p) const throw();
  void unmap_page(Page const &p, bool ro = false) const throw();

public:
  long clear(unsigned long offs, unsigned long size) const throw() override;

  static Dataspace_noncont *create(Q_alloc *q, unsigned long size,
                                   Flags flags = L4Re::Dataspace::F::RWX);

protected:
  union
  {
    Page *_pages;
    Page _page;
  };

private:
  Address map_address(l4_addr_t offset, Flags flags) const;
};
};
