
AM_CFLAGS = -Wall -Wuninitialized -Wwrite-strings

INCLUDES = \
	-I$(includedir) \
	-I$(includedir)/eos \
	-I$(includedir)/eut \
	-I$(includedir)/lua \
	-I$(includedir)/ubus \
	-I$(includedir)/ubox \
	-I$(includedir)/time \
	-I$(includedir)/cpelog

# Makefile for the erm executable file
bin_PROGRAMS = cpeserver

#############################################################
cpeserver_SOURCES = \
        cwa_sock.c  \
	invoke_acl.c  \
	invoke_config.c \
	invoke_server.c

cpeserver_LDFLAGS = -static -L$(libdir)
cpeserver_LDADD = \
	-llua -lubus -lubox -ljson-c -ltime -lcpelog \
	-leos -leut \
	-lm -ldl -lz -lpthread
	
