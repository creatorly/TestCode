#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>

#define SERVER_PORT    8745
#define SERVER_IPADDR  0x7F000001 


int main()
{
	char buf[256];
	int listenfd,connfd,n;
	struct timeval timo;
	struct sockaddr_in serveraddr;
	char buffer[1024] = {0};
	int nCount;
	char *filename = "/home/linye/test_source/socket/file/send.txt";

	FILE *fp = fopen(filename,"rb");
	if (fp == NULL){
		printf("can`t open file\n");
		return 0;
	}

	if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("create socket error\n");
        return 0;
    };

	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr = htonl(SERVER_IPADDR);
	//serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(listenfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1){
        printf("bink socket error\n");
        return 0;
    }

	if(listen(listenfd,10) == -1){
        printf("listen socket error\n");
        return 0;
    }
	
	if((connfd = accept(listenfd,(struct sockaddr*)NULL,NULL)) == -1){
       	printf("accept socket error\n");
    }
        
    while ( (nCount = fread(buffer, 1, sizeof(buffer), fp)) > 0 ) {
		send(connfd, buffer, nCount, 0);
	}
	
//	shutdown(connfd, SHUT_WR);
//	recv(connfd, buffer, sizeof(buffer), 0);

	fclose(fp);
    close(connfd);
    close(listenfd);
    return 0;
}
