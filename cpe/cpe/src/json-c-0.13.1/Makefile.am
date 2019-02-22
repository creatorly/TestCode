#ACLOCAL_AMFLAGS = -I autoconf-archive/m4

lib_LTLIBRARIES = libjson-c.la

libjson_cincludedir = $(includedir)/json-c
libjson_cinclude_HEADERS = \
	config.h \
	arraylist.h \
	bits.h \
	debug.h \
	json.h \
	json_c_version.h \
	json_config.h \
	json_inttypes.h \
	json_object.h \
	json_object_iterator.h \
	json_pointer.h \
	json_tokener.h \
	json_util.h \
	json_visit.h \
	linkhash.h \
	printbuf.h

noinst_HEADERS=\
	json_object_private.h \
	math_compat.h \
	strdup_compat.h \
	snprintf_compat.h \
	vasprintf_compat.h \
	random_seed.h \
	strerror_override.h

libjson_c_la_LDFLAGS = -version-info 4:0:0 -no-undefined -Wl,-Bsymbolic-functions

libjson_c_la_SOURCES = \
	arraylist.c \
	debug.c \
	json_c_version.c \
	json_object.c \
	json_object_iterator.c \
	json_pointer.c \
	json_tokener.c \
	json_util.c \
	json_visit.c \
	linkhash.c \
	printbuf.c \
	random_seed.c \
	strerror_override.c \
	strerror_override_private.h
