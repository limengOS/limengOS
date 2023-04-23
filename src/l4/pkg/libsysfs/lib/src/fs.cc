/**
 * Copyright (C) 2021-2022 lili
 * Author: lili
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 *
 */

#include <l4/l4re_vfs/backend>
#include <l4/cxx/string>
#include <l4/cxx/avl_tree>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/re/rm>
#include <l4/re/dataspace>
#include <l4/cxx/ipc_stream>
#include <l4/util/bitops.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>

namespace {

using namespace L4Re::Vfs;
using cxx::Ref_ptr;

enum
{
  OP_OPEN   = 0,
  OP_READ,
  OP_WRITE,
  OP_CLOSE,
  OP_FSTAT,
  OP_LSEEK,
  OP_FTRUNCATE64,
  OP_FCHMOD,
  OP_MKDIR,
  OP_PIPE,
};

static void *allocate_new_region(long size, long mem_flags,
                                             long attach_flags)
{
    void * ret = NULL;
    long   err = 0;

    L4::Cap<L4Re::Dataspace> ds = L4Re::Util::cap_alloc.alloc<L4Re::Dataspace>();
    if (!ds.is_valid()) {
        printf("libsysfs allocate ds : failed.\n");
        goto out;
    }

    err =  L4Re::Env::env()->mem_alloc()->alloc(size, ds, mem_flags);
    if (err < 0){
        printf("libsysfs allocate_new_region: failed!\n");
        goto out;
    }

    err = L4Re::Env::env()->rm()->attach(&ret, size, 
                                          (L4Re::Rm::F::Attach_flags)attach_flags
                                            | L4Re::Rm::F::Search_addr
                                            | L4Re::Rm::F::RW ,
                                             L4::Ipc::make_cap(ds, (attach_flags & L4Re::Rm::F::R)
                                                                   ? L4_CAP_FPAGE_RO
                                                                   : L4_CAP_FPAGE_RW),
                                             0, l4util_log2(size) + 1);
out:
    return ret;
}

static void release_region(void *addr)
{
    L4::Cap<L4Re::Dataspace> ds;
    int err = L4Re::Env::env()->rm()->detach((l4_addr_t)addr, &ds);

    if (err < 0)
        printf("libsysfs release_region: Detach failed!");

    if (ds.is_valid()) {
        //ds->release();        //not support at op_release, see moe dataspace.h
        L4Re::Util::cap_alloc.free(ds);
    }
}

// int find(l4_addr_t *addr, unsigned long *size, l4_addr_t *offset,
//           unsigned *flags, L4::Cap<Dataspace> *m) 
static int get_region_ds(void *bf, size_t sz, L4::Cap<L4Re::Dataspace> *ds)
{
    L4Re::Rm::Offset of;
    L4Re::Rm::Flags fg;

    return L4Re::Env::env()->rm()->find((l4_addr_t*)bf, (unsigned long *)&sz, &of, &fg, ds);
}

class Sysfs_dir : public Be_file
{
public:
  explicit Sysfs_dir(void) throw() {}
  int get_entry(const char *, int, mode_t, Ref_ptr<File> *) throw();
  int mkdir(const char *, mode_t) throw();
  int pipe(Ref_ptr<File> file[2]) throw();
  #if   0
  //ssize_t getdents(char *, size_t) throw();
  int fstat64(struct stat64 *buf) const throw();
  int utime(const struct utimbuf *) throw();
  int fchmod(mode_t) throw();
  int unlink(const char *) throw();
  int rename(const char *, const char *) throw();
  int faccessat(const char *, int, int) throw();
  #endif

private:
};

class Sysfs_fs : public Be_file_system
{
public:
  Sysfs_fs() : Be_file_system("sysfs") 
  {
     server = L4Re::Env::env()->get_cap<void>("sysfs");
     if (!server.is_valid())
     {
        printf("Could not get capability sysfs !\n");
        return;
     }
     vfs_ops->mount("", "sys", "sysfs", 0, 0);

  }

  int mount(char const *source, unsigned long mountflags,
            void const *data, cxx::Ref_ptr<File> *dir) throw()
  {
    (void)mountflags;
    (void)source;
    (void)data;
    *dir = cxx::ref_ptr(new Sysfs_dir());
    if (!dir->ptr())
      return -ENOMEM;
    return 0;
  }

  L4::Cap<void> server ;
};

class Sysfs_file : public Be_file
{
public:
  explicit Sysfs_file(int f, unsigned int pages) throw()
    : Be_file(), _f(f), _pgs(pages) 
    {
        _d =(char*) allocate_new_region(L4_PAGESIZE, L4Re::Mem_alloc::Continuous | L4Re::Mem_alloc::Pinned,
                                            L4Re::Rm::F::Eager_map);
    }

  off64_t size() const throw();
  int fstat64(struct stat64 *buf) const throw();
  int ftruncate64(off64_t p) throw();
  int ioctl(unsigned long, va_list) throw();
  int utime(const struct utimbuf *) throw();
  int fchmod(mode_t) throw();
  int unlock_all_locks() throw();
  ssize_t readv(const struct iovec *v, int iovcnt) throw();
  ssize_t writev(const struct iovec *v, int iovcnt) throw();
  ssize_t rwv(const struct iovec *v, int iovcnt, L4::Opcode op) throw();
  
  long lseek64(off_t offset, unsigned int origin) throw();

  ~Sysfs_file() throw();        //virtual ~File() throw() = 0

private:
  ssize_t page_rw(void *bf, size_t sz, L4::Opcode op) throw();
  ssize_t map_bufpage_read_write(void *bf, size_t sz, L4::Opcode op) throw();
  ssize_t read_write(void *bf, size_t sz, L4::Opcode op) throw();

  int _f;
  unsigned int _pgs;
  char *_d;
};

L4::Cap<void> sysfs_get_server();
int
Sysfs_dir::get_entry(const char *name, int flags, mode_t mode,
                     Ref_ptr<File> *file) throw()
{
  if (!*name)
    {
      return -ENOENT;
    }
  char *d =(char*) allocate_new_region(L4_PAGESIZE, L4Re::Mem_alloc::Continuous | L4Re::Mem_alloc::Pinned,
                                            L4Re::Rm::F::Eager_map);
  if (!d)
      return -ENOMEM;

  snprintf(d, L4_PAGESIZE,"%s", name);

  L4::Ipc::Iostream s(l4_utcb());

  s << L4::Opcode(OP_OPEN);
  s << (int)flags;
  s << (mode_t)mode;
  s << L4::Ipc::Snd_fpage::mem((l4_addr_t)d, L4_PAGESHIFT,
                                L4_FPAGE_RWX, 0);
  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  
 
  if (r >= 0)
  {
    long f;
    int  n;
    s >> f >> n;
    if (f >= 0) {
        *file = cxx::ref_ptr(new Sysfs_file(static_cast<int>(f), (unsigned int)n));
        if (!file->ptr())
            r = -ENOMEM;
    } else
        r = f;
  }

  release_region((void*)d);

  return r;
}

int 
Sysfs_dir::mkdir(const char *name, mode_t mode) throw()
{
  if (!*name)
    {
      return -ENOENT;
    }
  char *d =(char*) allocate_new_region(L4_PAGESIZE, L4Re::Mem_alloc::Continuous | L4Re::Mem_alloc::Pinned,
                                            L4Re::Rm::F::Eager_map);
  if (!d)
      return -ENOMEM;

  snprintf(d, L4_PAGESIZE,"%s", name);

  L4::Ipc::Iostream s(l4_utcb());

  s << L4::Opcode(OP_MKDIR);
  s << (mode_t)mode;
  s << L4::Ipc::Snd_fpage::mem((l4_addr_t)d, L4_PAGESHIFT,
                                L4_FPAGE_RWX, 0);
  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  
 
  if (r >= 0)
    s >> r;

  release_region((void*)d);

  return r;
}

int 
Sysfs_dir::pipe(Ref_ptr<File> file[2]) throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << L4::Opcode(OP_PIPE);
  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
 
  if (r >= 0)
  {
    long r2;
    int f, f1, n;

    s >> r2 >> f >> f1 >> n;
    if (r2 >= 0) {
        file[0] = cxx::ref_ptr(new Sysfs_file(static_cast<int>(f), (unsigned int)n));
        file[1] = cxx::ref_ptr(new Sysfs_file(static_cast<int>(f1), (unsigned int)n));
        if (!(&file[1])->ptr())
            r = -ENOMEM;
    } else
        r = (int)r2;
  }

  return r;
}

ssize_t Sysfs_file::page_rw(void *bf, size_t sz, L4::Opcode op) throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << (L4::Opcode) op
    << (int) _f
    << (size_t)sz;

  int o = l4util_log2(sz);      //page align
  if (o < L4_PAGESHIFT)
    o = L4_PAGESHIFT;

  s << L4::Ipc::Snd_fpage::mem(l4_trunc_size((l4_addr_t)bf, o), o,
                                L4_FPAGE_RWX, 0);
  
  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  if (r < 0)
    return r; // failure
  
  long ret;
  s >> ret;
    
  return ret;
}

ssize_t Sysfs_file::map_bufpage_read_write(void *bf, size_t sz, L4::Opcode op) throw()
{
    if (! sz)
        return sz;

    l4_addr_t b = (l4_addr_t)bf;
    ssize_t   r = 0;
    size_t    n, n2, left = sz;
    unsigned char order, *bp;
        
    n = (size_t)(b & (L4_PAGESIZE - 1));
    if (n){
        n2 = L4_PAGESIZE - n <= left ? L4_PAGESIZE - n : left;
        if (op == OP_WRITE)
            memcpy(_d, bf, n2);
        r = page_rw(_d, n2, op);
        if (r <= 0 || (size_t)r > n2)
            return r;
        if (op == OP_READ)
            memcpy(bf, _d, r);
        if ((size_t)r < n2)
            return r;
        left -= (size_t)r;
        b = l4_round_page(b);
    }

    for (; left >= L4_PAGESIZE; left -= (size_t)r, b += (size_t)r){
        order = l4_fpage_max_order(L4_PAGESHIFT, b, b, b + left, 0);
        if (order > _pgs)
            order = _pgs;

        bp = (unsigned char*)b;
        for(int i = 0; i < (1 << (order - L4_PAGESHIFT)); i++){
            *(volatile unsigned char *)bp = *bp;  //force map page and not optimized by gcc
            bp += L4_PAGESIZE;
        }
        n = (size_t)(1 << order);

        r = page_rw((void*)b, n, op);
        if (r < 0)
            r = 0;
        if ((size_t)r != n)    //file end
            return sz - left + r;
    }
    if (left) { 
        if (op == OP_WRITE)
            memcpy(_d, (const void*)b, left);
        r = page_rw(_d, left, op);
        if (r > 0 && (size_t)r <= left) {
            if (op == OP_READ)
                memcpy((void*)b, _d, r);
            left -= (size_t)r;
        }
    }
        
    return sz - left;
}

ssize_t Sysfs_file::read_write(void *bf, size_t sz, L4::Opcode op) throw()
{
    l4_addr_t b = (l4_addr_t)bf;
    ssize_t   r = 0;
    size_t    n, left = sz;

    for (; left > 0; left -= (size_t)r, b += (size_t)r){
        n = left > L4_PAGESIZE ? L4_PAGESIZE : left;

        if (op == OP_WRITE)
            memcpy(_d, (void*)b, n);
        r = page_rw(_d, n, op);
        if (r < 0)
            return r;
        if (op == OP_READ && (size_t)r <= n)
            memcpy((void*)b, _d, r);
        if ((size_t)r != n)    //file end
            return sz - left + r;
    }
        
    return sz - left;
}

ssize_t Sysfs_file::rwv(const struct iovec *v, int iovcnt, L4::Opcode op) throw()
{
  L4::Cap<L4Re::Dataspace> ds;
  ssize_t n = 0, sum = 0;
  for (int i = 0; i < iovcnt; ++i)
    {
      if (v[i].iov_len < L4_PAGESIZE){
        n = read_write(v[i].iov_base, v[i].iov_len, op);
      }else {
        n = map_bufpage_read_write(v[i].iov_base, v[i].iov_len, op);
      }
      if (n <= 0)
        break;
      sum += n;
    }
  return sum == 0 ? n : sum;
}

ssize_t Sysfs_file::readv(const struct iovec *v, int iovcnt) throw()
{
  if (iovcnt < 0)
    return -EINVAL;
  
  return rwv(v, iovcnt, OP_READ);
}

ssize_t Sysfs_file::writev(const struct iovec *v, int iovcnt) throw()
{
  if (iovcnt < 0)
    return -EINVAL;
  
  return rwv(v, iovcnt, OP_WRITE);
}


int Sysfs_file::unlock_all_locks() throw()
{
  return 0;
}

int Sysfs_file::fstat64(struct stat64 *buf) const throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << (L4::Opcode) OP_FSTAT
    << (int) _f;

  s << L4::Ipc::Snd_fpage::mem((l4_addr_t)_d, L4_PAGESHIFT,
                                L4_FPAGE_RWX, 0);

  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  if (r < 0)
    return r; // failure

  long ret;
  s >> ret;

  if(!ret)
    memcpy(buf, _d, sizeof(struct stat64) < L4_PAGESIZE ? sizeof(struct stat64) : L4_PAGESIZE);

  return ret;
}

long Sysfs_file::lseek64(off_t offset, unsigned int origin) throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << (L4::Opcode) OP_LSEEK
    << (int) _f
    << (off_t) offset
    << (unsigned int) origin;

  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  if (r < 0)
    return r; // failure

  long ret;
  s >> ret;
  return ret;
}

int Sysfs_file::ftruncate64(off64_t p) throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << (L4::Opcode) OP_FTRUNCATE64
    << (int) _f
    << (off64_t) p;

  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  if (r < 0)
    return r; // failure

  long ret;
  s >> ret;
  return ret;
}

off64_t Sysfs_file::size() const throw()
{ return 0; }

int
Sysfs_file::ioctl(unsigned long v, va_list args) throw()
{
  switch (v)
    {
    case FIONREAD: // return amount of data still available
      int *available = va_arg(args, int *);
      *available = 0;//_file->data().size() - pos();
      return 0;
    };
  return -EINVAL;
}

int
Sysfs_file::utime(const struct utimbuf *times) throw()
{
  //_file->info()->st_atime = times->actime;
  //_file->info()->st_mtime = times->modtime;
  return 0;
}

int
Sysfs_file::fchmod(mode_t m) throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << (L4::Opcode) OP_FCHMOD
    << (int) _f
    << (mode_t) m;

  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  if (r < 0)
    return r; // failure

  long ret;
  s >> ret;
  return ret;
}

Sysfs_file::~Sysfs_file() throw()
{
  L4::Ipc::Iostream s(l4_utcb());

  s << (L4::Opcode) OP_CLOSE
    << (int) _f;
  int r = l4_error(s.call(sysfs_get_server().cap(), L4RE_PROTO_SYSFS));
  if (r >= 0){
    long ret;
    s >> ret;
  }

  release_region((void*)_d);
}

static Sysfs_fs _sysfs L4RE_VFS_FILE_SYSTEM_ATTRIBUTE;

L4::Cap<void> sysfs_get_server()
{
    return _sysfs.server;
}

}
