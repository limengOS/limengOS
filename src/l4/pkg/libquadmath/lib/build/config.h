/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* libm includes cbrtl */
#define HAVE_CBRTL 1

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* libm includes feholdexcept */
#define HAVE_FEHOLDEXCEPT 1

/* Define to 1 if you have the <fenv.h> header file. */
//l4/#define HAVE_FENV_H 1

/* libm includes fesetenv */
//l4/#define HAVE_FESETENV 1

/* libm includes fesetround */
//l4/#define HAVE_FESETROUND 1

/* libm includes fetestexcept */
//l4/#define HAVE_FETESTEXCEPT 1

/* libm includes feupdateenv */
//l4/#define HAVE_FEUPDATEENV 1

/* __attribute__((visibility ("hidden"))) supported */
#define HAVE_HIDDEN_VISIBILITY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if the math.h includes POSIX's signgam. */
#define HAVE_MATH_H_SIGNGAM 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <printf.h> header file. */
#define HAVE_PRINTF_H 1

/* GNU C Library stype printf hooks supported */
//l4/#define HAVE_PRINTF_HOOKS 1

/* libm includes sqrtl */
#define HAVE_SQRTL 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "libquadmath"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "GCC Quad-precision Math Library"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GCC Quad-precision Math Library 0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libquadmath"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* whether i18n number rewriting can be supported */
//l4/#define USE_I18N_NUMBER_H 1

/* whether localeconv should be used */
//l4/#define USE_LOCALECONV 1

/* whether nl_langinfo should be used */
//l4/#define USE_NL_LANGINFO 1

/* whether nl_langinfo should be used for wide char locale info */
//l4/#define USE_NL_LANGINFO_WC 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "0.1"

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */
