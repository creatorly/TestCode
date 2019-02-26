
EOS_WARN_FLAGS = -Wall -Wuninitialized -Wwrite-strings
EOS_OPT_FLAGS =

AM_CFLAGS = $(EOS_WARN_FLAGS) $(EOS_OPT_FLAGS)

INCLUDES = \
        -I$(top_srcdir)/config \
        -I$(top_srcdir)/include \
        -I$(top_srcdir)/include/utils

INCLUDES += -I$(includedir) -I$(includedir)/eos -I$(includedir)/eut

lib_LTLIBRARIES = libsys.la

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=0
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

libsys_la_LDFLAGS = -version-info ${version_Current}:${version_Revision}:${version_Age}

libsys_la_SOURCES = asys_init.c
