#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>

#define UDP_PORT 8741
#define UDP_IPADDR 0x7F000001 

int main()
{
	char buf[512];
	int sockfd;
	int addrLen;
	int strLen;
	struct sockaddr_in serveraddr;
	struct sockaddr fromaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(UDP_PORT);
	serveraddr.sin_addr.s_addr = htonl(UDP_IPADDR);

	addrLen = sizeof(fromaddr);
    while (1) {
	  	fgets(buf,512,stdin);
		printf("send:%s\n", buf);
		sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		strLen = recvfrom(sockfd, buf, sizeof(buf), 0, &fromaddr, &addrLen);
		buf[strLen] = 0;
		printf("Message form server: %s\n", buf);
    }
	close(sockfd);
	return 0;
}
