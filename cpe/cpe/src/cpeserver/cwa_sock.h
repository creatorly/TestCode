#ifndef __CWA_SOCK_H__
#define __CWA_SOCK_H__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <cpeLog.h>

#define ANDROID_SOCK_IP		"192.168.48.129"
#define ANDROID_SOCK_PORT 	10040
#define BUF_MAXSIZE 		1024
 
int cwaSock_SendData(int send_port, char * data, int len, char *resultData, int retBufSize);

/* __CWA_SOCK_H__ */ 
#endif
