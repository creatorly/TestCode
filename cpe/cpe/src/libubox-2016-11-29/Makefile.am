#ACLOCAL_AMFLAGS = -I autoconf-archive/m4

INCLUDES = -I$(includedir) -I$(includedir)/json-c

lib_LTLIBRARIES = libubox.la

libuboxincludedir = $(includedir)/libubox
libuboxinclude_HEADERS = \
	avl-cmp.h \
	blob.h \
	blobmsg_json.h \
	kvlist.h \
	md5.h \
	safe_list.h \
	uloop.h \
	ustream.h \
	utils.h \
	avl.h \
	blobmsg.h \
	json_script.h \
	list.h \
	runqueue.h \
	ulog.h \
	usock.h \
	ustream-ssl.h \
	vlist.h

libubox_la_LDFLAGS = -version-info 4:0:0 -no-undefined -Wl,-Bsymbolic-functions

libubox_la_SOURCES = \
	avl.c \
	avl-cmp.c \
	blob.c \
	blobmsg_json.c \
	blobmsg.c \
	json_script.c \
	uloop.c \
	usock.c \
	ustream.c \
	ustream-fd.c \
	vlist.c \
	utils.c \
	safe_list.c \
	runqueue.c \
	md5.c \
	kvlist.c \
	ulog.c \
	base64.c
