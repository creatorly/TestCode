## Process this file with automake to produce Makefile.in

EXTRA_DIST = \
    CONTRIBUTORS \
    LICENSE \
    BUILDSTAMP \
    SVN-VERSION \
    acsite.m4

if HAVE_JSON
JSON_DIR=json-c-0.13.1
endif

if HAVE_UBOX
UBOX_DIR=libubox-2016-11-29
endif

if HAVE_TIME
TIME_DIR=time
endif

if HAVE_CPELOG
CPELOG_DIR=cpelog
endif

if HAVE_LUA
LUA_DIR=lua-5.1.4
endif

if HAVE_UBUS
UBUS_DIR=ubus-2016-10-12
endif

if HAVE_UHTTPD
UHTTPD_DIR=uhttpd-2016-10-06
endif

if HAVE_CPESERVER
CPESERVER_DIR=cpeserver
endif

SUBDIRS = $(JSON_DIR) $(UBOX_DIR) $(TIME_DIR) $(CPELOG_DIR) $(LUA_DIR) $(UBUS_DIR) $(UHTTPD_DIR) $(CPESERVER_DIR)  

