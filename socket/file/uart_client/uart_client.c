#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_un address;
    char sendbuf[32];
    char rcvbuf[32];

    /* create a socket */
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    address.sun_family = AF_UNIX;
    //strcpy(address.sun_path, "/data/uart_socket");
    strcpy(address.sun_path, "/home/ly/uart_socket");

    /* connect to the server */
    int result = connect(sockfd, (struct sockaddr *)&address, sizeof(address));
    if(result == -1)
    {
        perror("connect failed: ");
        exit(1);
    }
   
    while(1)
    {
        printf("please enter baud\n");
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
        {
            write(sockfd, sendbuf, (strlen(sendbuf) - 1));  
            read(sockfd, rcvbuf, sizeof(rcvbuf));  
            printf("get char from server: %s\n", rcvbuf);  
        } 
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

