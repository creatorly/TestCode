#ACLOCAL_AMFLAGS = -I autoconf-archive/m4

lib_LTLIBRARIES = libcpelog.la

libcpelogincludedir = $(includedir)/cpelog
libcpeloginclude_HEADERS = \
	cpeLog.h

libcpelog_la_LDFLAGS = -version-info 4:0:0 -no-undefined -Wl,-Bsymbolic-functions

libcpelog_la_SOURCES = \
	cpeLog.c
