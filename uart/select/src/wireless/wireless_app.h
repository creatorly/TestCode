
#ifndef WIRELESS_APP_H
#define WIRELESS_APP_H

#include "wireless_interface_inc.h"

#define WIRELESS_GPIO_MODE_VAL            0x0    /*    all gpio input for ring detecting    */
#define WIRELESS_GPIO_CTRL_VAL            0x0
#define WIRELESS_GOIP_BALANCE_STR         "$GOIP$"
#define WIRELESS_GOIP_LCD_STR             "$LCD$"
#define WIRELESS_CLCC_INQUIRE_PERIOD      2000  /* unit:ms */

_INT32  WIRELESS_ChannelInit(_UINT32 ChannelID);
_INT32  WIRELESS_AppInit(_LONG32 param);
_VOID   WIRELESS_AppMainTask(_VOID);
_VOID   WIRELESS_AppStat(EOS_INFO_PRINTF fpPrint, _VOID* pData);
_VOID   WIRELESS_ComStrWrite(_UINT32 ChanId, _CHAR8* strValue, _UINT32 Len);

_INT32  WRL_FSM_SendEvent(_UINT32 ChannelID, _UINT32 EventType, EN_WRL_FSM_EVENT_ID_INDEX EventID, _UCHAR8 Param);
_VOID   WIRELESS_WakeUp();

#endif

