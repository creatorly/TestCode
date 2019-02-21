
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#ifndef ANDROID
#include <linux/serial.h>
#endif
#include "eos.h" 

#include "wireless_debug.h"
#include "wireless_vifi_uart.h"
#include "wireless_interface_inc.h" 

#if (ACOM_BOARD_SUBTYPE == MT_U3)
#define WRL_VIFI_UART_LSIM  "/dev/ttyMT2"
#define WRL_VIFI_UART_VSIM  "/dev/ttyMT3"
#elif (ACOM_BOARD_SUBTYPE == MT_U4)
#define WRL_VIFI_UART_LSIM  "/dev/ttyMT2"
#define WRL_VIFI_UART_VSIM  "/dev/ttyMT3"
#elif (ACOM_BOARD_TYPE == MDM9X07)
#define WRL_VIFI_UART_LSIM  "/dev/ttyHSL1"
#elif (ACOM_BOARD_TYPE == MSM89XX)
#define WRL_VIFI_UART_LSIM  "/dev/ttyUSB2"
#define WRL_VIFI_UART_VSIM  "/dev/ttyMSM1"
#elif (ACOM_HW_SUBTYPE == HWST_2G4G)
#define WRL_VIFI_UART_LSIM  "/dev/ttyS0"    //3G
#define WRL_VIFI_UART_VSIM  "/dev/ttyS1"    //virtual sim card.
#else
#define WRL_VIFI_UART_TTYS0 "/dev/ttyS0"    //3G
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (ACOM_BOARD_TYPE == MT673X)
#include <sys/socket.h>
#include <sys/un.h>

struct  sockaddr_un address;
static _INT32 iUartSockfd = -1;
#endif

static _INT32 wrluartInit(_INT32 fd, _INT32 speed, _INT32 flow_ctrl, _INT32 databits, _INT32 stopbits, _INT32 parity);
static _INT32 wrluartOpen(_CHAR8* ttySn);
static _INT32 wrluartSetBaud(_INT32 fd, _INT32 baud);
static _VOID wrluartClose(_INT32 fd);

//search the device file to check the device is on
_INT32 WrUart_getAcmDevName(unsigned char *deviceName, ENUM_TTY_DEV acmDev)
{
    DIR *pDir;
    struct dirent *dirInfo[5];
    unsigned int acmNumber = 0;
    unsigned int acmNumberTemp =0 , i =0, j=0;
    unsigned char devName[5][10] = {0};
    char devNum = 0;

    if((pDir = opendir("/dev")) == NULL)
    {
        WIRELESS_ERROR(0, "Open /dev faild.ERROR\n");
        return EOS_FALSE;
    }
    while((dirInfo[acmNumber] = readdir(pDir)) != NULL)
    {
        if(strncmp(dirInfo[acmNumber]->d_name, "ttyUSB", 6) == 0)
        {
            strncpy(devName[acmNumber], dirInfo[acmNumber]->d_name, 7);
            devName[acmNumber][7] = 0;
            //printf("TEST2-%d-read:%s,get:%s;\n", acmNumber, dirInfo[acmNumber]->d_name, devName[acmNumber]);
            if(acmNumber < 4)   //need to get three usb ports
            {
                acmNumber++;
            }
            else if(acmNumber == 4) //Is the device name we want
            {
                for(i = 0; i < 4; i++)
                {
                    for(j = i+1; j <= 4; j++)
                    {
                        if(devName[i][6] > devName[j][6])
                        {
                            devNum = devName[i][6];
                            devName[i][6] = devName[j][6];
                            devName[j][6] = devNum;
                        }
                    }
                }
                strcpy(deviceName, "/dev/");
                strcpy(deviceName + strlen("/dev/"), devName[acmDev]);
                //printf("TEST--:%s;%s;%s;%s;%s.\n", devName[0],devName[1],devName[2],devName[3],devName[4]);
                closedir(pDir);
                return EOS_TRUE;
            }
        }
    }
    closedir(pDir);
    return EOS_FALSE;
}

_INT32 WrlUart_Init(_INT32 ChannelId)
{
    _INT32 fd_uart = EOS_ERROR;
    _INT32 err = EOS_ERROR;
    _INT32 err1 = EOS_ERROR;
    _INT32 i = 0; 
    unsigned char acmDeviceName[30] = {0};

#ifdef ANDROID
    if(ChannelId > 0)
    {
        WIRELESS_INFO("Channel %d Uart Init Error\n", ChannelId);
        return -1;
    }
#elif (ACOM_BOARD_TYPE == MDM9X07)
    if(ChannelId > 0)
    {
        WIRELESS_INFO("Channel %d Uart Init Error\n", ChannelId);
        return -1;
    }
#endif
    for(i = 0; (i < 3)&&(err == EOS_ERROR); i++)
    {
        if(0 == ChannelId){
            fd_uart = wrluartOpen(WRL_VIFI_UART_LSIM);
        }
        else if(1 == ChannelId){
            if(WrUart_getAcmDevName(acmDeviceName, ENUM_TTY_2)){
                fd_uart = wrluartOpen(acmDeviceName);
            }
        }else{
            if(WrUart_getAcmDevName(acmDeviceName, ENUM_TTY_3)){
                fd_uart = wrluartOpen(acmDeviceName);
            }
        }
        if(fd_uart == EOS_ERROR)
        {
            continue;
        }
        err1 = wrluartInit(fd_uart,115200,0,8,1,'N');
        if(err1 == EOS_ERROR)
        {
            close(fd_uart);
            continue;
        }

        err = EOS_OK;
    }
    
    if(err == EOS_ERROR)
    {
        WIRELESS_ERROR(ChannelId, "Channel %d Uart Set Error!.\n", ChannelId);
        return EOS_ERROR;
    }
    
    WIRELESS_INFO("Channel %d Uart Set OK!(Handler=%d; Device=%s).\n", ChannelId, fd_uart, acmDeviceName);
    return fd_uart;
}

#if ((ACOM_BOARD_TYPE == MT673X) || (ACOM_BOARD_TYPE == RALINK) || (ACOM_BOARD_TYPE == MSM89XX))
_INT32 Wrl_UartVsimInit(_INT32 baud)
{
    _INT32 fd_uart = EOS_ERROR;
    _INT32 i = 0; 

#ifdef VAD_VSIM_UARTBAUD_ENABLE
    if(-1 == iUartSockfd)
    {
        WIRELESS_INFO("start uart client");

        // create a socket
        iUartSockfd = socket(AF_UNIX, SOCK_STREAM, 0);

        address.sun_family = AF_UNIX;
        strcpy(address.sun_path, "/data/data/net.eoutech.uuwifi.ufi/uart_socket");
        
        // connect to the server
        int res = connect(iUartSockfd, (struct sockaddr *)&address, sizeof(address));
        if(res == -1)
        {
            WIRELESS_INFO("connect failed");
            iUartSockfd = -1;
        }
        else
        {
            WIRELESS_INFO("connect ok fd(%d)",iUartSockfd);
        }
    }
    else
    {
        WIRELESS_INFO("already connect ok fd(%d)",iUartSockfd);
    }
#endif
    
    wrluartSetBaud(fd_uart, baud);

    fd_uart = wrluartOpen(WRL_VIFI_UART_VSIM);
    wrluartInit(fd_uart, baud, 0, 8, 2, 'E');
    if(fd_uart == EOS_ERROR)
    {
        return EOS_ERROR;
    }
    WIRELESS_INFO("Vsim uart init ok.\n");
    return fd_uart;
}
#endif

_INT32 WrlUart_unInit(_INT32 ChannelId, _INT32 fd)
{
    wrluartClose(fd);
    WIRELESS_INFO("Channel %d Uart uninit OK!(Handler=%d).\n", ChannelId, fd);
}

/*send data from uart
 *fd:the device file describe
 *length:the length of "p_data"
 */
_INT32 WrlUart_Write(_INT32 fd, _UCHAR8* p_data, _UINT32 length)
{
    int len = 0;
    static int channel0fd = -1;

    if((fd == EOS_ERROR) || (fd == 0))
    {
        WIRELESS_ERROR(0, "WrlUart_Write Err:(fd=%d),p_data:%s\r\n", fd, p_data); 
        printf("WrlUart_Write Err:(fd=%d),p_data:%s\r\n", fd, p_data); 
        return 0;
    }

    if(channel0fd = -1)
        channel0fd = fd;
    
    len = write(fd, p_data, length);
    if (len == length)
    {
        WIRELESS_INFO("WrlUart_Write:(fd=%d)(%d)(%s)\r\n", fd, len, p_data);
        return EOS_OK;
    }
    else
    {
#ifndef ANDROID    
        extern int errno;
        if(errno != 0){
            WIRELESS_ERROR(0, "WrlUart_Write Err:(fd=%d)(%d!=%d)(%s)\n", fd, len, length, strerror(errno)); 
            printf("WrlUart_Write Err:(fd=%d)(%d!=%d)(%s)\r\n", fd, len, length, strerror(errno)); 
        }
#endif
        tcflush(fd,TCOFLUSH);
        return EOS_ERROR;
    }
}

_INT32 WrlUart_Flush(_INT32 fd)
{
    ioctl(fd, TCFLSH);

    return EOS_OK;
}

/*read the data from uart
 *p_data  :the point variable for saving the data
 *length  :the size of the p_data.
 */
_INT32 WrlUart_Read(_INT32 fd, _UCHAR8* p_data)
{
    _INT32 length = 0;

    if(fd == EOS_ERROR)
    {
        return 0;
    }
    ioctl(fd, FIONREAD, &length);
    if(length > 0)
    {
        length = read(fd, p_data, 255);
        //WIRELESS_INFO("WrlUart_Read:(fd=%d)(%d)\n", fd, length);
    }
    
    return length;
}

/*********************Local Function***********************************************/
static _INT32 wrluartOpen(_CHAR8* ttySn)
{
    _INT32 fd = -1;
    
    fd = open( ttySn, O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd < 0)
    {
        WIRELESS_ERROR(0, "Cann't Open %s\r\n", ttySn);
        return EOS_ERROR;
    }

     //fcntl(fd, F_SETFL, FNDELAY);
    fcntl(fd, F_SETFL, 0); 
    
    if(0 == isatty(STDIN_FILENO))
    {
        WIRELESS_ERROR(0, "Standard input is not a terminal device.\n");
        //return EOS_ERROR;
    }
    return fd;
}

static _VOID wrluartClose(_INT32 fd)
{
    close(fd);
}

static _INT32 wrluartInit(_INT32 fd, _INT32 speed, _INT32 flow_ctrl, _INT32 databits, _INT32 stopbits, _INT32 parity)
{
    _INT32   i,status;
    _INT32   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
    _INT32   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};
         
    struct termios options;
   
    /*tcgetattr(fd,&options),this function gets the paramer of the uart,and saves int the options,
    if it runs successfully,returns 0,or else returns 1*/
    if(tcgetattr(fd, &options)  !=  0)
    {
        //printf("wrluartInit: Getting the paramer of the uart.Error!\n");    
        return EOS_ERROR; 
    }
    //set the baud rate
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++){
        if  (speed == name_arr[i]){             
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
            //fprintf(stderr,"wrluartInit: Unsupported data size.\n");
            return EOS_ERROR; 
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
            //fprintf(stderr,"wrluartInit: Unsupported parity.\n");    
            return EOS_ERROR; 
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
            return EOS_ERROR;
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
        //perror("wrluartInit: com set error!");  
        return EOS_ERROR; 
    }

    WIRELESS_INFO("(fd=%d) Init uart ok!\n", fd);
    
    return EOS_OK;; 
}

/*set the special baud rate.*/
static _INT32 wrluartSetBaud(_INT32 fd, _INT32 baud)
{
#ifdef VAD_VSIM_UARTBAUD_ENABLE
    _CHAR8 sendBuf[32];
    _CHAR8 rcvBuf[32];

    memset(sendBuf,0,sizeof(sendBuf));
    memset(rcvBuf,0,sizeof(rcvBuf));
    sprintf(sendBuf,"%d",baud);
    WIRELESS_INFO("uartsock send %s \r\n",sendBuf);
    write(iUartSockfd, sendBuf, strlen(sendBuf));
    read(iUartSockfd, rcvBuf, sizeof(rcvBuf));
    WIRELESS_INFO("uartsock rcv:%s \r\n", rcvBuf);
#else
#ifndef ANDROID
    _INT32   status;
    struct termios   Opt;
    struct serial_struct Serial;

    tcgetattr(fd, &Opt);        /*Get current options*/
    tcflush(fd, TCIOFLUSH);/*Flush the buffer*/     
    cfsetispeed(&Opt, B38400);/*Set input speed,38400 is necessary? who can tell me why?*/  
    cfsetospeed(&Opt, 38400); /*Set output speed*/
    tcflush(fd,TCIOFLUSH);        /*Flush the buffer*/
    status = tcsetattr(fd, TCSANOW, &Opt);  /*Set the 38400 Options*/
    if  (status != 0)
    {        
            perror("wrluartSetBaud: tcsetattr fd1");  
            return EOS_ERROR;     
    }   
                    
    if((ioctl(fd,TIOCGSERIAL,&Serial))<0)/*Get configurations vim IOCTL*/
    {
            printf("wrluartSetBaud: Fail to get Serial!\n");
            return EOS_ERROR;
    }
    Serial.flags = ASYNC_SPD_CUST;/*We will use custom buad,May be standard,may be not */
    Serial.custom_divisor=Serial.baud_base/baud;/*In Sep4020,baud_base=sysclk/16*/

    if((ioctl(fd,TIOCSSERIAL,&Serial))<0)/*Set it*/
    {
            printf("wrluartSetBaud: Fail to set Serial\n");
            return EOS_ERROR;
    }
    
    ioctl(fd,TIOCGSERIAL,&Serial);/*Get it again,not necessary.*/
    WIRELESS_INFO("(fd=%d)(baud=%d,custom_divisor=%d,baud_base=%d)OK.\n", fd, baud, Serial.custom_divisor, Serial.baud_base);
#endif
#endif
    return EOS_OK;
}

#ifdef __cplusplus
}
#endif

