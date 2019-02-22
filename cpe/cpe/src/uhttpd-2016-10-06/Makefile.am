
AM_CFLAGS = -Wall -Wuninitialized -Wwrite-strings

INCLUDES = -I$(includedir) -I$(includedir)/lua -I$(includedir)/ubus -I$(includedir)/ubox -I$(includedir)/time -I$(includedir)/cpelog

# Makefile for the erm executable file
bin_PROGRAMS = uhttpd

#############################################################
uhttpd_SOURCES = \
	main.c \
	listen.c \
	client.c \
	utils.c \
	file.c \
	auth.c \
	cgi.c \
	relay.c \
	proc.c \
	plugin.c \
	handler.c

uhttpd_LDFLAGS = -static -L$(libdir)
uhttpd_LDADD = \
	-llua -lubus -lubox -ljson-c -ltime -lcpelog \
	-lm -ldl -lz -lpthread
	
