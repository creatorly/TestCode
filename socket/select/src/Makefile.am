## Process this file with automake to produce Makefile.in

EXTRA_DIST = \
    CONTRIBUTORS \
    LICENSE \
    BUILDSTAMP \
    SVN-VERSION \
    acsite.m4

if HAVE_SYS
SYS_DIR=sys
endif

if HAVE_SOCKET
SOCKET_DIR=socket
endif

SUBDIRS = $(SYS_DIR) $(SOCKET_DIR)  
