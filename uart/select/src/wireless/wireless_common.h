
#ifndef WIRELESS_COMMON_H
#define WIRELESS_COMMON_H

#include <ut_mem.h>

#define AT_END_FLAG        0x0A
#define SMS_SEND_FLAG    '>'

#define WIRELESS_COM_TRANS_RDY_MASK        (0x20)

/* The maximum size of a USSD message is 160 bytes. For GSM 7 Bit,size is 182 characters,For UCS,it's 80 characters.*/
#define WIRELESS_USSD_MAX_LEN                512

#define WIRELESS_USSD_GETRSP_TIMEOUT        (200*60*2)

#define WIRELESS_INQ_INITSTATUS_PERIOD        2000

#define WIRELESS_INQ_BSINFO_DELAY            (30*1000)

#define WIRELESS_INQ_BSINFO_PERIOD         (10*1000)

#define WIRELESS_BALANCE_MULTIPLE_PARAM        10000

#define WIRELESS_BALANCE_MAX_VALUE            2000000000

#define WIRELESS_AT_CMD_WAITING_MAX                16

#define WIRELESS_MAX_SMS_PKT_NUMBER                8
#define WIRELESS_MAX_SMS_PKT_PDU_LEN            512

#define WRL_READ_BALANCE_DIGIT_NUM                4

enum
{
    NVM_GOIP_MODTYPE_M10 = 0,
    NVM_GOIP_MODTYPE_2406,
    NVM_GOIP_MODTYPE_C202,
    NVM_GOIP_MODTYPE_MC323,
    NVM_GOIP_MODTYPE_MC600,
    NVM_GOIP_MODTYPE_S5320,
    NVM_GOIP_MODTYPE_CM300,
    NVM_GOIP_MODTYPE_U10,
    NVM_GOIP_MODTYPE_UC15,
    NVM_GOIP_MODTYPE_G610,
    NVM_GOIP_MODTYPE_UC20,
    NVM_GOIP_MODTYPE_H330,
    NVM_GOIP_MODTYPE_L810,
    NVM_GOIP_MODTYPE_SIM800,
    NVM_GOIP_MODTYPE_EC20,
    NVM_GOIP_MODTYPE_M26,

    NVM_GOIP_MODTYPE_MAX
};

typedef enum
{
    EN_WIRELESS_GET_BAN_KEYWORD_NOFOUND = -100,
    EN_WIRELESS_GET_BAN_KEYWORD_SIMBLOCKED,
    EN_WIRELESS_GET_BAN_DIGIT_NOFOUND,
    EN_WIRELESS_GET_BAN_INVALID,
    EN_WIRELESS_GET_BAN_BUTT
}EN_WIRELESS_GET_BALANCE_RESULT;


#define WIRELESS_MALLOC(size)               ut_mem_alloc((size), __FILE__, __LINE__)
#define WIRELESS_REMALLOC(addr, size)       ut_mem_realloc((addr), (size), __FILE__, __LINE__)
#define WIRELESS_FREE(addr)                 ut_mem_free(addr, __FILE__, __LINE__)

#if ((ACOM_BOARD_TYPE == RALINK) || (ACOM_BOARD_TYPE == MT673X) || (ACOM_BOARD_TYPE == MDM9X07)|| (ACOM_BOARD_TYPE == MSM89XX) || (ACOM_BOARD_TYPE == SC98XX))
#if ((ACOM_PORT_NUM == 4) || (ACOM_PORT_NUM == 8) || (ACOM_PORT_NUM == 16))
#if ((ACOM_HW_SUBTYPE == HWST_GOIP16X8) || (ACOM_HW_SUBTYPE == HWST_GOIP4P) || (ACOM_HW_SUBTYPE == HWST_GOIP8P))
typedef struct {
    unsigned int channelid;
    unsigned int baudrate;
}__attribute__((packed)) ioctl_channel_baudrate;

typedef struct {
    unsigned int channelid;
    unsigned int baudrate;
}__attribute__((packed)) ioctl_channel_setipr;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[1024];
}__attribute__((packed)) ioctl_channel_read_st;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[512];
}__attribute__((packed)) ioctl_channel_write_st;

typedef struct {
    unsigned int baudrate;    /* 0:38400, 1:115200    */
}__attribute__((packed)) ioctl_fpga_baudrate;

#define LTQ_FPGA_UART_IOC_MAGIC          0x99

#define LTQ_FPGA_UART_GET_DATA_READ         _IO(LTQ_FPGA_UART_IOC_MAGIC, 2)
#define LTQ_FPGA_UART_SET_DATA_WRITE        _IO(LTQ_FPGA_UART_IOC_MAGIC, 3)
#define LTQ_FPGA_UART_SET_IPR_WRITE         _IO(LTQ_FPGA_UART_IOC_MAGIC, 4)
#define LTQ_FPGA_UART_SET_BAUDRATE          _IO(LTQ_FPGA_UART_IOC_MAGIC, 5)

#else
/* for ch438    */
typedef struct {
    unsigned int channelid;
    unsigned int baudrate;
}__attribute__((packed)) ioctl_channel_baudrate;

typedef struct {
    unsigned int channelid;
    unsigned int baudrate;
}__attribute__((packed)) ioctl_channel_setipr;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[1024];
}__attribute__((packed)) ioctl_channel_read_st;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[512];
}__attribute__((packed)) ioctl_channel_write_st;

#define CH438_IOC_MAGIC          0x92

#define CH438_SET_COM_ADAPT         _IO(CH438_IOC_MAGIC, 1)
#define CH438_GET_DATA_READ         _IO(CH438_IOC_MAGIC, 2)
#define CH438_SET_DATA_WRITE        _IO(CH438_IOC_MAGIC, 3)
#define CH438_SET_IPR_WRITE         _IO(CH438_IOC_MAGIC, 4)

#endif
#else
typedef struct {
    unsigned int channelid;
    unsigned int baudrate;
}__attribute__((packed)) ioctl_channel_baudrate;

typedef struct {
    unsigned int channelid;
    unsigned int baudrate;
}__attribute__((packed)) ioctl_channel_setipr;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[256];
}__attribute__((packed)) vsim_read_st;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[1024];
}__attribute__((packed)) ioctl_channel_read_st;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[512];
}__attribute__((packed)) ioctl_channel_write_st;

typedef struct {
    unsigned int baudrate;    /* 0:38400, 1:115200    */
}__attribute__((packed)) ioctl_fpga_baudrate;

#define LTQ_FPGA_UART_IOC_MAGIC          0x99

#define LTQ_FPGA_UART_GET_DATA_READ         _IO(LTQ_FPGA_UART_IOC_MAGIC, 2)
#define LTQ_FPGA_UART_SET_DATA_WRITE        _IO(LTQ_FPGA_UART_IOC_MAGIC, 3)
#define LTQ_FPGA_UART_SET_IPR_WRITE         _IO(LTQ_FPGA_UART_IOC_MAGIC, 4)
#define LTQ_FPGA_UART_SET_BAUDRATE          _IO(LTQ_FPGA_UART_IOC_MAGIC, 5)

#endif

#define LTQ_SIMPOOL_RSTDET_MAX 4

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[1024];
}__attribute__((packed)) ioctl_simpool_read_st;

typedef struct {
    unsigned int channelid;
    unsigned int size;
    unsigned char buff[512];
}__attribute__((packed)) ioctl_simpool_write_st;


typedef struct {
    unsigned char simrstdet[LTQ_SIMPOOL_RSTDET_MAX];
}__attribute__((packed)) ioctl_get_simrst_st;

typedef struct {
    unsigned int channelid;
}__attribute__((packed)) ioctl_simpool_prerst_st;

typedef struct {
    unsigned int channelid;
}__attribute__((packed)) ioctl_simpool_clrwdata_st;

typedef struct {
    unsigned int baud;
}__attribute__((packed)) ioctl_simpool_setbaud_st;

typedef struct {
    unsigned int channelid;
    unsigned int etu;
}__attribute__((packed)) ioctl_simpool_setetu_st;

#define LTQ_SIMPOOL_IOC_MAGIC                  0x9a

#define LTQ_SIMPOOL_GET_DATA_READ             _IO(LTQ_SIMPOOL_IOC_MAGIC, 1)
#define LTQ_SIMPOOL_SET_DATA_WRITE            _IO(LTQ_SIMPOOL_IOC_MAGIC, 2)
#define LTQ_SIMPOOL_GET_RESET_SIGNAL          _IO(LTQ_SIMPOOL_IOC_MAGIC, 3)
#define LTQ_SIMPOOL_SET_PRERST_DATA         _IO(LTQ_SIMPOOL_IOC_MAGIC, 4)
#define LTQ_SIMPOOL_CLEAR_WRITE_DATA          _IO(LTQ_SIMPOOL_IOC_MAGIC, 5)
#define LTQ_SIMPOOL_SET_BAUDRATE            _IO(LTQ_SIMPOOL_IOC_MAGIC, 6)  /* Set for M35 or MC323    */
#define LTQ_SIMPOOL_SET_ETU                     _IO(LTQ_SIMPOOL_IOC_MAGIC, 7)
#define LTQ_SIMPOOL_SET_DATA_WRITE_PUSH        _IO(LTQ_SIMPOOL_IOC_MAGIC, 8)

/* end    */
#endif

#endif


