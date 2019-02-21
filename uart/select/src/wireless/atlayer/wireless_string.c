
#include "eos.h"
#include "wireless_string.h"
#include "wireless_debug.h"
#include "gbl_macrodef.h"
/**
 * @brief search for strSub in strMain from the offset.
 * This fucntion should be called if the string is very long.
 * @param T_pSTR strMain head address of main string to search in
 * @param  T_pSTR strSub head address of sub string to search for
 * @param  T_S16 offset begin address offset
 * @param  T_U16 length: string length
 * @return T_S16
 * @retval -1: not find; else offset in main string
 */
_SHORT16 Utl_StrFndL(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 length)
{
    _USHORT16        curLoc = offset;
    _USHORT16        i = 0;
    _USHORT16        subStrLen;
    const _CHAR8*    pMain;


    if (offset < 0)
        return -1;

    subStrLen = strlen(strSub);
    if ((offset + subStrLen) > length)
    {
        return -1;
    }
    
    pMain = strMain + offset;

    while (*(pMain + subStrLen - 1) != 0)
    {
        for (i = 0; i < subStrLen; i++)
        {
            if (*(pMain + i) != *(strSub + i))
                break;
        }
        if (i == subStrLen)
        {
            return curLoc;
        }

        curLoc++;
        pMain++;
    }

    return -1;
}

/**
 * @brief Insert one string(strSub) into another string(strDest).
 * 
 * @param T_pSTR strDest destination string
 * @param  T_pSTR strSub sub-string
 * @param  T_S16 offset the location of strDest where the strSub will be added in
 * @return T_S8
 * @retval head of destination string
 */
_CHAR8* Utl_StrIns(_CHAR8* strDest, const _CHAR8* strSub, _SHORT16 offset)
{
    _USHORT16    lenDest, lenSub;
    _SHORT16    i;
    _SHORT16    curOffset = (_SHORT16)offset;

    if (offset < 0)
        return strDest;

    lenDest = strlen(strDest);
    lenSub    = strlen(strSub);

    if (lenSub == 0)
        return strDest;
    if ((_USHORT16)curOffset > lenDest)
        curOffset = lenDest;
    strDest[lenDest + lenSub] = 0;
    for (i = lenDest + lenSub - 1; i >= (_SHORT16)(curOffset + lenSub); i--)
        strDest[i] = strDest[i-lenSub];
    for (i = curOffset + lenSub - 1; i >= curOffset; i--)
        strDest[i] = strSub[i - curOffset];
  
    return strDest;
}


_CHAR8* Utl_Itoa(_INT32 intNum, _CHAR8* strDest, _UCHAR8 flag)
{
    _INT32        i = 0;
    _INT32        datanew;
    _INT32        index;
    T_STR_100    strTemp;
    _BOOL        negv = EOS_FALSE;

    strDest[0] = '\0';
    if (intNum < 0)
    {
        intNum *= (-1);
        negv = EOS_TRUE;
    }

    if (flag == 16)
    {
        if (intNum < 16)
        {
            if(intNum >= 10)
                strDest[0] = (_CHAR8)(intNum + 55);
            else 
                strDest[0] = (_CHAR8)(intNum + 48);
            strDest[1] = '\0';
        }
        else
        {
            while (intNum >= 16)
            {
                datanew = intNum;
                intNum = intNum/16;
                if((datanew - intNum*16) >= 10)
                    strTemp[i] = (_CHAR8)(datanew - intNum * 16 + 55);
                else 
                    strTemp[i] = (_CHAR8)(datanew - intNum * 16 + 48);
                i ++ ;
                if (intNum < 16)
                {
                    if(intNum >= 10)
                        strTemp[i] = (_CHAR8)(intNum + 55);
                    else if(intNum != 0)
                        strTemp[i] = (_CHAR8)(intNum + 48);
                    strTemp[i + 1] = 0;
                    break;
                }
            }
            for( index = 0; index <= i; index ++)
                *(strDest + index) = strTemp[i - index];
            *(strDest + index) = 0;
        }
    }
    else
    {
        if (intNum < 10)
        {
            strDest[0] = (_CHAR8)(intNum + 48);
            strDest[1] = '\0';
        }
        else
        {
            while(intNum >= 10)
            {
                datanew = intNum;
                intNum = intNum/10;
                strTemp[i] = (_CHAR8)(datanew - intNum * 10 + 48);
                i ++ ;
                if(intNum < 10)
                {    
                    strTemp[i] = (_CHAR8)(intNum + 48);
                    strTemp[i + 1] = 0;
                    break;
                }
            }
            for( index = 0; index <= i; index ++)
                *(strDest + index) = strTemp[i - index];
            *(strDest + index) = 0;
        }
    }

    if (negv)
        Utl_StrIns(strDest, "-", 0);

    return strDest;
}

/**
 * @brief : Convert string to integer.
*
 * @param T_pDATA strDest
 * @return _INT32
 * @retval integer value.
 */
_INT32 Utl_Atoi(const _CHAR8* strMain)
{
    const _UCHAR8* pMain = NULL;
    _INT32        sum;
    _BOOL        negv = EOS_FALSE;
    _INT32        i = 0;

    pMain = (const _UCHAR8*)strMain;
    sum = 0;
    if ((*pMain) == '-')
    {
        negv = EOS_TRUE;
        pMain++;
    }

    while (*pMain)
    {
        if ('0' <= (*pMain) && (*pMain) <= '9')
            sum = sum * 10 + (*pMain - '0');
        else
            sum = sum * 10;
        pMain++;
        i++;
    }

    if (negv)
        sum *= (-1);

    return sum;
}

/**
 * @brief cur strSour from offset to end and copy to strDest
 * 
 * @param T_pSTR strDest: head address of dest string
 * @param  T_pSTR strSour: head address of source string
 * @param  T_S16 offset: begin index
 * @param  T_S16 end:  end index
 * @return T_S8
 * @retval void
 */

_CHAR8* Utl_StrMid_N_File(_CHAR8* strDest, _INT32 size, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, const _CHAR8* file, _INT32 line)
{
    _USHORT16 iLength;

    iLength = strlen( strSour );
    if(size <= (end - offset))
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Utl_StrMid_N(%d < %d)(%s:%d)\r\n", size, (end - offset), file, line);
        end = offset + size - 1;
    }

    return Utl_StrMidL32( strDest, strSour, offset, end, iLength );
}

_CHAR8* Utl_StrMidL_N_File(_CHAR8* strDest, _INT32 size, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, _USHORT16 strlength, const _CHAR8* file, _INT32 line)
{
    if(size <= (end - offset))
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Utl_StrMidL_N(%d < %d)(%s:%d)\r\n", size, (end - offset), file, line);
        end = offset + size - 1;
    }

    return Utl_StrMidL16( strDest, strSour, offset, end, strlength);
}


/**
 * @brief copy sub string from specify start and end place
 *
 * @param T_pSTR strDest: head address of destination string
 * @param  T_pSTR strSour: head address of source string
 * @param  T_S32 offset: begin address offset
 * @param  T_U32 end: end address
 * @param  T_U32 length: string length
 * @return T_S8 head address of destination string
 */
_CHAR8* Utl_StrMidL32(_CHAR8* strDest, const _CHAR8* strSour, _INT32 offset, _INT32 end, _UINT32 strlength)
{
    _INT32    iLength;
    _INT32    i;

    strDest[0] = '\0';
    if (offset < 0 || end < 0)
        return strDest;
    
    if (offset > end)
        return strDest;

    iLength = strlength;
    if (iLength == 0)
        return strDest;

    if (offset > iLength - 1)
        offset = iLength - 1;
    if (end > iLength - 1)
        end = iLength - 1;

    for (i = offset; i <= end; i++){            
        if ((i < iLength-1) &&
            Utl_IsDWordChar(strSour + i)){    /*It isn't last char and it's DWord character*/
            if (i==end)
                break;    
            strDest[i - offset] = *(strSour + i);
            i++;
        }
        strDest[i - offset] = *(strSour + i);
    }
    strDest[i - offset] = '\0';
    return strDest;
}

/**
 * @brief copy sub string from specify start and end place
 *
 * @param T_pSTR strDest: head address of destination string
 * @param  T_pSTR strSour: head address of source string
 * @param  T_S16 offset: begin address offset
 * @param  T_U16 end: end address
 * @param  T_U16 length: string length
 * @return T_S8 head address of destination string
 */
_CHAR8* Utl_StrMidL16(_CHAR8* strDest, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, _USHORT16 strlength)
{
    _SHORT16    iLength = 0;
    _SHORT16    i = 0;

    strDest[0] = '\0';
    if (offset < 0 || end < 0)
        return strDest;
    
    if (offset > end)
        return strDest;

    iLength = strlength;
    if (iLength == 0)
        return strDest;

    if (offset > iLength - 1)
        offset = iLength - 1;
    if (end > iLength - 1)
        end = iLength - 1;

    for (i = offset; i <= end; i++){            
        if ((i < iLength-1) &&
            Utl_IsDWordChar(strSour + i)){    /*It isn't last char and it's DWord character*/
            if (i==end)
                break;    
            strDest[i - offset] = *(strSour + i);
            i++;
        }
        strDest[i - offset] = *(strSour + i);
    }
    strDest[i - offset] = '\0';
    return strDest;
}


/**
 * @brief Check the first of the string is Chinese character or not.
 * 
 * @param T_pDATA string
 * @return T_BOOL
 * @retval AK_TRUE:    found DWORD character; AK_FALSE: not found.
 */
_BOOL Utl_IsDWordChar(const _CHAR8* strMain)
{
    const _UCHAR8* pMain;


    pMain = (const _UCHAR8*)strMain;

    if (*(pMain) > 0x80) 
        return EOS_TRUE;
    return EOS_FALSE;
}

_SHORT16 Utl_StrFndNL(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 count, _USHORT16 length)
{
    _USHORT16    curLoc = offset;
    _USHORT16    i;
    _USHORT16    subStrLen;
    _USHORT16    mainStrLen;
     const _CHAR8*    pMain;


    if (offset < 0)
        return -1;

    subStrLen = strlen(strSub);
    mainStrLen = length;
    if (offset + subStrLen > mainStrLen)
        return -1;
    if (subStrLen > count)
        return -1;

    pMain = strMain + offset;

    while (*(pMain + subStrLen - 1) != 0)
    {
        for (i = 0; i < subStrLen; i++)
        {
            if (*(pMain + i) != *(strSub + i))
                break;
        }
        if (i == subStrLen)
        {
            return curLoc;
        }

        curLoc++;
        pMain++;
        if ((curLoc + subStrLen) > (offset + count))
            break;
    }

    return -1;
}

/**
 * @brief Compare two string
 *
 * @param T_pSTR str1 head address of string1
 * @param  T_pSTR str2 head address of string1
 * @return T_S16
 * @retval compare value
 */
_CHAR8 Utl_StrCmp(const _CHAR8* str1, const _CHAR8* str2)
{
    _SHORT16 len = 0;
    const _UCHAR8*    pStr1 = (const _UCHAR8*)str1;
    const _UCHAR8*    pStr2 = (const _UCHAR8*)str2;

    while (((_UCHAR8)(*(pStr1)) != 0) || ((_UCHAR8)(*(pStr2)) != 0))
    {
        if ((*(pStr1)) > (*(pStr2)))
            return 1;

        if ((*(pStr1)) < (*(pStr2)))
            return -1;

        pStr1++;
        pStr2++;
        len++;
    }

    return 0;
}

/**
 * @brief the string is empty or not
 * 
 * @param T_pSTR str strMain--head address of string
 * @return T_S32
 * @retval the length
 */
_BOOL    Utl_StrIsEmpty(const _CHAR8* strMain)
{
    if (*strMain == '\0')
        return EOS_TRUE;

    return EOS_FALSE;
}

/**
 * @brief Check the string is digital character or not.
 * 
 * @param T_pDATA string
 * @return T_BOOL
 * @retval AK_TRUE:    found Chinese character; FALSE: not found.
 */
_BOOL Utl_StrDigital(const _CHAR8* strMain)
{
    _UINT32 i;
    _UINT32   len;

    len = Utl_StrLen32(strMain);

    for (i = 0; i < len; i++)
    {
        if ( ( strMain[i] <= '9' && strMain[i] >= '0' ) || strMain[i] == '+' || strMain[i] == '*' )
        {
        }
        else
        {
            return EOS_FALSE;
        }
    }
    
    return EOS_TRUE;
}

/**
 * @brief get length of a string
 *
 * @param T_pSTR str strMain--head address of string
 * @return T_S32
 * @retval the length
 */
_INT32 Utl_StrLen32(const _CHAR8* strMain)
{
    _INT32    mainStrLen = 0;
    if (strMain == 0)
        return 0;

    while(strMain[mainStrLen]!=0)
        mainStrLen++;

    return mainStrLen;
}

/**
 * @brief search for a char in strMain from the index element
 * 
 * @param T_pSTR str head address of main string to search in
 * @param  T_S8 chr the char search for
 * @param  T_S16 index begin address offset
 * @return T_S16
 * @retval -1: not find; else offset in main string
 */
_SHORT16 Utl_StrFndChr(const _CHAR8* strMain, _CHAR8 chr, _SHORT16 offset)
{
    _USHORT16 curLoc = offset;
    const _CHAR8*    pMain;


    if (offset < 0)
        return -1;

    if (offset >= strlen(strMain))
        return -1;

    pMain = strMain + offset;

    while (*pMain != 0)
    {
        if (*pMain == chr)
        {
            return curLoc;
        }

        curLoc++;
        pMain++;
    }

    return -1;
}

/**
 * @brief search for a char in strMain from the string tail
 * 
 * @param T_pSTR str head address of main string to search in
 * @param  _UINT32 string length
 * @param  T_S8 chr the char search for
 * @return _SHORT16
 * @retval -1: not find; else offset in main string
 */
_SHORT16 Utl_StrNFndChr_Reverse(const _CHAR8* strMain,  _UINT32 strLen, _CHAR8 chr)
{
    while(strLen > 0)
    {
        if(strMain[--strLen] == chr)
        {
            return strLen;
        }
    }
    
    return -1;
}


/**
 * @brief connect one or more char to strDest
 * 
 * @param T_pSTR strDest head address of destination string
 * @param  T_S8 chr source char
 * @param  T_U16 count
 * @return T_S8
 * @retval head address of destination string
 */
_CHAR8* Utl_StrCatChr(_CHAR8* strDest, _CHAR8 chr, _USHORT16 count)
{
    _USHORT16    i;
    _USHORT16    len;
   
    len = strlen(strDest);
    if (count <= 0)
        return strDest;

    for (i = 0; i < count; i++)
        strDest[len + i] = chr;
    strDest[len + i] = 0;
  
    return strDest;
}


/**
* @brief Append N characters of a string(16 bit len ver)
*
* @param T_pSTR strDest: Null-terminated destination string
* @param T_pCSTR strSrc: Null-terminated source string
* @param T_U16 length: Number of characters to append
* @return T_pSTR
* @retval
*/
_CHAR8* Utl_StrCatN(_CHAR8* strDest, const _CHAR8* strSrc, _USHORT16 length)
{
    _INT32 i = 0;
    _INT32    len;
    _CHAR8* destP = strDest;


    len = Utl_StrLen32(strDest);

    strDest += len;

    while (*(strSrc + i)!=0 && i <((_INT32)(_UINT32)length))
    {
    strDest[i] = strSrc[i];
    i++;
    }
    strDest[i] = 0;

    return strDest;
}

/**
 * @brief Copy 'length' bytes from strSour to strDest.
 * 
 * @param  T_pSTR strDest head address of source string
 * @param  T_pSTR strSour head address of destination string
 * @param  T_S16 length  number of bytes.
 * @return T_S8
 * @retval head address of destination string
 */
 _CHAR8* Utl_StrCpyN(_CHAR8* strDest, const _CHAR8* strSour, _USHORT16 length)
{
    _USHORT16    i = 0;
    _CHAR8*        d;
    const _CHAR8*    s;

    d = strDest;
    s = strSour;
    
    while ((*s) && (i < length))
    {
        *d++ = *s++;
        i++;
    }
    *d = 0;

    return strDest;
}



