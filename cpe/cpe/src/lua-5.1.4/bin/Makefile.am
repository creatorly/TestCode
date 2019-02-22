
AM_CFLAGS = -Wall -Wuninitialized -Wwrite-strings

INCLUDES = -I$(includedir) -I$(includedir)/lua

# Makefile for the erm executable file
#bin_PROGRAMS = lua luac
bin_PROGRAMS = lua

#############################################################
lua_SOURCES = \
        lua.c

lua_LDFLAGS = -static -L$(libdir)
lua_LDADD = \
	-llua \
	-lm -ldl -lz -lpthread
	
#############################################################
#luac_SOURCES = \
#        luac.c

#luac_LDFLAGS = -static -L$(libdir)
#luac_LDADD = \
#	-llua \
#	-lm -ldl -lz -lpthread
	
