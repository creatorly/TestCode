#include "ztest.h"
#include "ubus.h"

int debugLevel = MSG_DEBUG;
    

/********************************************************************/
void ubus_event_test(void)
{    
    uloop_init();
    ztest_ubus_init();
    ztest_ubus_event_init();
    uloop_run();
    ztest_ubus_clean();
    uloop_done();
}

/********************************************************************/
void ubus_test(void)
{    
    uloop_init();
    ztest_ubus_init();
    uloop_run();
    ztest_ubus_clean();
    uloop_done();
}

/********************************************************************/
void recv_sock_callback(struct uloop_fd *u, unsigned int events)
{    
    char buf[1024] = {0};
    int connect_fd;
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(struct sockaddr);    

    connect_fd = accept(u->fd, (struct sockaddr *)(&cli_addr), &len);
    if (connect_fd < 0) {
        perror("accept");
        return;
    }
    if (recv(connect_fd, buf, 1024, 0) > 0) {
        printf("recv_buf: %s\n", buf);
    }
    close(connect_fd);
}

int usock_and_uloop_fd_test(void)
{
    int type = USOCK_TCP | USOCK_SERVER  | USOCK_NOCLOEXEC | USOCK_IPV4ONLY;
    const char *host = "127.0.0.1";
    const char *service = "1212";
    int u_fd = usock(type, host, service);    
    if (u_fd < 0) {
        perror("usock");
        return -1;
    }
    
    struct uloop_fd fd_sock = {
        .cb = recv_sock_callback,
        .fd = u_fd,
    };
    uloop_init();
    /*添加uloop_fd*/
    uloop_fd_add(&fd_sock, ULOOP_READ);
    uloop_run();
    uloop_fd_delete(&fd_sock);
    uloop_done();

    return 0;
}

/********************************************************************/
int m_uartFd = -1;

void read_uart_callback(struct uloop_fd *u, unsigned int events)
{
    unsigned char buffer[128] = {0};
    int len = 0;
    
    if (ULOOP_READ) {
        len = read(u->fd, buffer, 1024);
        if (len > 0) 
        {
#if 1
            {
                char PrintBuff[1024];
                int uiPrintLen = 0;

                uiPrintLen = sprintf((PrintBuff + uiPrintLen), "######## Uart Read(%02d)", len);

                for(int ii = 0; ii < len; ii++)
                {
                    uiPrintLen += sprintf((PrintBuff + uiPrintLen),"%02X ", buffer[ii]);
                }

                printf("%s\r\n", PrintBuff);
            }
#endif
        }
    }
}

void write_uart_timeout_callback(struct uloop_timeout *timeout)
{
    //char buffer[] = {0x55,0xAA,0x00,0x01,0x00,0x00,0x00};
    unsigned char buffer[] = {0xFF,0xFF,0xAA,0x02,0x11,0x22,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x02,0x11,0x22,0x40,0x99,0x16};
    int len = 0;
    
    printf("write_uart_timeout_callback\r\n");
    
    len = write(m_uartFd, buffer, 21);
#if 1
    {
        char PrintBuff[1024];
        int uiPrintLen = 0;

        uiPrintLen = sprintf((PrintBuff + uiPrintLen), "######## Uart Write(%02d)", len);

        for(int ii = 0; ii < len; ii++)
        {
            uiPrintLen += sprintf((PrintBuff + uiPrintLen),"%02X ", buffer[ii]);
        }

        printf("%s\r\n", PrintBuff);
    }
#endif

    uloop_timeout_set(timeout, 5000);
}

int UartInit(void)
{
    int baudrate = 115200;
    int databits = 8;
    int stopbits = 1;
    char parity = 'N';
    
    m_uartFd = open("/dev/ttyS2",O_RDWR|O_NOCTTY|O_NDELAY);
    if(m_uartFd <= 0)
    {
        perror("file open error");
        return -1;
    }

    //恢复串口为阻塞状态
    if(fcntl(m_uartFd, F_SETFL, 0) < 0)  
    {
        perror("fcntl failed!");
        return -1;
    }

    struct termios options;
    int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
    int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300}; 
    int   i;
     
    /*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1. 
    */
    if( tcgetattr( m_uartFd,&options)  !=  0)  
    {  
        perror("SetupSerial 1"); 
        return -1;
    }  

    //设置串口输入波特率和输出波特率  
    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
    {  
        if(baudrate == name_arr[i])
        {
            cfsetispeed(&options, speed_arr[i]);   
            cfsetospeed(&options, speed_arr[i]);    
        }  
    }

    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;  
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD; 

    //不使用流控制  
    options.c_cflag &= ~CRTSCTS;

    //特殊字符不做转换
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    //设置数据位  
    //屏蔽其他标志位  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    { 
        case 5:  
            options.c_cflag |= CS5;  
            break;  
        case 6:  
            options.c_cflag |= CS6;  
            break;  
        case 7: 
            options.c_cflag |= CS7;  
            break;  
        case 8: 
            options.c_cflag |= CS8;  
            break;    
        default:
            perror("Unsupported data size");  
            return -1;;   
    }  
    //设置校验位  
    switch (parity)  
    {
        case 'n':  
        case 'N': //无奇偶校验位。  
            options.c_cflag &= ~PARENB;   
            options.c_iflag &= ~INPCK;
            break;   
        case 'o':
        case 'O'://设置为奇校验      
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK; 
            break;   
        case 'e':
        case 'E'://设置为偶校验    
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;  
        case 's':  
        case 'S': //设置为空格   
            options.c_cflag &= ~PARENB;  
            options.c_cflag &= ~CSTOPB;  
            break;   
        default:
            perror("Unsupported parity");
            return -1;
    }
    // 设置停止位   
    switch (stopbits)  
    {
        case 1:
            options.c_cflag &= ~CSTOPB; break;   
        case 2:
            options.c_cflag |= CSTOPB; break;
        default:
            perror("Unsupported stop bits");
        return -1;
    }  
     
    //修改输出模式，原始数据输出  
    options.c_oflag &= ~OPOST;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    //options.c_lflag &= ~(ISIG | ICANON);
     
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  
     
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(m_uartFd,TCIFLUSH);  
     
    //激活配置 (将修改后的termios数据设置到串口中）  
    if (tcsetattr(m_uartFd,TCSANOW,&options) != 0)
    {  
        perror("com set error!\n");    
        return -1;  
    }

    printf("Led Uart Init OK,fd(%d).\r\n",m_uartFd);

    return m_uartFd;
}

void uloop_fd_uart_test(void)
{
    int fd_uart = -1;

    fd_uart = UartInit();
    if(-1 == fd_uart)
    {
        printf("uart init error\r\n");
        return;
    }
    struct uloop_fd fd_uart_test = {
        .cb = read_uart_callback,
        .fd = fd_uart,
    };
    struct uloop_timeout fd_uart_timeout = {
        .cb = write_uart_timeout_callback,
    };
        
    uloop_init();
    /*添加uloop_fd*/
    uloop_fd_add(&fd_uart_test, ULOOP_READ);
    uloop_timeout_set(&fd_uart_timeout, 5000); //5 second
    uloop_run();
    uloop_fd_delete(&fd_uart_test);
    uloop_timeout_cancel(&fd_uart_timeout);
    uloop_done();
}

/********************************************************************/
void timeout_callback(struct uloop_timeout *timeout)
{
    printf("timeout_callback\r\n");
   
    uloop_timeout_set(timeout, 5000);
}

void uloop_timeout_test(void)
{
    struct uloop_timeout fd_timeout = {
        .cb = timeout_callback,
    };
    uloop_init();
    uloop_timeout_set(&fd_timeout, 5000); //5 second
    uloop_run();
    uloop_timeout_cancel(&fd_timeout);
    uloop_done();
}

/********************************************************************/
void read_std_callback(struct uloop_fd *u, unsigned int events)
{
    char buf[1024] = {0};
    if (ULOOP_READ) {
        if ( read(u->fd, buf, 1024) > 0) {
            printf("read_std: %s\n", buf);
        }
    }
}

void uloop_fd_test(void)
{
    struct uloop_fd fd_test = {
        .cb = read_std_callback,
        .fd = STDIN_FILENO,
    };
    uloop_init();
    /*添加uloop_fd*/
    uloop_fd_add(&fd_test, ULOOP_READ);
    uloop_run();
    uloop_fd_delete(&fd_test);
    uloop_done();
}

/********************************************************************/
typedef struct _Kvlist_Data {
    char data;
    struct list_head list;
} Kvlist_Data;

void kvlist_test(void)
{

}

/********************************************************************/
typedef struct _ListData {
    char data;
    struct list_head list;
} ListData;

void list_test(void)
{
    ListData mylist, *tmp = NULL;
    struct list_head *pos = NULL, *p = NULL;
    char ch = '0';
    long dec = 1;
    long dec_number = 0;
    int i = 0;
    INIT_LIST_HEAD(&mylist.list);

    while((ch == '0') || (ch == '1')) {
        tmp = (ListData *)malloc(sizeof(ListData));
        tmp->data = ch;
        list_add(&(tmp->list), &(mylist.list));
        printf("list_add %d\n", tmp->data);
        ch = '1';
        i++;
        if(i == 10)
        {
            ch = '2';
        }
    }

    // 遍历过程不可操作链表
    list_for_each(pos, &mylist.list) {
        tmp = list_entry(pos, ListData,list);
        printf("list_entry %d\n", tmp->data);
        dec_number += (int)(tmp->data - '0') * dec;
        dec *= 2;
    }

    printf("Decimal number is %ld\n", dec_number);

    // 遍历过程可操作链表
    list_for_each_safe(pos, p, &mylist.list) {
        tmp = list_entry(pos, ListData, list);
        printf("list_del %d\n", tmp->data);
        list_del(pos);
        free(tmp);
    }

    if(list_empty(&mylist.list)){
        printf("The list now is empty!\n");
    }

    return;
}

/********************************************************************/
void md5_test(void)
{
    char* data = "test data";
    unsigned char buf[16] = {0};
    md5_ctx_t ctx;

    md5_begin(&ctx);
    md5_hash(data, strlen(data), &ctx);
    md5_end(buf, &ctx);

#if 1
    {
        char PrintBuff[1024];
        int uiPrintLen = 0;

        uiPrintLen = sprintf((PrintBuff + uiPrintLen), "test data md5:");
        for(int ii = 0; ii < 16; ii++)
        {
            uiPrintLen += sprintf((PrintBuff + uiPrintLen),"%02x ", buf[ii]);
        }
        dbg_printf(MSG_INFO, PrintBuff);
    }   
#endif

    memset(buf, 0, sizeof(buf));
    md5sum("/usr/bin/ztest", buf);

#if 1
    {
        char PrintBuff[1024];
        int uiPrintLen = 0;

        uiPrintLen = sprintf((PrintBuff + uiPrintLen), "/usr/bin/ztest md5sum:");
        for(int ii = 0; ii < 16; ii++)
        {
            uiPrintLen += sprintf((PrintBuff + uiPrintLen),"%02x ", buf[ii]);
        }
        dbg_printf(MSG_INFO, PrintBuff);
    }  
#endif
}

void log_init(void)
{
    ulog_open(ULOG_SYSLOG, LOG_USER, NULL);
    ulog_threshold(LOG_INFO);
}

int main(int argc, char **argv){

    log_init();
    
    printf("--------md5_test start--------\n");
    md5_test();
    printf("--------md5_test end--------\n");
    
    printf("--------list_test start--------\n");
    list_test();
    printf("--------list_test end--------\n");

    printf("--------kvlist_test start--------\n");
    kvlist_test();
    printf("--------kvlist_test end--------\n");

    printf("--------uloop_fd_test start--------\n");
    uloop_fd_test();
    printf("--------uloop_fd_test end--------\n");

    printf("--------uloop_timeout_test start--------\n");
    uloop_timeout_test();
    printf("--------uloop_timeout_test end--------\n");        

    printf("--------uloop_fd_uart_test start--------\n");
    uloop_fd_uart_test();
    printf("--------uloop_fd_uart_test start--------\n");

    printf("--------usock_and_uloop_fd_test start--------\n");
    usock_and_uloop_fd_test();
    printf("--------usock_and_uloop_fd_test end--------\n");

    printf("--------ubus_test start--------\n");
    ubus_test();
    printf("--------ubus_test end--------\n");

    printf("--------ubus_event_test start--------\n");
    ubus_event_test();
    printf("--------ubus_event_test end--------\n");
    
    
    return 1;
}

