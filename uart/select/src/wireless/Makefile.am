
EOS_WARN_FLAGS = -Wall -Wuninitialized -Wwrite-strings
EOS_OPT_FLAGS =

AM_CFLAGS = $(EOS_WARN_FLAGS) $(EOS_OPT_FLAGS)
INCLUDES = \
	-I$(top_srcdir)/config \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/utils \
	-I./atlayer -I./atlayer/wavecom

INCLUDES += -I$(includedir) -I$(includedir)/eos -I$(includedir)/eut

lib_LTLIBRARIES = libwireless.la

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=0
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

libwireless_la_LDFLAGS = -version-info ${version_Current}:${version_Revision}:${version_Age}

libwireless_la_SOURCES = \
	atlayer/sim_manager.c \
	atlayer/msg_manager.c \
	atlayer/call_manager.c \
	atlayer/wm_channel.c \
	atlayer/wireless_dataconvert.c \
	atlayer/line.c \
	atlayer/wavecom/WAVEC_at_parse.c \
	atlayer/wavecom/WAVEC_parse_urc.c \
	atlayer/wavecom/WAVEC_at_pack.c \
	atlayer/wavecom/WAVEC_Interface.c \
	atlayer/dataconn_manager.c \
	atlayer/stk_manager.c \
	atlayer/radio_server.c \
	atlayer/proxy.c \
	atlayer/wireless_list.c \
	atlayer/wireless_string.c \
	atlayer/wm_interface.c \
	atlayer/proxycom.c \
	atlayer/svrcom.c \
	wireless_interface.c \
	wireless_app.c \
	wireless_vifi_uart.c


