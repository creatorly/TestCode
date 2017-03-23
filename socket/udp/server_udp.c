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
	int addrLen;
	int strLen;
	socklen_t size;
    fd_set readable;	
	struct sockaddr_in serveraddr;
	struct sockaddr fromaddr;

	if( (listenfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
          printf("create socket error\n");
		  return 0;
	};

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(UDP_PORT);
	serveraddr.sin_addr.s_addr = htonl(UDP_IPADDR);

	bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));

	size = sizeof(struct sockaddr_in);
	addrLen = sizeof(fromaddr);
	
    while (1) {
		strLen = recvfrom(listenfd, buf, sizeof(buf), 0, (struct sockaddr *) &fromaddr, &size);
		printf("receive:%s\n", buf);
		sendto(listenfd, buf, strLen, 0, &fromaddr, size);
    }
	close(listenfd);
	return 0;
}
