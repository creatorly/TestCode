#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

#define SERVER_PORT    8745
#define SERVER_IPADDR  0x7F000001 

void *thread_fun(void *arg)
{
	int size, j;
	char recv_buff[1024];
	int *parg = (int *)arg;
	int new_fd = *parg;

	printf("new_fd = %d\n", new_fd);
	while((size = read(new_fd, recv_buff, 1024)) > 0)
	{
		if(recv_buff[0] == '@')
		break;

		printf("Message from client(%d): %s\n", size, recv_buff);
		
		for(j = 0; j < size; j++)
		{
			recv_buff[j] = toupper(recv_buff[j]);
		}
		write(new_fd, recv_buff, size);
		
		memset(recv_buff, 0, sizeof(recv_buff));
	}
	close(new_fd);
	return NULL;
}

int main()
{
	char buf[256];
	int listenfd,connfd;
	int nbytes,size;
	pthread_t tid;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;

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
    	close(listenfd);
        return 0;
    }

	if(listen(listenfd,10) == -1){
        printf("listen socket error\n");
    	close(listenfd);
        return 0;
    }
    while (1) {
		size = sizeof(struct sockaddr_in);
		connfd = accept(listenfd,(struct sockaddr*)(&clientaddr),&size);
		if(connfd < 0){	
			if(errno == EINTR){
				continue;
			}
			else{
        		printf("accept socket error\n");
    			close(listenfd);
        		return 0;
			}
    	}
		printf("connfd = %d\n",connfd);
		if((pthread_create(&tid,NULL,thread_fun,&connfd)) == -1){
			printf("pthread create error\n");  
    		close(listenfd);
    		close(connfd);
        	return 0;
		}
    }
	return 1;
}
