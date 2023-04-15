/*
 * Global kernel configuration
 */

INTERFACE:

#include <globalconfig.h>
#include "config_tcbsize.h"
#include "l4_types.h"

// special magic to allow old compilers to inline constants

#if defined(__clang__)
# define COMPILER "clang " __clang_version__
# define GCC_VERSION 409
#else
#if defined(__GNUC__)
# define COMPILER "gcc " __VERSION__
# define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
# define COMPILER "Non-GCC"
# define GCC_VERSION 0
#endif
#endif

#define GREETING_COLOR_ANSI_OFF    "\033[0m"

#define FIASCO_KERNEL_SUBVERSION 3

class Config
{
public:

  static const char *const kernel_warn_config_string;
  enum User_memory_access_type
  {
    No_access_user_mem = 0,
    Access_user_mem_direct,
    Must_access_user_mem_direct
  };

  enum {
    SERIAL_ESC_IRQ	= 2,
    SERIAL_ESC_NOIRQ	= 1,
    SERIAL_NO_ESC	= 0,
  };

  static void init();
  static void init_arch();

  // global kernel configuration
  enum
  {
    Kernel_version_id = 0x87004444 | (FIASCO_KERNEL_SUBVERSION << 16), // "DD....."
    // kernel (idle) thread prio
    Kernel_prio = 0,
    // default prio
    Default_prio = 1,

    Warn_level = CONFIG_WARN_LEVEL,

    One_shot_min_interval_us =   200,
    One_shot_max_interval_us = 10000,


#ifdef CONFIG_FINE_GRAINED_CPUTIME
    Fine_grained_cputime = true,
#else
    Fine_grained_cputime = false,
#endif

#ifdef CONFIG_STACK_DEPTH
    Stack_depth = true,
#else
    Stack_depth = false,
#endif
#ifdef CONFIG_NO_FRAME_PTR
    Have_frame_ptr = 0,
#else
    Have_frame_ptr = 1,
#endif
#ifdef CONFIG_JDB
    Jdb = 1,
#else
    Jdb = 0,
#endif
#ifdef CONFIG_JDB_LOGGING
    Jdb_logging = 1,
#else
    Jdb_logging = 0,
#endif
#ifdef CONFIG_JDB_ACCOUNTING
    Jdb_accounting = 1,
#else
    Jdb_accounting = 0,
#endif
#ifdef CONFIG_MP
    Max_num_cpus = CONFIG_MP_MAX_CPUS,
#else
    Max_num_cpus = 1,
#endif
#ifdef CONFIG_BIG_ENDIAN
    Big_endian = true,
#else
    Big_endian = false,
#endif
  };

  static Cpu_number max_num_cpus() { return Cpu_number(Max_num_cpus); }

  static bool getchar_does_hlt_works_ok;
  static bool esc_hack;
  static unsigned tbuf_entries;

  static constexpr Order page_order()
  { return Order(PAGE_SHIFT); }

  static constexpr Bytes page_size()
  { return Bytes(PAGE_SIZE); }
};

#define GREETING_COLOR_ANSI_TITLE  "\033[1;32m"
#define GREETING_COLOR_ANSI_INFO   "\033[0;32m"

INTERFACE[ia32]:
#define TARGET_NAME "x86-32"

INTERFACE[ux]:
#define TARGET_NAME "ux-x86-32"

INTERFACE[amd64]:
#define TARGET_NAME "x86-64"

INTERFACE:

#define CONFIG_KERNEL_VERSION_STRING \
  GREETING_COLOR_ANSI_TITLE "Welcome to L4/Fiasco.OC!\\n"                      \
  GREETING_COLOR_ANSI_INFO "L4/Fiasco.OC microkernel on " CONFIG_XARCH "\\n"      \
                           "Rev: " CODE_VERSION " compiled with " COMPILER \
                           TARGET_NAME_PHRASE "    [" CONFIG_LABEL "]\\n"    \
                           "Build: #" BUILD_NR " " BUILD_DATE "\\n"            \
  GREETING_COLOR_ANSI_OFF

//---------------------------------------------------------------------------
INTERFACE [ux]:

EXTENSION class Config
{
public:
  // 8 percent of total RAM, >=800MB RAM => 64MB kmem
  static const unsigned kernel_mem_per_cent = 8;
  enum
  {
    kernel_mem_max      = 64 << 20
  };
};

//---------------------------------------------------------------------------
INTERFACE [!ux && !64bit]:

EXTENSION class Config
{
public:
  // 8 percent of total RAM, >=750MB RAM => 60MB kmem
  static const unsigned kernel_mem_per_cent = 8;
  enum
  {
    kernel_mem_max      = 60 << 20
  };
};

//---------------------------------------------------------------------------
INTERFACE [!ux && 64bit]:

EXTENSION class Config
{
public:
  // 6 percent of total RAM, >=55466MB RAM => 3328MB kmem
  static const unsigned kernel_mem_per_cent = 6;
  enum
  {
    kernel_mem_max      = 3328UL << 20
  };
};

//---------------------------------------------------------------------------
INTERFACE [serial]:

EXTENSION class Config
{
public:
  static int  serial_esc;
};

//---------------------------------------------------------------------------
INTERFACE [!serial]:

EXTENSION class Config
{
public:
  static const int serial_esc = 0;
};


//---------------------------------------------------------------------------
INTERFACE [!virtual_space_iface]:

#define FIASCO_SPACE_VIRTUAL

//---------------------------------------------------------------------------
INTERFACE [virtual_space_iface]:

#define FIASCO_SPACE_VIRTUAL virtual

//---------------------------------------------------------------------------
INTERFACE [!virt_obj_space || ux]:

#define FIASCO_VIRT_OBJ_SPACE_OVERRIDE

//---------------------------------------------------------------------------
INTERFACE [virt_obj_space && !ux]:

#define FIASCO_VIRT_OBJ_SPACE_OVERRIDE override


//---------------------------------------------------------------------------
IMPLEMENTATION:

#include <cstring>
#include <cstdlib>
#include "feature.h"
#include "initcalls.h"
#include "koptions.h"
#include "panic.h"
#include "std_macros.h"

KIP_KERNEL_ABI_VERSION(FIASCO_STRINGIFY(FIASCO_KERNEL_SUBVERSION));

// class variables
bool Config::esc_hack = false;
#ifdef CONFIG_SERIAL
int  Config::serial_esc = Config::SERIAL_NO_ESC;
#endif

unsigned Config::tbuf_entries = 0x20000 / sizeof(Mword); //1024;
bool Config::getchar_does_hlt_works_ok = false;

#ifdef CONFIG_FINE_GRAINED_CPUTIME
KIP_KERNEL_FEATURE("fi_gr_cputime");
#endif

//-----------------------------------------------------------------------------
IMPLEMENTATION:

#include <stdio.h>

IMPLEMENT FIASCO_INIT
void Config::init()
{
  init_arch();

  if (Koptions::o()->opt(Koptions::F_esc))
    esc_hack = true;

#ifdef CONFIG_SERIAL
  if (    Koptions::o()->opt(Koptions::F_serial_esc)
      && !Koptions::o()->opt(Koptions::F_noserial)
      && !Koptions::o()->opt(Koptions::F_nojdb))
    {
      serial_esc = SERIAL_ESC_IRQ;
    }
#endif
}
