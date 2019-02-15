#SUBDIRS =

EOS_WARN_FLAGS = -Wall -Wuninitialized -Wwrite-strings
EOS_OPT_FLAGS =

AM_CFLAGS = $(EOS_WARN_FLAGS) $(EOS_OPT_FLAGS)
INCLUDES = -I$(top_srcdir)

INCLUDES += -I$(includedir)

# Makefile for the erm executable file
bin_PROGRAMS = send

#############################################################
send_SOURCES = \
        send_p.c \
	ut_popen.c

send_LDFLAGS = -static
send_LDADD = -lz -lpthread
	
