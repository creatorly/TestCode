#ifndef _ZTEST_H_H_
#define _ZTEST_H_H_

#include <uci.h>
#include <syslog.h>
#include <libubox/ulog.h>
#include <libubox/md5.h>
#include <libubox/list.h>
#include <libubox/usock.h>
#include <libubox/uloop.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/serial.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libubus.h>
#include <fcntl.h> 
#include <termios.h>


extern int debugLevel;

enum{
	MSG_ERROR,
	MSG_WARNING,
	MSG_INFO,
	MSG_DEBUG,
	MSG_MSGDUMP,
	MSG_EXCESSIVE,
};

#define dbg_printf(level, ...)						\
do													\
{													\
	if (debugLevel > level)							\
	{												\
		syslog(LOG_WARNING, __VA_ARGS__);			\
	}												\
}													\
while (0)


#endif /* _ZDETECT_H_H_ */

