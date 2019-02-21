
#ifndef WIRELESS_STRING_H
#define WIRELESS_STRING_H

#include "eos.h"

#define Utl_StrMid(strDest, strSour, offset, end) Utl_StrMid_N(strDest, sizeof(strDest),strSour, offset, end)

#define Utl_StrMid_N(strDest, iLength,strSour, offset, end) Utl_StrMid_N_File(strDest, iLength, strSour, offset, end, __FILE__, __LINE__)

#define Utl_StrMidL(strDest, strSour, offset, end, strlength) Utl_StrMidL_N(strDest, sizeof(strDest),strSour, offset, end, strlength)

#define Utl_StrMidL_N(strDest, iLength,strSour, offset, end, strlength) Utl_StrMidL_N_File(strDest, iLength, strSour, offset, end, strlength, __FILE__, __LINE__)

_SHORT16 Utl_StrFndL(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 length);

_INT32 Utl_Atoi(const _CHAR8* strMain);

_CHAR8* Utl_StrMid_N_File(_CHAR8* strDest, _INT32 size, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, const _CHAR8* file, _INT32 line);
_CHAR8* Utl_StrMidL32(_CHAR8* strDest, const _CHAR8* strSour, _INT32 offset, _INT32 end, _UINT32 strlength);
_CHAR8* Utl_StrMidL_N_File(_CHAR8* strDest, _INT32 size, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, _USHORT16 strlength, const _CHAR8* file, _INT32 line);
_CHAR8* Utl_StrMidL16(_CHAR8* strDest, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, _USHORT16 strlength);
_BOOL Utl_IsDWordChar(const _CHAR8* strMain);
_SHORT16 Utl_StrFndNL(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 count, _USHORT16 length);
_CHAR8 Utl_StrCmp(const _CHAR8* str1, const _CHAR8* str2);
_BOOL    Utl_StrIsEmpty(const _CHAR8* strMain);
_BOOL Utl_StrDigital(const _CHAR8* strMain);
_INT32 Utl_StrLen32(const _CHAR8* strMain);
_SHORT16 Utl_StrFndChr(const _CHAR8* strMain, _CHAR8 chr, _SHORT16 offset);
_SHORT16 Utl_StrNFndChr_Reverse(const _CHAR8* strMain,  _UINT32 strLen, _CHAR8 chr);
_CHAR8* Utl_Itoa(_INT32 intNum, _CHAR8* strDest, _UCHAR8 flag);
_CHAR8* Utl_StrCatChr(_CHAR8* strDest, _CHAR8 chr, _USHORT16 count);
_CHAR8* Utl_StrCatN(_CHAR8* strDest, const _CHAR8* strSrc, _USHORT16 length);
_CHAR8* Utl_StrCpyN(_CHAR8* strDest, const _CHAR8* strSour, _USHORT16 length);


#endif

