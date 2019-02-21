
#include "eos.h"
#include "gbl_macrodef.h"
#include "wireless_dataconvert.h"
#include "wireless_string.h"

static _USHORT16 initial_year=2000;


const unsigned short gb_UCS2Table[GSM_2_GBK_TOTALNUM] = {
0x0394,0x03a6,0x0393,0x039b,0x03a9,0x03a0,0x03a8,0x03a3,0x0398,0x039e,0x20ac,0x00a3,
0x00a5,0x00e8,0x00e9,0x00f9,0x00ec,0x00f2,0x00e7,0x00d8,0x00f8,0x00c5,0x00e5,0x00c6,
0x00e6,0x00df,0x00c9,0x00a4,0x00a1,0x00c4,0x00d6,0x00d1,0x00dc,0x00a7,0x00bf,0x00e4,
0x00f6,0x00f1,0x00fc,0x00e0,
};


/*GSM Default Alphabet*/
const unsigned short gb_GSMDefaultTable[] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0xff,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
};

/*GSM Extend Alphabet,for example: extend code 0x14 character use GSM code is 0x1b14
*/
const unsigned short gb_GSMExtendTable[] = {
0x14,0x28,0x29,0x2f,0x3c,0x3d,0x3e,0x40,
};

const unsigned short gb_GSM2UNICODETable[] = {
0x0040,0x00a3,0x0024,0x00a5,0x00e8,0x00e9,0x00f9,0x00ec,0x00f2,0x00e7,0x000a,0x00d8,0x00f8,0x000d,0x00c5,0x00e5,
0x0094,0x035f,0x00a6,0x0393,0x039b,0x03a9,0x03a0,0x03a8,0x03a3,0x0398,0x039e,0x00ff,0x03c6,0x00e6,0x00df,0x00c9,
0x0020,0x0021,0x0022,0x0023,0x00a4,0x0025,0x0026,0x0027,0x0028,0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,0x002f,
0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003a,0x003b,0x003c,0x003d,0x003e,0x003f,
0x00a1,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004a,0x004b,0x004c,0x004d,0x004e,0x004f,
0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005a,0x00c4,0x00d6,0x00d1,0x00dc,0x00a7,
0x00bf,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006a,0x006b,0x006c,0x006d,0x006e,0x006f,
0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x00e4,0x00f6,0x00f1,0x00fc,0x00e0,
0x005e,0x007b,0x007d,0x005c,0x005B,0x007e,0x005d,0x007c,
};


/**
 *@brief Convert Unicode string to GSM string
 *
 *@param T_pTSTR strUnicode: source string
 *@param T_pSTR strGSM: destiminate string
 *@return T_BOOL
 */
_BOOL ConvUnicodeToGSM(_USHORT16* strUnicode, _CHAR8* strGSM)
{
    _SHORT16 i = 0, j, k;
    _USHORT16 ic;
    _BOOL value = EOS_TRUE;
    _CHAR8* strP;

    strP = (_CHAR8*)strGSM;
    j = Utl_StrLen32(strUnicode);
    
    while (i < j)
    {
        ic = strUnicode[i++];
        for (k = 0; k < GSMALPHASTART+GSMALPHAEXTEND+1; k++)
        {
            if (ic == gb_GSM2UNICODETable[k])
            {
                if (k > GSMALPHASTART)
                {
                    *strP++ = 0x1b;
                    *strP++ = gb_GSMExtendTable[k-GSMALPHASTART-1];
                    break;
                }
                else
                {
                    *strP++ = (_UCHAR8)gb_GSMDefaultTable[k];
                    break;
                }
            }
        }
        if (k >= GSMALPHASTART+GSMALPHAEXTEND+1)
        {
            value = EOS_FALSE;
            break;
        }
    }
    *strP = 0;
    return value;
}

/**
  * @brief Convert default GSM alphabet to UCS2 string
  *
  * @param[in]  T_pCSTR strGSM    source string
  * @param[out] T_pSTR   strUCS2   destiminate string
  *@return the length of the UCS2 string
  */
_SHORT16 ConvGSMToUCS2Str(const _CHAR8* strGSM, _CHAR8* strUCS2)
{
    _USHORT16 usGSM = 0;
    _UCHAR8 strTemp[5] = {0};
    _SHORT16     i = 0;
    _SHORT16    len = 0;
    _SHORT16    ret = 0;
    
    len = strlen(strGSM);
    while (i < len)
    {
        usGSM = (_USHORT16)strGSM[i];
        Utl_Itoa((_USHORT16)usGSM, (_CHAR8*)strTemp, 16);
        strcat((_CHAR8*)strUCS2, (_CHAR8*)strTemp);

        i++;
        
        ret += 2;
    }

    return ret;
}

/**
 *@brief Convert the UNICODE name to UCS2
 *
 *@param  T_pCTSTR strText    source string
 *@param  T_pSTR   strUCS2   destiminate string
 *@return the length of the UCS2 string
 **/
_SHORT16 ConvUnicodeToUCS2Str(const _USHORT16* strText, _CHAR8* strUCS2)
{
    _SHORT16  i, j, k, ret;
    _UCHAR8 strTemp[5];
    _USHORT16  usUnicode = 0;

    i = 0;
    j = Utl_StrLen32(strText);
    strUCS2[0] = 0;
    ret = 0;

    while (i < j)
    {
        usUnicode = (_USHORT16)strText[i];
        for (k = 0; k < GSMALPHAEXTEND+GSMALPHASTART+1; k++)
        {
            if (usUnicode == gb_GSM2UNICODETable[k])
            {
                if (k < GSMALPHASTART+1) /*GSM alphabet*/
                {
                    usUnicode = (_USHORT16)(gb_GSMDefaultTable[k]);

                    break;
                }
                else /*GSM extend alphabet*/
                {

                    usUnicode = (_USHORT16)((0x1b<<8)+gb_GSMExtendTable[k-GSMALPHASTART-1]);
                    break;
                }
            }
        }
        /*convert usUnicode to UCS2    */
        if (usUnicode > 0x0fff)
        {
            
        }
        else if (usUnicode > 0x00ff)
        {
            strcat((_CHAR8*)strUCS2, "0");
        }
        else if (usUnicode > 0x000f)
        {
            strcat((_CHAR8*)strUCS2, "00");
        }
        else
        {
            strcat((_CHAR8*)strUCS2, "000");
        }
        Utl_Itoa( (_USHORT16)usUnicode, (_CHAR8*)strTemp, 16 );
        strcat((_CHAR8*)strUCS2, (_CHAR8*)strTemp);
        ret += 4;
        
        i++;
    }
    return ret;    
}

/**
 * @brief convert hex string to integer
 * 
 * @param T_pSTR str_Hex:    source hex string pointer.
 * @return T_U16: the integer value.
 * @retval 
 */
_USHORT16 ConverHexToi(const _CHAR8* str_Hex)
{
    _USHORT16 i,c;
    _SHORT16  length,j;

    length = strlen((_CHAR8*)str_Hex);
    i = 0;

    for( j=length-1; j>=0; j-- )
    {
        c = (_UCHAR8)str_Hex[j];
        if('a' <= c && c <= 'f')
        {
            c -= 'a';
            c += 10;
        }
        else if('A' <= c && c <= 'F')
        {
            c -= 'A';
            c += 10;
        }
        else 
            c -= '0';

        c <<= ((length - 1 - j) * 4);
        i |= c;
    }
    return i;
}


/**
 *@brief Convert UCS2 mode string to UNICODE mode string
 *
 *@param  T_pCSTR strUCS2  source string
 *@param  T_pTSTR strUnicode destiminate string for phonebook entry
 *return bool value
 **/
_BOOL ConvUCS2StrToUnicode(const _CHAR8* strUCS2, _USHORT16* strUnicode)
{
    _SHORT16 i,j, k;
    _USHORT16    iC;
    _BOOL  value = EOS_TRUE;
    _USHORT16* strP;
    _CHAR8 strPart[16];

    i = 0;
    j = strlen((_CHAR8*)strUCS2);
    
    if((j % 4) != 0)
    {
        value = EOS_FALSE;
    }

    strP = (_USHORT16*)strUnicode;
    while( i<j )
    {
        Utl_StrMid(strPart, strUCS2, i, (_USHORT16)(i + 3));
        
        iC = ConverHexToi((_CHAR8*)strPart);
        if (iC == 0x1b) /*extend GSM alphabet*/
        {
            i += 4;
            Utl_StrMid(strPart, strUCS2, i, (_USHORT16)(i + 3));
            iC = ConverHexToi((_CHAR8*)strPart);
            for (k = 0; k < 8; k++)
            {
                if (iC == gb_GSMExtendTable[k])
                {
                    iC = gb_GSM2UNICODETable[k+0x80];
                    break;

                }
            }
        }
        else if (iC < 0x80)
        {
            for (k = 0; k < 128; k++)
            {
                if (iC == gb_GSMDefaultTable[k])
                {
                    iC = gb_GSM2UNICODETable[k];
                    break;
                }
            }
        }

        *strP++ = iC;
            
        i += 4;
    }
    *strP = 0;
    
    return value;
}

/**
  * @brief convert 8bit data to unicode
  * function used in stk
  * 
  * @param[in]: T_pSTR str8BitData, source string with 8bit
  * @param[out]: T_pTSTR strUnicode, destiminate string with unicode
  * @retval: the length of the unicode string
  */
_SHORT16 Conv8BitDataToUnicode(_CHAR8* str8BitData, _USHORT16* strUnicode)
{
    _USHORT16 i = 0, ret = 0, len;
    _USHORT16* strP;
    _UCHAR8 strPart[16] = {0};

    strP = (_USHORT16*)strUnicode;
    len = strlen(str8BitData);
    while (i < len)
    {
        Utl_StrMid(strPart, str8BitData, i, (_USHORT16)(i + 1));
        *strP++ = ConverHexToi((_CHAR8*)strPart);

        i += 2;

        ret += 1;
    }

    *strP = 0;
    return ret;
}

_BOOL Utl_StrIsHex(const _CHAR8* strMain)
{
    _UINT32 i;
    _UINT32   len;

    len = Utl_StrLen32(strMain);

    for (i = 0; i < len; i++)
    {
        if ( ( strMain[i] <= '9' && strMain[i] >= '0' ) 
            || (strMain[i] >= 'A' && strMain[i] <= 'F')  
            || (strMain[i] >= 'a' && strMain[i] <= 'f') 
            || (strMain[i] == 0x0D) 
            || (strMain[i] == 0x0A) )
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
 * @brief Get the day number from Initial years, 
 *          default value of initial year is 2000, 
 *           if 2000-1-1, return 0 
 * 
 * @param T_SYSTIME *dtTime
 * @return T_S16
 * @retval 
 */
static _SHORT16 GetDayNumFromInitialYear(RIL_SYSTIME *dtTime)
{
    _SHORT16 i;
    _SHORT16 year;
    _SHORT16 month;
    _SHORT16 day;
    _SHORT16 month_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    _SHORT16 daynum = -1;
    
    year = dtTime->year;
    month = dtTime->month;
    day = dtTime->day;
    if (year < 100)
        year = initial_year + year;
    else if (year >= initial_year)
        year = initial_year + year % initial_year;

    if (year < initial_year || month <= 0 || day <= 0)
        return 0;

    for (i = initial_year; i < year; i++)
    {
        if (!(i%4 == 0 && i%100 != 0 || i%400 == 0))
             daynum += 365;
        else
             daynum += 366;
    }
    for (i = 1; i < month; i++)
    {
        if (i == 2 && (year%4 == 0 && year%100 != 0 || year%400 == 0))
             daynum += 29;
        else
             daynum += month_day[i-1];
    }
    return (daynum + day);
}


/**
 * @brief Add days to the current time
 * 
 * @param T_SYSTIME *dtTime
 * @param  T_U16 days
 * @return T_VOID
 * @retval 
 */
_VOID Time_AddDays(RIL_SYSTIME *dtTime, _USHORT16 days)
{
    _SHORT16 daynum;
    _SHORT16 daynumsave;
    _SHORT16 i;
    _SHORT16 year;
    _SHORT16 month_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    daynum = GetDayNumFromInitialYear(dtTime) + days;
    daynumsave = daynum;
    if (daynum <= 0)
        return;

    for (i = initial_year; ; i++)
    {
        if (!(i%4 == 0 && i%100 != 0 || i%400 == 0))
             daynum -= 365;
        else
             daynum -= 366;

        if (daynum < 0)
        {
            daynum = daynumsave;
            break;
        }
        else
            daynumsave = daynum;
    }
    dtTime->year = (_USHORT16)i;
    year = dtTime->year;

    for (i = 1; i <= 12; i++)
    {
        if (i == 2 && (year%4 == 0 && year%100 != 0 || year%400 == 0))
             daynum -= 29;
        else
             daynum -= month_day[i-1];

        if (daynum < 0)
        {
            daynum = daynumsave;
            break;
        }
        else
            daynumsave = daynum;
    }
    dtTime->month = (_UCHAR8)i;
    dtTime->day = (_UCHAR8)(daynum + 1);
    
    return;
}


/**
 * @brief Add hours to the current time
 * 
 * @param T_SYSTIME *dtTime
 * @param  T_U16 hours
 * @return T_VOID
 * @retval 
 */
_VOID Time_AddHours(RIL_SYSTIME *dtTime, _USHORT16 hours)
{
    _USHORT16 curHour;
    
    curHour = dtTime->hour + hours;
    dtTime->hour = (_UCHAR8)(curHour % 24);
    if (curHour >= 24)
        Time_AddDays(dtTime, (_SHORT16)(curHour / 24));

    return;
}


