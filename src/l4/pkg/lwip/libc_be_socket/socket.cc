#define LWIP_PREFIX_BYTEORDER_FUNCS 1

#include <l4/l4re_vfs/vfs.h>
#include <l4/l4re_vfs/backend>
#include <l4/cxx/ref_ptr>
#include <errno.h>
#include <cstring>

#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"


#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>

extern "C" {
int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_shutdown(int s, int how);
int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
 int lwip_close(int s);
int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_listen(int s, int backlog);
ssize_t lwip_recv(int s, void *mem, size_t len, int flags);
ssize_t lwip_read(int s, void *mem, size_t len);
ssize_t lwip_readv(int s, const struct iovec *iov, int iovcnt);
ssize_t lwip_recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen);
ssize_t lwip_recvmsg(int s, struct msghdr *message, int flags);
ssize_t lwip_send(int s, const void *dataptr, size_t size, int flags);
ssize_t lwip_sendmsg(int s, const struct msghdr *message, int flags);
ssize_t lwip_sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen);
int lwip_socket(int domain, int type, int protocol);
}

namespace {


class Socket_file : public L4Re::Vfs::Be_file
{
private:
  int _s;

public:
  Socket_file(int s) throw()
  : _s(s)
  {}

  ~Socket_file() throw()
  {
      lwip_close(_s);
  }

  static cxx::Ref_ptr<Socket_file> socket(int domain, int type, int protocol) throw();
  int bind(sockaddr const *, socklen_t) throw();
  int connect(sockaddr const *, socklen_t) throw();
  ssize_t send(void const *, size_t, int) throw();
  ssize_t recv(void *, size_t, int) throw();
  ssize_t sendto(void const *, size_t, int, sockaddr const *, socklen_t) throw();
  ssize_t recvfrom(void *, size_t, int, sockaddr *, socklen_t *) throw();
#if 0
  ssize_t sendmsg(msghdr const *, int) throw();
  ssize_t recvmsg(msghdr *, int) throw();
  int getsockopt(int level, int opt, void *, socklen_t *) throw();
  int setsockopt(int level, int opt, void const *, socklen_t) throw();
#endif
  int listen(int) throw();
  int accept(sockaddr *addr, socklen_t *) throw();
#if 0
  int shutdown(int) throw();

  int getsockname(sockaddr *, socklen_t *) throw();
  int getpeername(sockaddr *, socklen_t *) throw();
#endif
  ssize_t readv(const struct iovec *vec, int iovcnt) throw();
  ssize_t writev(const struct iovec *vec, int iovcnt) throw();

};



cxx::Ref_ptr<Socket_file>
Socket_file::socket(int domain, int type, int protocol) throw()
{
  using cxx::Ref_ptr;

  int s = lwip_socket(domain, type, protocol);

  if (s < 0)
      return Ref_ptr<>::Nil;

  Ref_ptr<Socket_file> ss(new Socket_file(s));
  if (!ss)
    {
      lwip_close(s);
      errno = ENOBUFS;
      return Ref_ptr<>::Nil;
    }
  return ss;
}

int
Socket_file::accept(sockaddr *addr, socklen_t *len) throw()
{
    return lwip_accept(_s, addr, len);
}

int
Socket_file::bind(sockaddr const *addr, socklen_t len) throw()
{

  return lwip_bind(_s, addr, len);
}

int
Socket_file::connect(sockaddr const *addr, socklen_t len) throw()
{
  return lwip_connect(_s, addr, len);
}

int
Socket_file::listen(int backlog) throw()
{
  return lwip_listen(_s, backlog);
}

ssize_t
Socket_file::recvfrom(void *_buf, size_t len, int flags, sockaddr *from, socklen_t *fromlen) throw()
{
  return lwip_recvfrom(_s, _buf, len, flags, from, fromlen);
}

ssize_t
Socket_file::sendto(void const *data, size_t size, int flags, sockaddr const *to, socklen_t tolen) throw()
{
  return lwip_sendto(_s, data, size, flags, to, tolen);
}

ssize_t
Socket_file::send(void const *data, size_t size, int flags) throw()
{
  return lwip_send(_s, data, size, flags);
}

ssize_t
Socket_file::recv(void *data, size_t size, int flags) throw()
{ return lwip_recv(_s, data, size, flags); }

ssize_t
Socket_file::readv(const struct iovec *vec, int iovcnt) throw()
{
  if (iovcnt > 1) printf("Socket_file::readv: Implement iovcnt>1\n");
  return recvfrom(vec[0].iov_base, vec[0].iov_len, 0, 0, 0);
}

ssize_t
Socket_file::writev(const struct iovec *vec, int iovcnt) throw()
{
  if (iovcnt > 1) printf("Socket_file::writev: Implement iovcnt>1\n");
  return send(vec[0].iov_base, vec[0].iov_len, 0);
}


}

int socket(int domain, int type, int protocol) __THROW
{
  using cxx::Ref_ptr;
  Ref_ptr<Socket_file> s(Socket_file::socket(domain, type, protocol));
  if (!s)
    return -1;

  int fd = L4Re::Vfs::vfs_ops->alloc_fd(s);
  if (fd < 0)
    {
      errno = ENFILE;
      return -1;
    }

  return fd;
}


