#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(libeos, 1.0, xp-dev@ejointech.com)
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(foreign)
CHECK_AUTOCONF

CXXFLAGS="$CXXFLAGS -Wall"
CFLAGS="$CFLAGS -Wall"

AM_CONFIG_HEADER([config.h])
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS(ctype.h)
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(strings.h)
AC_CHECK_HEADERS(stdio.h)
AC_CHECK_HEADERS(stdlib.h)
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(stdarg.h)
AC_CHECK_HEADERS(varargs.h)
AC_CHECK_HEADERS(sys/time.h)
AC_CHECK_HEADERS(assert.h)
AC_CHECK_HEADERS(signal.h)
AC_CHECK_HEADERS(sys/signal.h)
AC_CHECK_HEADERS(malloc.h)
AC_CHECK_HEADERS(alloca.h)
AC_CHECK_HEADERS(sys/select.h)
AC_CHECK_HEADERS(sys/types.h)
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(sys/soundcard.h)
AC_CHECK_HEADERS(arpa/inet.h)
AC_CHECK_HEADERS(arpa/nameser.h)
AC_CHECK_HEADERS(netdb.h)
AC_CHECK_HEADERS(resolv8_compat.h)
AC_CHECK_HEADERS(stdbool.h)
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_HEADERS(semaphore.h)
AC_CHECK_HEADERS(ifaddrs.h)
AC_CHECK_HEADERS(zlib.h)
AC_CHECK_HEADERS(linux/zlib.h)

AC_CHECK_HEADERS([resolv.h], [], [],
[#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UID_T
AC_C_VOLATILE

AC_CHECK_TYPES([struct timeval],,,[
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
])

AC_CHECK_LIB(pthread, pthread_create, have_pthread=1, have_pthread=0)
if test "$have_pthread" = "1"; then
  CFLAGS="$CFLAGS -DHAVE_PTHREAD=1"
fi

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([clock_gettime getcwd gethostbyname gethostname gettimeofday inet_ntoa localtime_r memchr memset mkdir putenv rmdir select setenv socket strchr strdup strerror strrchr strstr strtol strtoul tzset close getifaddrs])

# init Cos FAC
# COSAC_INIT_FLAGS

#COSAC_LIB_BASE
#COSAC_LIB_CALL
#COSAC_LIB_SIP
#COSAC_LIB_MEDIA

AM_CONDITIONAL(USE_BLDNO, test x$enable_buildnumber = xyes)

# define the home directory of open-wrt
AC_ARG_WITH(openwrt, AS_HELP_STRING([--with-openwrt], [prefix for installed open-wrt]),OPEN_WRT_HOME=$withval,OPEN_WRT_HOME=)
AC_SUBST(OPEN_WRT_HOME)

# define the hardware type
AC_ARG_WITH(board-type, AS_HELP_STRING([--with-board-type], [define the board type]), BOARD_TYPE=$withval, BOARD_TYPE=RALINK)
AC_SUBST(BOARD_TYPE)
AM_CONDITIONAL([MDM9X07], [test x$BOARD_TYPE = xMDM9X07])
AM_CONDITIONAL([RALINK], [test x$BOARD_TYPE = xRALINK])

# define the hardware type
AC_ARG_WITH(hw-type, AS_HELP_STRING([--with-hw-type], [define the hardware type]), HW_TYPE=$withval, HW_TYPE=goip)
AC_SUBST(HW_TYPE)
AM_CONDITIONAL([HWGOIP], [test x$HW_TYPE = xHWGOIP])
AM_CONDITIONAL([HWGOIP], [test x$HW_TYPE = xHWUUWIFI])
AM_CONDITIONAL([HWVOIP], [test x$HW_TYPE = xHWVOIP])

# define the OEM name
AC_ARG_WITH(oem-name, AS_HELP_STRING([--with-oem-name], [define the OEM name]), OEM_NAME=$withval, OEM_NAME=ejoin)
AC_SUBST(OEM_NAME)
AM_CONDITIONAL([OEM_EJOIN], [test x$OEM_NAME = xejoin])
AM_CONDITIONAL([OEM_SKYLINE], [test x$OEM_NAME = xskyline])

# enable the ssl or not
AC_ARG_ENABLE([ssl], [AS_HELP_STRING([--enable-ssl],[enable ssl (default is yes)])],,
  [AC_DEFINE([ENABLE_SSL],[1],[Define if need to enable ssl])])
AM_CONDITIONAL(ENABLE_SSL, test "$enable_ssl" == "yes" -o "$enable_ssl" == "")

AC_CONFIG_FILES([
  Makefile
])

AC_OUTPUT

