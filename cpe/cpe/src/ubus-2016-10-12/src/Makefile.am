#ACLOCAL_AMFLAGS = -I autoconf-archive/m4

#UBUS_UNIX_SOCKET="/etc/cpe/ubus.sock"
#UBUS_MAX_MSGLEN=1048576

INCLUDES = -I$(includedir) -I$(includedir)/ubox -I$(includedir)/time
##############################################################################
lib_LTLIBRARIES = libubus.la

libubusincludedir = $(includedir)/ubus

libubusinclude_HEADERS = \
	ubusmsg.h \
	ubus_common.h \
	libubus.h

libubus_la_LDFLAGS = -version-info 4:0:0 -no-undefined -Wl,-Bsymbolic-functions

#libubus_la_CPPFLAGS = -DUBUS_UNIX_SOCKET=$(UBUS_UNIX_SOCKET) -DUBUS_MAX_MSGLEN=$(UBUS_MAX_MSGLEN)

libubus_la_SOURCES = \
	libubus.c \
	libubus-io.c \
	libubus-obj.c \
	libubus-sub.c \
	libubus-req.c \
	libubus-acl.c 

