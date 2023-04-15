/*
 * (c) 2011 Alexander Warg <warg@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

#pragma once

namespace cxx { namespace Bits {

template< typename T >
class List_iterator_end_ptr
{
private:
  template< typename U > friend class Basic_list;
  static void *_end;
};

template< typename T >
void *List_iterator_end_ptr<T>::_end;

template< typename VALUE_T, typename TYPE >
struct Basic_list_policy
{
  typedef VALUE_T *Value_type;
  typedef VALUE_T const *Const_value_type;
  typedef TYPE **Type;
  typedef TYPE *Const_type;
  typedef TYPE *Head_type;
  typedef TYPE Item_type;

  static Type next(Type c) { return static_cast<Type>(&(*c)->_n); }
  static Const_type next(Const_type c) { return static_cast<Const_type>(c->_n); }
};

/// Internal: Common functions for all head-based list implementations.
template< typename POLICY >
class Basic_list
{
  Basic_list(Basic_list const &) = delete;
  void operator = (Basic_list const &) = delete;

public:
  typedef typename POLICY::Value_type Value_type;
  typedef typename POLICY::Const_value_type Const_value_type;

  class Iterator
  {
  private:
    typedef typename POLICY::Type Internal_type;

  public:
    typedef typename POLICY::Value_type value_type;
    typedef typename POLICY::Value_type Value_type;

    Value_type operator * () const { return static_cast<Value_type>(*_c); }
    Value_type operator -> () const { return static_cast<Value_type>(*_c); }
    Iterator operator ++ () { _c = POLICY::next(_c); return *this; }

    bool operator == (Iterator const &o) const { return *_c == *o._c; }
    bool operator != (Iterator const &o) const { return !operator == (o); }

    Iterator() : _c(__end()) {}

  private:
    friend class Basic_list;
    static Internal_type __end()
    {
      union X { Internal_type l; void **v; } z;
      z.v = &Bits::List_iterator_end_ptr<void>::_end;
      return z.l;
    }

    explicit Iterator(Internal_type i) : _c(i) {}

    Internal_type _c;
  };

  class Const_iterator
  {
  private:
    typedef typename POLICY::Const_type Internal_type;

  public:
    typedef typename POLICY::Value_type value_type;
    typedef typename POLICY::Value_type Value_type;

    Value_type operator * () const { return static_cast<Value_type>(_c); }
    Value_type operator -> () const { return static_cast<Value_type>(_c); }
    Const_iterator operator ++ () { _c = POLICY::next(_c); return *this; }

    friend bool operator == (Const_iterator const &lhs, Const_iterator const &rhs)
    { return lhs._c == rhs._c; }
    friend bool operator != (Const_iterator const &lhs, Const_iterator const &rhs)
    { return lhs._c != rhs._c; }

    Const_iterator() : _c(0) {}
    Const_iterator(Iterator const &o) : _c(*o) {}

  private:
    friend class Basic_list;

    explicit Const_iterator(Internal_type i) : _c(i) {}

    Internal_type _c;
  };

  // BSS allocation
  explicit Basic_list(bool) {}
  Basic_list() : _f(0) {}

  Basic_list(Basic_list &&o) : _f(o._f)
  {
    o.clear();
  }

  Basic_list &operator = (Basic_list &&o)
  {
    _f = o._f;
    o.clear();
    return *this;
  }

  /// Check if the list is empty.
  bool empty() const { return !_f; }
  /// Return the first element in the list.
  Value_type front() const { return static_cast<Value_type>(_f); }

  /**
   * Remove all elements from the list.
   *
   * After the operation the state of the elements is undefined.
   */
  void clear() { _f = 0; }

  /// Return an iterator to the beginning of the list.
  Iterator begin() { return Iterator(&_f); }
  /// Return a const iterator to the beginning of the list.
  Const_iterator begin() const { return Const_iterator(_f); }
  /**
   *  Return a const iterator that begins at the given element.
   *
   *  \param c  Element where the iterator should start.
   *
   *  \pre The element `c` must already be in a list.
   */
  static Const_iterator iter(Const_value_type c) { return Const_iterator(c); }
  /// Return a const iterator to the end of the list.
  Const_iterator end() const { return Const_iterator(nullptr); }
  /// Return an iterator to the end of the list.
  Iterator end() { return Iterator(); }

protected:
  static typename POLICY::Type __get_internal(Iterator const &i) { return i._c; }
  static Iterator __iter(typename POLICY::Type c) { return Iterator(c); }

  /// Pointer to front of the list.
  typename POLICY::Head_type _f;
};

}}

