
#ifndef WIRELESS_VIFI_UART_H
#define WIRELESS_VIFI_UART_H


#ifdef __cplusplus
extern "C" {
#endif

#if (ACOM_HW_SUBTYPE == HWST_2G4G)
typedef enum {
    ENUM_TTY_0=0,
    ENUM_TTY_1,
    ENUM_TTY_2,
    ENUM_TTY_3,
    ENUM_TTY_4
}ENUM_TTY_DEV;
#else
typedef enum {
    ENUM_AT_ACM,
    ENUM_LOCK_ACM,
    ENUM_NET_ACM
}ENUM_TTY_DEV;
#endif

#if (ACOM_BOARD_TYPE == MT673X)
#define UART_LOW_BAUD       8736
#define UART_HIGH_BAUD      101556
#elif (ACOM_BOARD_TYPE == MSM89XX)
#define UART_LOW_BAUD       8736
#define UART_HIGH_BAUD      115200
#elif (ACOM_BOARD_TYPE == RALINK)
#define UART_HIGH_BAUD      115200
#endif

_INT32 WrUart_getAcmDevName(unsigned char *deviceName, ENUM_TTY_DEV acmDev);
_INT32 WrlUart_Init(_INT32 ChannelId);
_INT32 WrlUart_unInit(_INT32 ChannelId, _INT32 fd);
_INT32 WrlUart_Write(_INT32 fd, _UCHAR8* p_data, _UINT32 length);
_INT32 WrlUart_Flush(_INT32 fd);
_INT32 WrlUart_Read(_INT32 fd, _UCHAR8* p_data);

_INT32 Wrl_UartVsimInit(_INT32 baud);


#ifdef __cplusplus
}
#endif

#endif

