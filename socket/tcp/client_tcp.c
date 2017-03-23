#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>

#define SERVER_PORT 8745
#define SERVER_IPADDR 0x7F000001 

int main(int argc, char** argv)
{
	char sendbuf[256];
	int sockfd;
	struct sockaddr_in serveraddr;

    memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr = htonl(SERVER_IPADDR);
	//serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1){
		if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        	printf("create socket error\n");
        	return 0;
    	};
    
		if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
        	printf("connet error\n");
        	return 0;
    	}
	
        fgets(sendbuf,256,stdin);
        printf("send:%s\n", sendbuf);
        if(send(sockfd, sendbuf, strlen(sendbuf), 0) < 0){
            printf("send msg error\n");
            return 0;
        }
		close(sockfd);
    }
    return 0;
}
