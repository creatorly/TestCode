
#ifndef WIRELESS_INTERFACE_H
#define WIRELESS_INTERFACE_H

#include "utils/ut_msgq.h"

_INT32 WIRELESS_initUart( _UINT32 channelID);
_INT32 WIRELESS_freeUart( _UINT32 channelID);

_INT32 WIRELESS_Init(_LONG32 param);
_INT32 WIRELESS_unInit(void);   //weikk add 2015.11.10

_INT32 WIRELESS_Start(_VOID);
_VOID  WIRELESS_Stat(EOS_INFO_PRINTF fpPrint, _VOID* pData);

#endif

