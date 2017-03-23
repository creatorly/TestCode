#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

#define UDP_PORT 8741
#define UDP_IPADDR 0x7F000001 

int main()
{
	char buf[512] = "Hello";
	int sockfd;
	int Timer;
	fd_set readable;
	struct timeval timo;	
	struct sockaddr_in serveraddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(UDP_PORT);
	serveraddr.sin_addr.s_addr = htonl(UDP_IPADDR);

	connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    while (1) {
		timo.tv_sec = 1;
		timo.tv_usec = 0;
		FD_ZERO(&readable);
		FD_SET(sockfd, &readable);
		Timer = select(sockfd + 1, &readable, NULL, NULL, &timo);

		if (Timer == 0)/* Timeout */
		{
	    	sendto(sockfd, buf, sizeof(buf), 0, NULL, 0);
        	printf("send:%s\n", buf);
		}
    }
	close(sockfd);
	return 0;
}
