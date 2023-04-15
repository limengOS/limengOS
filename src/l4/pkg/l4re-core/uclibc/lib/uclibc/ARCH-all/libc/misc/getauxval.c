#include <errno.h>
#include <libc-symbols.h>

unsigned long int __getauxval(unsigned long int type)
{
  __set_errno(ENOENT);
  return 0;
}

weak_alias(__getauxval, getauxval)
