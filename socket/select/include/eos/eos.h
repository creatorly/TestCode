#ifndef __EOS_EOS_H
#define __EOS_EOS_H

#if defined(__cplusplus)
extern "C" {
#endif


#include "eos_config.h"


#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
#include "vxWorks.h"
#include "string.h"
#include "stdio.h"
#include "semLib.h"
#include "taskLib.h"
#include "netBufLib.h"
#include "netinet/in.h"
#include "netinet/tcp.h"
#include "sockLib.h"
#include "ioLib.h"
#include "times.h"
#include "ctype.h"
/*    20060308    */ 
#include "resolvLib.h"
/* 2013.05.22 pengjian added */
#include "tickLib.h"
#include "sysLib.h"
#include "semLib.h"
#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_LINUX)
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/types.h"
#include "sys/socket.h"
/* young #include "sys/times.h"*/
#include "netinet/in.h"
#include "netinet/tcp.h"
#include "netdb.h"
#include "arpa/inet.h"
#include "sys/ioctl.h"
#include "pthread.h"
#include "errno.h"
#include "signal.h"
#include "unistd.h"
#include "fcntl.h"
#include "time.h"
#include "linux/stddef.h"
#include "sys/select.h"
/* young add 20060825 for file operation    */
#include "dirent.h"
#include "unistd.h"
#include "sys/stat.h"
/* young added    */
#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_WIN32)
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stddef.h"
#include "malloc.h"
#include "winsock2.h"
#include "ws2tcpip.h"    
#include "process.h"
#include "time.h"
#endif



#include "eos_typedef.h"
#include "eos_statistics.h"
#include "eos_sem.h"
#include "eos_mem.h"
#include "eos_task.h"

#include "eos_timer.h"
#include "eos_iodrv.h"
#include "eos_trace.h"
#include "eos_lst.h"

#include "eos_string.h"
#include "eos_buf.h"
#include "eos_address.h"
#include "eos_socket.h"
#include "eos_select.h"
#include "eos_msg.h"
#include "eos_ver.h"
#include "eos_hash.h"
#include "eos_exec.h"
#include "eos_time.h"
#include "eos_tick.h"
#include "eos_signal.h"

/* 20060615    */
#include "eos_strtok.h"
#include "eos_cliva.h"


/* 20060627    */
#include "eos_ctype.h"
#include "eos_libc.h"

/* 20060720    */
#include "eos_rtdbg.h"

/* 20061220    */
#include "eos_log.h"

/*    20070110    */
#include "eos_wdt.h"
#include "eos_pid.h"

/* 20060323    */
#define    EOS_DEBUG_PRINTF            eos_printf("EOS %s %d\r\n",__FILE__,__LINE__);        

_INT32  eos_init();

_VOID   eos_srand(_UINT32 your_seed);

#if defined(__cplusplus)
}
#endif 
#endif


