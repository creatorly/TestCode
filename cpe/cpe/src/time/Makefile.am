#ACLOCAL_AMFLAGS = -I autoconf-archive/m4

lib_LTLIBRARIES = libtime.la

libtimeincludedir = $(includedir)/time
libtimeinclude_HEADERS = \
	time_log.h

libtime_la_LDFLAGS = -version-info 4:0:0 -no-undefined -Wl,-Bsymbolic-functions

libtime_la_SOURCES = \
	time_log.c
