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
	char recvbuf[256],sendbuf[256];
	int sockfd,n;
	struct sockaddr_in servaddr;
	char buffer[1024] = {0};
	int nCount;

	char filename[100] = {0};
	printf("Input filename to save: ");
	gets(filename);
	FILE *fp = fopen(filename, "wb");
	if(fp == NULL){
		printf("Cannot open file\n");
		return 0;
	}

    memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);
	servaddr.sin_addr.s_addr = htonl(SERVER_IPADDR);
	//serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
       	printf("create socket error\n");
       	return 0;
    };
    
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
      	printf("connet error\n");
       	return 0;
    }

    while( (nCount = recv(sockfd, buffer, sizeof(buffer), 0)) > 0 ){
    	fwrite(buffer, nCount, 1, fp);
	}
	puts("File transfer success!");
	
	fclose(fp);
	close(sockfd);
    return 0;
}
