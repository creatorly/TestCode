#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>

#define UDP_PORT   8741
#define UDP_IPADDR 0x7F000001 

int main()
{
	char buf[256];
	int listenfd;
	int Timer;
	socklen_t size;
    fd_set readable;	
	struct timeval timo;
	struct sockaddr_in serveraddr;

	if( (listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
          printf("create socket error\n");
		  return 0;
	};

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(UDP_PORT);
	serveraddr.sin_addr.s_addr = htonl(UDP_IPADDR);

	bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));

	size = sizeof(struct sockaddr_in);
	
    while (1) {
        timo.tv_sec = 5;
        timo.tv_usec = 0;
        FD_ZERO(&readable);
        FD_SET(listenfd, &readable);
        Timer = select(listenfd + 1, &readable, NULL, NULL, &timo);
        
        if (Timer == 0)/* Timeout */
            printf("Timeout\n");
        
        else if (Timer < 0) /* On error, catch a signal ... wait again. */
            printf("wait...\n");

        else
        {
            recvfrom(listenfd, buf, sizeof(buf), 0, (struct sockaddr *) &serveraddr.sin_family, &size);
            printf("receive:%s\n", buf);
        }
    }
	close(listenfd);
	return 0;
}
