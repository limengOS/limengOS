/**
 * Copyright (C) 2021-2022 lili
 * Author: lili
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 *
 */

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <l4/re/env>
#include <l4/re/util/cap_alloc>
#include <l4/re/util/object_registry>
#include <l4/re/util/br_manager>
#include <l4/re/error_helper>
#include <l4/util/util.h>

#include <l4/cxx/ipc_server>

#include <pthread.h>
#include <pthread-l4.h>
#include <errno.h>

/*  from dde/ddekit/src/thread.c */
struct ddekit_condvar {
    pthread_cond_t cond;
};
typedef struct ddekit_condvar ddekit_condvar_t;

struct ddekit_thread {
    pthread_t pthread;
    void *data;
    void *stack;
    ddekit_condvar_t *sleep_cv;
    const char *name;
};
typedef struct ddekit_thread ddekit_thread_t;

extern "C" {
long sys_open( const char * filename, int flags, int mode);
long sys_read( unsigned int, char * , size_t);
long sys_write( unsigned int, char * , size_t);
long sys_close(unsigned int);
long sys_fstat64(unsigned long fd, struct stat64 * statbuf);
long sys_lseek(unsigned int fd, off_t offset, unsigned int origin);
long sys_ftruncate(unsigned int fd, unsigned long length);
long sys_fchmod(unsigned int fd, mode_t m);
long sys_mkdir(const char *, mode_t);
long sys_pipe(int *fildes);

void prepare_namespace(void);
int mck_root_data_setup(char const *, char const *);

ddekit_thread_t *ddekit_thread_setup_myself(const char *name);
int l4dde26_process_add_worker(void);
};

enum
{
  RESERVE_SHIFT   = 4,
  BUF_N_PAGES     = 1 << RESERVE_SHIFT,
  BUF_SHIFT       = L4_PAGESHIFT + RESERVE_SHIFT,
};

#if 0
class Loop_hooks :
  public L4::Ipc_svr::Timeout_queue_hooks<Loop_hooks, L4Re::Util::Br_manager>,
  public L4::Ipc_svr::Ignore_errors
{
public:
  static l4_kernel_clock_t now()
  { return l4_kip_clock(l4re_kip()); }
};
#endif

static l4_addr_t buf_addr;
static int buf_pages;

class Br_manager_buffers : public L4::Ipc_svr::Server_iface
{
public:
  int alloc_buffer_demand(Demand const &demand)
  {
    (void)demand;

    _addr = 0;
    for(_o = BUF_SHIFT; _o >= L4_PAGESHIFT; _o--)
    {
        if (L4Re::Env::env()->rm()->reserve_area(&_addr, 1 << _o,
                                                  L4Re::Rm::F::Search_addr, _o) == 0)
            break;
    }
    if (_o < L4_PAGESHIFT)
      return -L4_ENOMEM;

    buf_addr = _addr;
    buf_pages = 1 << (_o - L4_PAGESHIFT);

    printf("Sysfs reserve_area buf_addr=0x%lx, pages=%d(%d)\n", buf_addr, buf_pages, BUF_N_PAGES);
    return L4_EOK;
  }

  /// Returns L4::Cap<void>::Invalid, we have no buffer management
  L4::Cap<void> get_rcv_cap(int) const
  { return L4::Cap<void>::Invalid; }

  /// Returns -L4_ENOMEM, we have no buffer management
  int realloc_rcv_cap(int)
  { return -L4_ENOMEM; }

  /// Returns -L4_ENOSYS, we have no timeout queue
  int add_timeout(L4::Ipc_svr::Timeout *, l4_kernel_clock_t)
  { return -L4_ENOSYS; }

  /// Returns -L4_ENOSYS, we have no timeout queue
  int remove_timeout(L4::Ipc_svr::Timeout *)
  { return -L4_ENOSYS; }

protected:
  /// Setup wait function for the server loop (Server<>).
  void setup_wait(l4_utcb_t *utcb, L4::Ipc_svr::Reply_mode)
  {
    l4_buf_regs_t *br = l4_utcb_br_u(utcb);
    br->bdr = 0;
    br->br[0] = L4_ITEM_MAP;
    br->br[1] = l4_fpage(_addr, _o, L4_FPAGE_RWX).raw;
  }
private:
  l4_addr_t _addr;
  unsigned int _o;
};

struct Loop_hooks :
  public L4::Ipc_svr::Ignore_errors,
  public L4::Ipc_svr::Default_timeout,
  public L4::Ipc_svr::Compound_reply,
  public Br_manager_buffers
{ };


class sys_file_server : public L4::Server_object_t<L4::Kobject>
{

typedef int (*Func_ptr)(L4::Ipc::Iostream &ios);
public:
  int dispatch(l4_umword_t obj, L4::Ipc::Iostream &ios);

  static int open(L4::Ipc::Iostream &ios);
  static int read(L4::Ipc::Iostream &ios);
  static int write(L4::Ipc::Iostream &ios);
  static int close(L4::Ipc::Iostream &ios);
  static int stat64(L4::Ipc::Iostream &ios);
  static int lseek(L4::Ipc::Iostream &ios);
  static int ftruncate(L4::Ipc::Iostream &ios);
  static int fchmod(L4::Ipc::Iostream &ios);
  static int mkdir(L4::Ipc::Iostream &ios);
  static int pipe(L4::Ipc::Iostream &ios);

  static Func_ptr func_p[] ;
};

/*
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
*/
sys_file_server::Func_ptr sys_file_server::func_p[] =
    { open,   read,   write, 
      close,  stat64, lseek,  ftruncate,
      fchmod, mkdir,  pipe,
    };

int sys_file_server::read(L4::Ipc::Iostream &ios)
{
  int fd;
  size_t len;

  ios >> fd;
  ios >> len;

  long r;
  r = sys_read(fd, (char *)buf_addr, len);

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::write(L4::Ipc::Iostream &ios)
{
  int fd;
  size_t len;

  ios >> fd;
  ios >> len;

  long r;
  r = sys_write(fd, (char *)buf_addr, len);

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::close(L4::Ipc::Iostream &ios)
{
  int fd;

  ios >> fd;

  long r;
  r = sys_close(fd);

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::open(L4::Ipc::Iostream &ios)
{
  char const *n = (const char*)buf_addr;
  

  int flags, mode;
  ios >> flags;
  ios >> mode;

  long fd = sys_open(n, flags, mode);
  //if (fd < 0)
  //  return -L4_EINVAL;

  ios << (long)fd << (int)buf_pages;
  return L4_EOK;
}

int sys_file_server::stat64(L4::Ipc::Iostream &ios)
{
  int fd;

  ios >> fd;

  long r;
  r =  sys_fstat64(fd, reinterpret_cast<struct stat64 *>(buf_addr));

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::lseek(L4::Ipc::Iostream &ios)
{
  int fd;
  off_t offset; 
  unsigned int origin;

  ios >> fd >> offset >> origin;

  long r;
  r = sys_lseek(fd, offset, origin);

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::ftruncate(L4::Ipc::Iostream &ios)
{
  int fd;
  unsigned long len;

  ios >> fd >> len;

  long r;
  r = sys_ftruncate(fd, len);

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::fchmod(L4::Ipc::Iostream &ios)
{
  int fd;
  mode_t m;

  ios >> fd >> m;

  long r;
  r = sys_fchmod(fd, m);

  ios << (long)r;
  
  return L4_EOK;
}

int sys_file_server::mkdir(L4::Ipc::Iostream &ios)
{
  char const *n = (const char*)buf_addr;
  int mode;

  ios >> mode;

  long r = sys_mkdir(n, mode);

  ios << (long)r;
  return L4_EOK;
}

int sys_file_server::pipe(L4::Ipc::Iostream &ios)
{
  int fildes[2];

  long r = sys_pipe(fildes);

  ios << (long)r;
  ios << (int)fildes[0] << (int)fildes[1] << (int)buf_pages;
  return L4_EOK;
}

int
sys_file_server::dispatch(l4_umword_t, L4::Ipc::Iostream &ios)
{
  l4_msgtag_t t;
        l4_msg_regs_t *mr_p = l4_utcb_mr_u(l4_utcb());

  ios >> t;

  if (t.label() != L4RE_PROTO_SYSFS)
    return -L4_EBADPROTO;

  L4::Opcode opcode;
  ios >> opcode;

  if ((unsigned int)opcode >= (sizeof(func_p)/sizeof(func_p[0])))
    return -L4_ENOSYS;

  return func_p[opcode](ios);
}

typedef L4Re::Util::Registry_server<Loop_hooks> Sysfs_server;
static Sysfs_server *sysfs_server;


static void* _server_loop_func(void *_svr)
{
  Sysfs_server *svr = static_cast<Sysfs_server *>(_svr);

  ddekit_thread_setup_myself("sysfs");
  l4dde26_process_add_worker();
  svr->loop();
  return 0;
}


int start_sysfs(char *type, char *rt)
{

  if (sysfs_server)
    return 0;

  static sys_file_server file;
  pthread_t sysfs_server_thread;

  int e = pthread_create(&sysfs_server_thread, NULL, NULL, NULL);
  if (e != 0)
    {
      printf("fatal: could not create Sysfs handler thread: %d\n", -errno);
      return -1;
    }

  sysfs_server = new Sysfs_server(Pthread::L4::utcb(sysfs_server_thread),
                              Pthread::L4::cap(sysfs_server_thread),
                              L4Re::Env::env()->factory());

  // Register server
  if (!sysfs_server->registry()->register_obj(&file, "sysfs").is_valid())
    {
      delete sysfs_server;
      sysfs_server = 0;
      printf("Could not register my service\n");
      return -2;
    }

  e = Pthread::L4::start(sysfs_server_thread, _server_loop_func, sysfs_server);
  if (e < 0)
    {
      delete sysfs_server;
      sysfs_server = 0;
      printf("fatal: could not start Sysfs handler thread: %d\n", e);
      return 1;
    }

  printf("Created Sysfs handler thread\n");

  mck_root_data_setup(type, rt);
  l4_sleep(50);         //need wait mmc scan delay work
  prepare_namespace();

  return 0;
}

