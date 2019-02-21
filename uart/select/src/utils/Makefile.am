
UT_WARN_FLAGS = -Wall -Wuninitialized -Wwrite-strings
UT_OPT_FLAGS =

AM_CFLAGS = $(UT_WARN_FLAGS) $(UT_OPT_FLAGS)

INCLUDES = \
	-I$(top_srcdir)/config \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/utils

INCLUDES += -I$(includedir) -I$(includedir)/eos -I$(includedir)/eut

lib_LTLIBRARIES = libutils.la

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=0
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

libutils_la_LDFLAGS = -version-info ${version_Current}:${version_Revision}:${version_Age}

libutils_la_SOURCES = \
	ut_charset.c \
	ut_exc.c \
	ut_fselect.c \
	ut_log.c \
	ut_mem.c \
	ut_msgq.c \
	ut_popen.c \
	ut_queue.c \
	ut_rc4cpt.c \
	ut_rtss.c  \
	ut_rtstat.c \
	ut_sbuf.c \
	ut_task.c \
	ut_timer.c \
	utils.c
