## Process this file with automake to produce Makefile.in

EXTRA_DIST = \
    CONTRIBUTORS \
    LICENSE \
    BUILDSTAMP \
    SVN-VERSION \
    acsite.m4

if HAVE_UTILS
UTILS_DIR=utils
endif

if HAVE_WIRELESS
WIRELESS_DIR=wireless
endif

if HAVE_SYS
SYS_DIR=sys
endif

SUBDIRS = $(UTILS_DIR) $(WIRELESS_DIR) $(SYS_DIR)  
