#SUBDIRS =

EOS_WARN_FLAGS = -Wall -Wuninitialized -Wwrite-strings
EOS_OPT_FLAGS =

AM_CFLAGS = $(EOS_WARN_FLAGS) $(EOS_OPT_FLAGS)
INCLUDES = -I$(top_srcdir)

INCLUDES += -I$(includedir)

# Makefile for the erm executable file
bin_PROGRAMS = main

#############################################################
main_SOURCES = \
    main.c \
	ut_sem.c

main_LDFLAGS = -static
main_LDADD = -lz -lpthread
	
