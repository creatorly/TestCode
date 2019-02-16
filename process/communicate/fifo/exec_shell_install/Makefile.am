## Process this file with automake to produce Makefile.in

EXTRA_DIST = \
    CONTRIBUTORS \
    LICENSE \
    BUILDSTAMP \
    SVN-VERSION \
    acsite.m4

if HAVE_UTILS
  UTILS_DIR=utils
endif

SUBDIRS = $(UTILS_DIR)

EOS_WARN_FLAGS = -Wall -Wuninitialized -Wwrite-strings
EOS_OPT_FLAGS =

AM_CFLAGS = $(EOS_WARN_FLAGS) $(EOS_OPT_FLAGS)
INCLUDES = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/utils

INCLUDES += -I$(includedir)

# Makefile for the erm executable file
bin_PROGRAMS = read

#############################################################
read_SOURCES = \
    read.c

read_LDFLAGS = -static -L$(libdir)
read_LDADD = -lutils -lz -lpthread
	


