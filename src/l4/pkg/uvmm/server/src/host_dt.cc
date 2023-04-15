/*
 * Copyright (C) 2018 Kernkonzept GmbH.
 * Author(s): Sarah Hoffmann <sarah.hoffmann@kernkonzept.com>
 *
 * This file is distributed under the terms of the GNU General Public
 * License, version 2.  Please see the COPYING-GPL-2 file for details.
 */

#include <cerrno>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <l4/cxx/minmax>
#include <l4/re/error_helper>

#include "debug.h"
#include "host_dt.h"

static Dbg warn(Dbg::Core, Dbg::Warn, "main");

namespace {

  class Mapped_file
  {
  public:
    explicit Mapped_file(char const *name)
    {
      int fd = open(name, O_RDWR);
      if (fd < 0)
        {
          warn.printf("Unable to open file '%s': %s\n", name, strerror(errno));
          return;
        }

      struct stat buf;
      if (fstat(fd, &buf) < 0)
        {
          warn.printf("Unable to get size of file '%s': %s\n", name,
                       strerror(errno));
          close(fd);
          return;
        }
      _size = buf.st_size;

      _addr = mmap(nullptr, _size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
      if (_addr == MAP_FAILED)
        warn.printf("Unable to mmap file '%s': %s\n", name, strerror(errno));

      close(fd);
    }
    Mapped_file(Mapped_file &&) = delete;
    Mapped_file(Mapped_file const &) = delete;

    ~Mapped_file()
    {
      if (_addr != MAP_FAILED)
        {
          if (munmap(_addr, _size) < 0)
            warn.printf("Unable to unmap file at addr %p: %s\n",
                        _addr, strerror(errno));
        }
    }

    void *get() const { return _addr; }
    bool valid() { return _addr != MAP_FAILED; }

  private:
    size_t _size = 0;
    void *_addr = MAP_FAILED;
  };

}

void
Vdev::Host_dt::add_source(char const *fname)
{
  Mapped_file mem(fname);
  if (!mem.valid())
    L4Re::chksys(-L4_EINVAL, "Unable to access overlay");

  if (valid())
    {
      get().apply_overlay(mem.get(), fname);
      return;
    }

  Dtb::Fdt fdt(mem.get());
  Device_tree dt(&fdt);

  dt.check_tree();

  // XXX would be nice to expand dynamically
  _fdt = new Dtb::Fdt(fdt, cxx::max(dt.size(), 0x200U));
}

void
Vdev::Host_dt::set_command_line(char const *cmd_line)
{
  if (!valid() || !cmd_line)
    return;

  // assume /choosen is present at this point
  auto node = get().path_offset("/chosen");
  node.setprop_string("bootargs", cmd_line);



  // Do pragmatic sanity check on console= argument of a (Linux) cmdline and
  // provide hints.
  printf("Command line: %s\n", cmd_line);
  if (strstr(cmd_line, "console="))
    {
      bool found_virtio_con = false;
      bool found_pl011 = false;
      bool found_8250 = false;
      get().scan([&] (Vdev::Dt_node const &node, unsigned /* depth */)
        {

          char const *const comp = "compatible";
          int count = node.stringlist_count(comp);
          if (count <= 0)
            return true;

          for (int i = 0; i < count; ++i)
            {
              int cid_len;
              char const *cid = node.stringlist_get(comp, i, &cid_len);
              if (!cid)
                continue;

              if (!strncmp(cid, "virtio,mmio", cid_len))
                {
                  int prop_size;
                  const char *p = node.get_prop<char>("l4vmm,vdev", &prop_size);
                  if (p && !strncmp(p, "console", prop_size))
                    found_virtio_con = true;
                }
              else if (   !strncmp(cid, "arm,pl011", cid_len)
                       or !strncmp(cid, "arm,sbsa-uart", cid_len))
                found_pl011 = true;
              else if (   !strncmp(cid, "uart,8250", cid_len)
                       or !strncmp(cid, "ns8250", cid_len))
                found_8250 = true;
            }
          return true;
        },
        [] (Vdev::Dt_node const &, unsigned) {});

      if (!found_virtio_con && strstr(cmd_line, "console=hvc"))
        printf("HINT: Found console=hvc on the command line but no "
               "VirtIO console device in device tree!\n"
               "HINT: VM-Output likely not to work!\n");
      if (!found_pl011 && strstr(cmd_line, "console=ttyAMA"))
        printf("HINT: Found console=ttyAMA on the command line but no "
               "arm,pl011/sbsa-uart device in device tree!\n"
               "HINT: VM-Output likely not to work!\n");
      if (!found_8250 && strstr(cmd_line, "console=ttyS"))
        printf("HINT: Found console=ttyS on the command line but no "
               "uart,8250/ns16550a device in device tree!\n"
               "HINT: VM-Output likely not to work!\n");
    }
}
