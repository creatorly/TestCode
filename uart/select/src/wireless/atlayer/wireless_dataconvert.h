
#ifndef    WIRELESS_DATACONVERT_H
#define WIRELESS_DATACONVERT_H

#include "eos.h"
#include "wm_struct.h"

#define GSM_2_GBK_TOTALNUM    40
#define    GSMALPHASTART        127        /**< start of GSM alphabet*/
#define GSMALPHAEXTEND        8     /**< number of GSM alphabet and GSM extend alphabet*/

_BOOL ConvUnicodeToGSM(_USHORT16* strUnicode, _CHAR8* strGSM);
_SHORT16 ConvGSMToUCS2Str(const _CHAR8* strGSM, _CHAR8* strUCS2);
_SHORT16 ConvUnicodeToUCS2Str(const _USHORT16* strText, _CHAR8* strUCS2);
_USHORT16 ConverHexToi(const _CHAR8* str_Hex);
_BOOL ConvUCS2StrToUnicode(const _CHAR8* strUCS2, _USHORT16* strUnicode);
_SHORT16 Conv8BitDataToUnicode(_CHAR8* str8BitData, _USHORT16* strUnicode);
_BOOL Utl_StrIsHex(const _CHAR8* strMain);
_VOID Time_AddDays(RIL_SYSTIME *dtTime, _USHORT16 days);
_VOID Time_AddHours(RIL_SYSTIME *dtTime, _USHORT16 hours);

#endif




