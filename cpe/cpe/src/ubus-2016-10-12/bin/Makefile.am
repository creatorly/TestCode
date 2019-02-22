
AM_CFLAGS = -Wall -Wuninitialized -Wwrite-strings

INCLUDES = -I$(includedir) -I$(includedir)/lua -I$(includedir)/ubus -I$(includedir)/ubox -I$(includedir)/time -I$(includedir)/cpelog

# Makefile for the erm executable file
bin_PROGRAMS = ubus ubusd

#############################################################
ubus_SOURCES = \
        cli.c

ubus_LDFLAGS = -static -L$(libdir)
ubus_LDADD = \
	-llua -lubus -lubox -ljson-c -ltime -lcpelog \
	-lm -ldl -lz -lpthread
	
#############################################################
ubusd_SOURCES = \
        ubusd.c \
	ubusd_id.c \
	ubusd_obj.c \
	ubusd_proto.c \
	ubusd_event.c \
	ubusd_acl.c \
	ubusd_monitor.c

ubusd_LDFLAGS = -static -L$(libdir)
ubusd_LDADD = \
	-llua -lubus -lubox -ljson-c -ltime -lcpelog \
	-lm -ldl -lz -lpthread
	
