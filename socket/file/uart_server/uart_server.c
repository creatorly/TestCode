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
#include <linux/serial.h>
#include <linux/input.h>
#include <sys/un.h>

#define WRL_VIFI_UART_VSIM  "/dev/ttyMT3"    //virtual sim card.

#ifdef __cplusplus
extern "C" {
#endif

static int wrluartSetBaud(int baud);
static int wrluartOpen(unsigned char* ttySn);
static int wrluartInit(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
static int wiluartSetSpeciBaud(int fd, int baud);

int main(int argc, char *argv[])
{
    int baud = 0;
    int childpid = -1;
    char rcvbuf[32];
    int server_sockfd;
    int client_sockfd;
    socklen_t client_len;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    /* delete the socket file */
    unlink("/data/data/net.eoutech.uuwifi.ufi/uart_socket"); 
    //unlink("/home/ly/uart_socket"); 

    /* create a socket */
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "/data/data/net.eoutech.uuwifi.ufi/uart_socket");
    //strcpy(server_addr.sun_path, "/home/ly/uart_socket");

    /* bind with the local file */
    bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    /* listen */
    listen(server_sockfd, 5);

    while(1)
    {  
        printf("server waiting:\n");        
        client_len = sizeof(client_addr);
        /* accept a connection */  
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);    
		
        while(1)
        //if((childpid = fork()) == 0)
        {
            //close(server_sockfd);  
            /* exchange data */  
			memset(rcvbuf, 0, sizeof(rcvbuf));
            read(client_sockfd, rcvbuf, sizeof(rcvbuf));  
            baud = atoi(rcvbuf);
            wrluartSetBaud(baud);
            write(client_sockfd, rcvbuf, strlen(rcvbuf));  
            printf("get char from client:%s,baud:%d\n", rcvbuf, baud);  

            //exit(0);							        
        }
        /* close the socket */  
        close(client_sockfd);  
    }
	
    return 0;
}

/*********************Local Function***********************************************/

static int wrluartSetBaud(int baud)
{
    int fd_uart = -1;
    int err = -1;

    fd_uart = wrluartOpen((unsigned char*)WRL_VIFI_UART_VSIM);

    if(fd_uart == -1)
    {
        printf("Channel %d Uart Open Error!\r\n", 0);
    }

    /*err = wrluartInit(fd_uart, 115200, 0, 8, 2, 'E');
    if(err == -1)
    {
        close(fd_uart);
        printf("Channel %d Uart Init Error!\r\n", 0);
    }*/

    wiluartSetSpeciBaud(fd_uart, baud/*12860*//*10309*//*10316*//*8736*//*50778*/);
        
    close(fd_uart);

	return 0;
}

static int wrluartOpen(unsigned char* ttySn)
{
    int fd = -1;
    
    fd = open((char *)ttySn, O_RDWR|O_NOCTTY|O_NDELAY);

    if(fd < 0)
    {
        printf( "Cann't Open %s\r\n", ttySn);
        return -1;
    }

    //fcntl(fd, F_SETFL, FNDELAY);  
    fcntl(fd, F_SETFL, 0);    
    
    if(0 == isatty(STDIN_FILENO))
    {
        printf("Standard input is not a terminal device.\r\n");
        //return EOS_ERROR;
    }
    return fd;
}

static int wrluartInit(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity)
{
    int   i,status;
    int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};
         
    struct termios options;
   
    /*tcgetattr(fd,&options),this function gets the paramer of the uart,and saves int the options,
    if it runs successfully,returns 0,or else returns 1*/
    if(tcgetattr(fd, &options)  !=  0)
    {
        printf("wrluartInit: Getting the paramer of the uart.Error!\n");    
        return -1; 
    }
    //set the baud rate
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++){
        if  (speed == name_arr[i])
        {             
            cfsetispeed(&options, speed_arr[i]); 
            cfsetospeed(&options, speed_arr[i]);  
        }
    }     
   
    //modify the mode.set the serial port free.
    options.c_cflag |= CLOCAL;
    //modify the mode, open the function for reading.
    options.c_cflag |= CREAD;
    
    //set data stream mode
    switch(flow_ctrl)
    {
        case 0 ://none
              options.c_cflag &= ~CRTSCTS;
              break;   
        case 1 ://hardware data stream mode
              options.c_cflag |= CRTSCTS;
              break;
        case 2 ://software data stream mode
              options.c_cflag |= IXON | IXOFF | IXANY;
              break;
    }
    //set the bit of the data.
    options.c_cflag &= ~CSIZE;
    switch (databits)
    {  
        case 5    :
            options.c_cflag |= CS5;
            break;
        case 6    :
            options.c_cflag |= CS6;
            break;
        case 7    :    
            options.c_cflag |= CS7;
            break;
        case 8:    
            options.c_cflag |= CS8;
            break;  
        default:   
            printf("wrluartInit: Unsupported data size.\r\n");
            return -1; 
    }
    //set the verify bit.
    switch (parity)
    {  
        case 'n':
        case 'N': //none
            options.c_cflag &= ~PARENB; 
            options.c_iflag &= ~INPCK;    
            break; 
        case 'o':  
        case 'O'://odd parity   
            options.c_cflag |= (PARODD | PARENB); 
            options.c_iflag |= INPCK;             
            break; 
        case 'e': 
        case 'E'://even parity
            options.c_cflag |= PARENB;       
            options.c_cflag &= ~PARODD;       
            options.c_iflag |= INPCK;      
            break;
        case 's':
        case 'S': //set the space
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break; 
        default:  
            printf("wrluartInit: Unsupported parity.\n");    
            return -1; 
    } 
    //set stop bit
    switch (stopbits)
    {  
        case 1:   
            options.c_cflag &= ~CSTOPB;
            break; 
        case 2:   
            options.c_cflag |= CSTOPB;
            break;
        default:   
            //fprintf(stderr,"wrluartInit: Unsupported stop bits.\n"); 
            return -1;
    }
   
    //modify the mode to output. original data.
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //options.c_lflag &= ~(ISIG | ICANON);    
    //other char for control.
    options.c_iflag &= ~(IXON | IXOFF | IXANY);    //
    options.c_iflag &= ~(INLCR | ICRNL | IGNCR);
    options.c_oflag &= ~(ONLCR | OCRNL);
   
    //set the waiting time and the least size for reading and writing.
    options.c_cc[VTIME] = 1; /* wait 1*(1/100)s */  
    options.c_cc[VMIN] = 1;
   
    //if the data is overflowing,clean the old data.
    tcflush(fd, TCIFLUSH);
   
    //activate the option. 
    if (tcsetattr(fd,TCSANOW,&options) != 0){
        printf("wrluartInit: com set error!, (tcsetattr(fd,TCSANOW,&options)):%s\r\n", strerror(errno));  
        //return -1; 
    }

    //printf("(fd=%d) Init uart ok!\r\n", fd);
    
    return 0;; 
}

static int wiluartSetSpeciBaud(int fd, int baud)
{
    int   status;
    struct termios   Opt;
    struct serial_struct Serial;

    tcgetattr(fd, &Opt);        /*Get current options*/
    tcflush(fd, TCIOFLUSH);/*Flush the buffer*/     
    cfsetispeed(&Opt, B38400);/*Set input speed,38400 is necessary? who can tell me why?*/  
    //cfsetospeed(&Opt, 38400); /*Set output speed*/
    cfsetospeed(&Opt, B38400); /*Set output speed*/
    tcflush(fd,TCIOFLUSH);        /*Flush the buffer*/
    status = tcsetattr(fd, TCSANOW, &Opt);  /*Set the 38400 Options*/
    if  (status != 0)
    {        
            perror("wiluartSetSpeciBaud: tcsetattr fd1\r\n");  
            //return -1;     
    }   
                    
    if((ioctl(fd,TIOCGSERIAL,&Serial))<0)/*Get configurations vim IOCTL*/
    {
            printf("wiluartSetSpeciBaud: Fail to get Serial!\r\n");
            return -1;
    }
    Serial.flags = ASYNC_SPD_CUST;/*We will use custom buad,May be standard,may be not */
    Serial.custom_divisor=Serial.baud_base/baud;/*In Sep4020,baud_base=sysclk/16*/

    if((ioctl(fd,TIOCSSERIAL,&Serial))<0)/*Set it*/
    {
            printf("wiluartSetSpeciBaud: Fail to set Serial\r\n");
            return -1;
    }
    
    ioctl(fd,TIOCGSERIAL,&Serial);/*Get it again,not necessary.*/
    printf("(fd=%d)(baud=%d,custom_divisor=%d,baud_base=%d)OK.\r\n", fd, baud, Serial.custom_divisor, Serial.baud_base);
    tcflush(fd,TCIOFLUSH);
    return 0;
}

#ifdef __cplusplus
}
#endif

