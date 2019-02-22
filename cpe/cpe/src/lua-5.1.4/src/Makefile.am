#ACLOCAL_AMFLAGS = -I autoconf-archive/m4

##############################################################################
lib_LTLIBRARIES = liblua.la

libluaincludedir = $(includedir)/lua

libluainclude_HEADERS = \
	lapi.h \
     	lcode.h \
  	ldo.h \
   	lgc.h \
	llimits.h  \
	lobject.h \
	lparser.h \
	lstring.h \
	ltm.h \
	lua.h \
	lundump.h \
	lzio.h \
	lauxlib.h \
	ldebug.h \
	lfunc.h \
	llex.h \
	lmem.h \
	lopcodes.h \
	lstate.h \
	ltable.h \
	luaconf.h \
	lualib.h \
	lvm.h

liblua_la_LDFLAGS = -version-info 4:0:0 -no-undefined -Wl,-Bsymbolic-functions

liblua_la_SOURCES = \
	lapi.c \
	ldblib.c \
	lfunc.c \
	llex.c \
	lobject.c \
	lstate.c \
	ltablib.c \
	lundump.c \
	lauxlib.c \
	ldebug.c \
	lgc.c \
	lmathlib.c \
	lopcodes.c \
	lstring.c \
	ltm.c \
	lvm.c \
	lbaselib.c \
	ldo.c \
	linit.c \
	lmem.c \
	loslib.c \
	lstrlib.c \
	lzio.c \
	lcode.c \
	ldump.c \
	liolib.c \
	loadlib.c \
	lparser.c \
	ltable.c \
	print.c

