
#include "eos.h"
#include "../../wireless_debug.h"

#include "WAVEC_at_pack.h"

static void get_stk_id_str(_USHORT16 id, _CHAR8* pData) 
{
    _CHAR8 d[8];

    if (id <= 0xf)
    {
        strcpy(pData, "0,0,0,0,\"0");
    }
    else
    {
        strcpy(pData, "0,0,0,0,\"");
    }
    Utl_Itoa(id, d, 16);
    strcat(pData, d);
    strcat(pData, "\"");
}

/**
 * @brief Convert system time sturcture to string.
 * @param RIL_SYSTIME *SysTime : SYSTEMTIME structure
 * @param  _CHAR8* strDate: standard format date "YY/MM/DD" for return
 * @param  _CHAR8* strTime: standard format time "HH:MM:SS" for return
 * @return _VOID
 */
static _VOID ConvertTimeSToStr(const RIL_SYSTIME* SysTime, _CHAR8* strDate, _CHAR8* strTime)
{
    _CHAR8 strTemp[30];

    strDate[0] = '\0';
    Utl_Itoa(SysTime->year, strTemp, 10);        /* year */
    if (SysTime->year < 10)
    {
        strcat(strDate, "0");
        strcat(strDate, strTemp);
    }
    else
    {
        strcpy(strDate, strTemp + strlen(strTemp) - 2);
    }

    strcat(strDate, "/");
    Utl_Itoa(SysTime->month,strTemp,10);        /* month */
    if(SysTime->month < 10)
        strcat(strDate, "0");
    strcat(strDate, strTemp);

    strcat(strDate, "/");
    Utl_Itoa(SysTime->day,strTemp,10);            /* day */
    if(SysTime->day < 10)
        strcat(strDate, "0");
    strcat(strDate, strTemp);

    strTime[0] = '\0';
    Utl_Itoa((SysTime->hour),strTemp,10);    /* hour */
    if((SysTime->hour) < 10)
        strcat(strTime, "0");
    strcat(strTime, strTemp);

    strcat(strTime, ":");
    Utl_Itoa(SysTime->minute,strTemp,10);        /* minute */
    if(SysTime->minute < 10)
        strcat(strTime, "0");
    strcat(strTime, strTemp);

    strcat(strTime, ":");
    Utl_Itoa(SysTime->second,strTemp,10);        /* minute */
    if(SysTime->second < 10)
        strcat(strTime, "0");
    strcat(strTime, strTemp);

    AK_FUNCTION_LEAVE("ConvertTimeSToStr");
    return;
}/* end ConvertTimeSToStr(SYSTIME SysTime,_CHAR8* strDate, _CHAR8* strTime) */

/**
 * @brief Send standard format AT command
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strAT: AT command
 * @param _CHAR8* strParam:Param of ATCmd
 * @return _VOID
 */
_VOID WaveC_Wm_SendStandardAT(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strAT, const _CHAR8* strParam)
{
    strcpy(ATCmd, strAT);
    if (NULL != strParam)
    {
        strcat(ATCmd, strParam);
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_Wm_SendStandardAT */

/**
 * @brief Delete SMS message
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strIndex: SMS index
 * @return _VOID
 * @retval
 */
_VOID WaveC_WmAt0705_DeleteSMS(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strIndex)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMGD=", strIndex);   
}/* end WaveC_WmAt0705_DeleteSMS */

/**
 * @brief Set SMS message format
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: SMS strMode
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SetSmsFormat(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMode)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMGF=", strMode);
} /* end WaveC_WmAt0705_SetSmsFormat */


_VOID WaveC_WmAt0705_SetTxtSmsShow(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMode)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSDH=", strMode);
} /* end WaveC_WmAt0705_SetTxtSmsShow */

/**
 * @brief List SMS messages from preferred store according to the "stat".
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param RIL_SMS_MODE stat: 0: Received unread; 1: Reveived read; 2: stored unsend; 3: stored send; 4: All message.
 * @return _VOID
 */
_VOID WaveC_WmAt0705_ListSMS(_CHAR8* ATCmd, _UINT32* size, RIL_SMS_MODE stat)
{
    strcpy(ATCmd, "AT+CMGL=");
    switch(stat) {
    case RIL_RECUNREAD:
        strcat(ATCmd, "0");
        break;
    case RIL_RECREAD:
        strcat(ATCmd, "1");
        break;
    case RIL_STOUNSENT:
        strcat(ATCmd, "2");
        break;
    case RIL_STOSENT:
        strcat(ATCmd, "3");
        break;
    case RIL_ALL:
        strcat(ATCmd, "4");
        break;
    default:
        break;
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Read SMS message
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strIndex: SMS index
 * @return _VOID
 */
_VOID WaveC_WmAt0705_ReadSMS(_CHAR8* ATCmd, _UINT32* size, _USHORT16 index)
{
    _CHAR8 temp[4];

       strcpy(ATCmd, "AT+CMGR=");
    Utl_Itoa(index, temp, 10);
    strcat(ATCmd, temp);
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

_VOID MC323_WmAt0705_ReadSMS(_CHAR8* ATCmd, _UINT32* size, _USHORT16 index)
{
    _CHAR8 temp[4];

    strcpy(ATCmd, "AT^HCMGR=");
    Utl_Itoa(index, temp, 10);
    strcat(ATCmd, temp);
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Send SMS in PDU mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strLength: SMS length
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SendSmsByPduMode(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strLength)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMGS=", strLength);
} /* end WaveC_WmAt0705_SendSmsByPduMode */

_VOID MC323_WmAt0705_SendSmsByPduMode(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strLength)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT^HCMGS=", strLength);
} /* end MC323_WmAt0705_SendSmsByPduMode */

/**
 * @brief Send SMS in PDU mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strLength: SMS length
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SendSmsByPduMode_CDMA(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMGS=,", strPDU);

    if (NULL != strPDU)
    {
        sprintf(ATCmd, "AT+CMGS=,\"%s\"", strPDU);
        strcat(ATCmd, ATCMD_RETURN);
        *size = (_USHORT16)strlen(ATCmd);
    }
    else
    {
        *size = 0;
        return;
    }
    WIRELESS_INFO("ATCmd :%s,(%s)(%d)\r\n", ATCmd, __FILE__, __LINE__);
} /* end WaveC_WmAt0705_SendSmsByPduMode */

/**
 * @brief Send SMS in TXT mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strLength: SMS length
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SendSmsByTxtMode(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU)
{    
    if (NULL != strPDU)
    {
        sprintf(ATCmd, "AT+CMGS=\"%s\"", strPDU);
       // strcat(ATCmd, "\x01a");
       strcat(ATCmd, ATCMD_RETURN);
        *size = (_USHORT16)strlen(ATCmd);
    }
    else
    {
        *size = 0;
        return;
    }
    WIRELESS_INFO("WaveC_WmAt0705_SendSmsByTxtMode:%s\r\n", ATCmd);
} /* end WaveC_WmAt0705_SendSmsByPduMode */


/**
 * @brief Save SMS in PDU mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strLength: SMS length
 * @param _UCHAR8 read_flag:0:unread sms,1:read sms
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SaveSmsByPduMode(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strLength, _UCHAR8 read_flag)
{
    _CHAR8  strDest[10];
    strcpy(ATCmd, "AT+CMGW=");
    strcat(ATCmd, strLength);
    strcat(ATCmd, ",");
    Utl_Itoa(read_flag, strDest, 10);
    strcat(ATCmd, strDest);          /* 0-rec unread, 1-rec read */
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0705_SaveSmsByPduMode */

/**
 * @brief Send PDU data
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strPDU: PUD data
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SendSmsPduData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU)
{   
    strcpy(ATCmd, strPDU);
    Utl_StrCatChr(ATCmd, AT_CONTROL_Z, 1);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0705_SendSmsPduData */

_VOID WaveC_WmAt0705_SendQiRawData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strData, _UINT32 len)
{  
    if(len < MAX_PDU_LEN)
    {
        memcpy(ATCmd, strData, len);
        *size = len;
    }
    else
    {
        memcpy(ATCmd, strData, MAX_PDU_LEN);
        *size = MAX_PDU_LEN;
    }
} /* end WaveC_WmAt0705_SendQiRawData */

_VOID WaveC_WmAt0705_SendSmsTxtData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU, _UINT32 uiLen)
{   
    //strcpy(ATCmd, strPDU);
    memcpy(ATCmd, strPDU, uiLen);
    ATCmd[uiLen] = AT_CONTROL_Z;
   // Utl_StrCatChr(ATCmd, AT_CONTROL_Z, 1);
    *size = uiLen + 1;
} /* end WaveC_WmAt0705_SendSmsPduData */

/**
 * @brief New SMS message indications
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: mode+mt+bm+ds+bfr
 * @return _VOID
 */
_VOID WaveC_WmAt0705_NewSMSIndication(_CHAR8* ATCmd, _UINT32* size, RIL_STORENEWMSG_MODE mode)
{
    if(RIL_STORENEWMSG_SIMFIRST == mode)
    {
        /*
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNMI=", "1,1,2,1");
        */
        
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNMI=", "2,1");
    }
    else if(RIL_STORENEWMSG_MEFIRST == mode)
    {
        #if 0
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNMI=", "2,2,2,1");
        #else
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNMI=", "3,2,0,0,0");
        #endif
    }
    else if(RIL_STORENEWMSG_MEFIRST_RPT == mode)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNMI=", "3,2,0,1,0");
    }    
}

/**
 * @brief New SMS message acknowledge to ME/TE, only phase 2+
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0705_NewSMSAck(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNMA", NULL);
} /* end WaveC_WmAt0705_NewSMSAck */

/**
 * @brief Inquire message storage
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param RIL_MSG_STORAGE storage:
 * @return _VOID
 */
_VOID WaveC_WmAt0705_InquireSmsStorage(_CHAR8* ATCmd, _UINT32* size, RIL_MSG_STORAGE storage)
{
    strcpy(ATCmd, "AT+CPMS=");

    switch(storage)
    {
    case RIL_MSG_MEM_SM:
        strcat(ATCmd, "\"SM\"");
        break;
    case RIL_MSG_MEM_ME:
        strcat(ATCmd, "\"ME\"");
        break;
    case RIL_MSG_MEM_MT:
        strcat(ATCmd, "\"MT\"");
        break;
    case RIL_MSG_MEM_BM:
        strcat(ATCmd, "\"BM\"");
        break;
    default:
        break;
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* WaveC_WmAt0705_InquireSmsStorage */

/**
 * @brief Request SMS server centre address
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0705_InquireSmsCenter(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSCA?", NULL);

} /* end WaveC_WmAt0705_InquireSmsCenter */

/**
 * @brief Set SMS server centre address
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strSca: Service centre address
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SetSmsCenter(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strSca)
{
    strcpy(ATCmd, "AT+CSCA=");
    strcat(ATCmd, "\"");
    strcat(ATCmd, strSca);
    strcat(ATCmd, "\"");

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0705_SetSmsCenter */

/**
 * @brief Inquire cell broadcast messages status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0705_InquireBroadcast(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSCB?", NULL);
} /* end WaveC_WmAt0705_InquireBroadcast */

/**
 * @brief Select cell broadcast messages
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: Accepts messages or not, 0: accept
 * @param _CHAR8* strMids: Combinations of CBM message IDs
 * @param _CHAR8* strDcss: Combinations of CBM data coding schemes
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SelectBroadcast(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMode, const _CHAR8* strMids, const _CHAR8* strDcss)
{
    strcpy(ATCmd, "AT+CSCB=");
    if (NULL != strMode)
    {
        strcat(ATCmd, strMode);
        if (NULL != strMids)
        {
            strcat(ATCmd, ",\"");
            strcat(ATCmd, strMids);
            strcat(ATCmd, "\"");
            /*if (NULL != strDcss)
            {
                strcat(ATCmd, ",\"");
                strcat(ATCmd, strDcss);
                strcat(ATCmd, "\"");
            }*/
        }
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0705_SelectBroadcast */

/**
 * @brief Select message service
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strService: GMS service
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SelectMsgService(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strService)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSMS=", strService);    
} /* end WaveC_WmAt0705_SelectMsgService */



/**
 * @brief Switch from data mode or PPP online mode to command mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SwitchDataToAtMode(_CHAR8* ATCmd, _UINT32* size)
{
    strcpy(ATCmd, "+++");
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAtV25_SwitchDataToAtMode */

/**
 * @brief Send AT
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_Attention(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
} /* end WaveC_WmAt0705_Attention */

/**
 * @brief Answer a call
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_AnswerCall(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "ATA", NULL);
} /* end WaveC_WmAtV25_AnswerCall */

/**
 * @brief Answer a call
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID CM300_WmAtV25_AnswerCall(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$QCCAV", NULL);
} /* end WaveC_WmAtV25_AnswerCall */

/**
 * @brief Mobile originated call to dial a number
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strNumber: telephone number
 * @param RIL_DATA_CLASS eClass: class
 * @return _USHORT16*
 */
_VOID WaveC_WmAtV25_DialCall(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{
    strcpy(ATCmd, "ATD");
    if (NULL != strNumber)
    {
        strcat(ATCmd, strNumber);
        if (RIL_dcVOICE == eClass)    /* send ATD with ';' only when the classes is VOICE */
        {
            strcat(ATCmd, ";");
        }
    }
    
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAtV25_DialCall */

/**
 * @brief Mobile originated call to dial a number
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strNumber: telephone number
 * @param RIL_DATA_CLASS eClass: class
 * @return _USHORT16*
 */
_VOID C202_WmAtV25_DialCall(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{

    strcpy(ATCmd, "AT+CDV=");
    if (NULL != strNumber)
    {
        strcat(ATCmd, strNumber);
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end C202_WmAtV25_DialCall */

/**
 * @brief Mobile originated call to dial a number
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strNumber: telephone number
 * @param RIL_DATA_CLASS eClass: class
 * @return _USHORT16*
 */
_VOID CM300_WmAtV25_DialCall(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{

    strcpy(ATCmd, "AT+CDV");
    if (NULL != strNumber)
    {
        strcat(ATCmd, strNumber);
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end CM300_WmAtV25_DialCall */


/**
 * @brief Enable/Disable command echo
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: 0: off, 1: on
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SetCommandEcho(_CHAR8* ATCmd, _UINT32* size, _BOOL bOnOff)
{
    if(bOnOff)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "ATE", "1");
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "ATE", "0");
    }
} /* end WaveC_WmAtV25_SetCommandEcho */

/**
 * @brief Disconnect existing connection
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_DisconnectCall(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "ATH", NULL);
} /* end WaveC_WmAtV25_DisconnectCall */

/**
 * @brief Disconnect existing connection
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID C202_WmAtV25_DisconnectCall(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CHV", NULL);
} /* end C202_WmAtV25_DisconnectCall */

/**
 * @brief Display product/additional identification information
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: value
 * @return _VOID
 */
_VOID WaveC_WmAtV25_GetProductID(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XGENDATA", strValue);
} /* end WaveC_WmAtV25_GetProductID */

/**
 * @brief Switch from command mode to data mode / PPP online mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SwitchAtToDataMode(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "ATO", NULL);
} /* end WaveC_WmAtV25_SwitchAtToDataMode */

/**
 * @brief Set result code presentation mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: 0: DCE transmits result code, 1: off
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SetResultPresentation(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "ATQ", strValue);
} /* end WaveC_WmAtV25_SetResultPresentation */

/**
 * @brief Set result code format mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: 0: response with numeric code, 1: verbose code
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SetResultFormat(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "ATV", strValue);
} /* end WaveC_SetResultFormat */

/**
 * @brief Set module to manufacture default parameter
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SetManufDefault(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT&F", NULL);
} /* end WaveC_SetManufDefault */

_VOID M10_WmAtV25_InquireCCID(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QCCID", NULL);
} /* end M10_WmAtV25_InquireCCID */

_VOID C202_WmAtV25_InquireCCID(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CCID?", NULL);
} /* end C202_WmAtV25_InquireCCID */

_VOID M10_WmAtV25_InquireIMSI(_CHAR8* ATCmd, _UINT32* size)
{
    /*WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CRSM=176,28423,0,0,9", NULL);*/
    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIMI", NULL);
}

/**
 * @brief Request TA serial number identification(IMEI)
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtV25_InquireIMEI(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGSN", NULL);
} /* end WaveC_InquireIMEI */

_VOID M10_WmAtV25_InquireIMEI(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+EGMR=0,7", NULL);
} /* end M10_WmAtV25_InquireIMEI */

_VOID C202_WmAtV25_InquireESN(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+GESN?", NULL);
} /* end C202_WmAtV25_InquireESN */

_VOID CM300_WmAtV25_InquireESN(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+GSN", NULL);
} /* end CM300_WmAtV25_InquireESN */


_VOID WaveC_WmAtV25_SetIMEI(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strIMEI)
{
    _UCHAR8 ucBuff[128];

    memset(ucBuff, 0, sizeof(ucBuff));
    sprintf(ucBuff, "AT+EGMR=1,7,\"%s\"",strIMEI);

    WaveC_Wm_SendStandardAT(ATCmd, size, ucBuff, NULL);
} /* end WaveC_SetIMEI */

_VOID WaveC_WmAtV25_LockBS(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strBS)
{
    _UCHAR8 ucBuff[128];
    _UINT32 uiBSIndex = 0;

    memset(ucBuff, 0, sizeof(ucBuff));

    if(NULL == strBS)
    {
        sprintf(ucBuff, "AT+QLOCKF=0");
    }
    else
    {
        uiBSIndex = atoi(strBS);
        sprintf(ucBuff, "AT+QLOCKF=1,2,%s", strBS);
    }

    WaveC_Wm_SendStandardAT(ATCmd, size, ucBuff, NULL);
} /* end WaveC_WmAtV25_LockBS */


/**
 * @brief Set fixed local rate
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strRate: baudrate
 * @return _VOID
 */
_VOID WaveC_WmAtV25_SetBaudrate(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strRate)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+IPR=", strRate);
} /* end WaveC_SetBaudrate */

/**
 * @brief Set Band
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: baudrate
 * @return _VOID
 */
_VOID WaveC_WmAtV25_InquireBand(_CHAR8* ATCmd, _UINT32* size)
{    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XBANDSEL?", NULL);    
}/*end of WaveC_InquireBand();*/


_VOID WaveC_WmAtV25_SetNetBand(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMode)
{
    _USHORT16 iLength;
    iLength = (_USHORT16)strlen(strMode);

    if(Utl_StrFndL(strMode, "EGSM", 0, iLength) != -1)
    {
           WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XBANDSEL=900", NULL);    
    }
    else if(Utl_StrFndL(strMode, "DCS", 0, iLength) != -1)
    {
           WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XBANDSEL=1800", NULL);    
    }
    else if(Utl_StrFndL(strMode, "DUAL", 0, iLength) != -1)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XBANDSEL=900, 1800", NULL);    
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
    }
}/* end WaveC_SetNetBand */

/**
 * @brief Inquire the current GPRS service state
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtGprs_InquireGprsState(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGATT?", NULL);
}/* end WaveC_WmAtGprs_InquireGprsState */

/**
 * @brief GPRS attach and detach
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strState: 0: detach(ed) (power-on default), 1: attach(ed)
 * @return _VOID
 */
_VOID WaveC_WmAtGprs_GprsAttachDetach(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strState)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGATT=", strState);
}/* end WaveC_WmAtGprs_GprsAttachDetach */

/**
 * @brief Define PDP Context
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strCid: specifies a particular PDP context definition
 * @param _CHAR8* strPdpType: IP: Internet Protocol
 * @param _CHAR8* strAPN: Access Point Name
 * @param _CHAR8* strPdpAddr: identifies the MT in the address space applicable to the PDP
 * @return _VOID
 */
_VOID WaveC_WmAtGprs_DefinePDPContext(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strCid, const _CHAR8* strPdpType, const _CHAR8* strAPN, const _CHAR8* strPdpAddr)
{
    strcpy(ATCmd, "AT+CGDCONT=");
    if (NULL != strCid)
    {
        strcat(ATCmd, strCid);
        if (NULL != strPdpType)
        {
            strcat(ATCmd, ",\"");
            strcat(ATCmd, strPdpType);
            strcat(ATCmd, "\"");
            if (NULL != strAPN)
            {
                strcat(ATCmd, ",\"");
                strcat(ATCmd, strAPN);
                strcat(ATCmd, "\"");
                if (NULL != strPdpAddr)
                {
                    strcat(ATCmd, ",\"");
                    strcat(ATCmd, strPdpAddr);
                    strcat(ATCmd, "\"");
                }
            }
        }
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Quality of Service Profile
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: parameters
 * @return _VOID
 */
_VOID WaveC_WmAtGprs_QualityOfService(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGQREQ=", strParam);
}

/**
 * @brief Request GPRS service
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtGprs_RequestGprsService(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "ATD*99***1#", NULL);
}


/**
 * @brief Get the current ACM(accumulated call meter) value
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_GetCurrentAcm(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CACM?", NULL);
} /* end WaveC_WmAt0707_GetCurrentAcm */

/**
 * @brief Resets the Advice of Charge related to the ACM value in SIM file EF
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strPasswd: SIM PIN2
 * @return _VOID
 */
_VOID WaveC_WmAt0707_ResetAcm(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPasswd)
{
    strcpy(ATCmd, "AT+CACM=");
    if (NULL != strPasswd)
    {
        strcat(ATCmd, "\"");
        strcat(ATCmd, strPasswd);
        strcat(ATCmd, "\"");
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_ResetAcm */

/**
 * @brief Get ACMmax(accumulated call meter maximum) value
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_GetAcmMax(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CAMM?", NULL);
} /* end WaveC_WmAt0707_GetAcmMax */

/**
 * @brief Sets the Advice of Charge related to the ACMmax value in SIM file EF
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strAcmax: Three bytes of the max. ACM value in hexadecimal format
 * @param _CHAR8* strPasswd: SIM PIN2
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetAcmMax(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strAcmax, const _CHAR8* strPasswd)
{
    strcpy(ATCmd, "AT+CAMM=");
    if (NULL != strAcmax)
    {
        strcat(ATCmd, "\"");
        strcat(ATCmd, strAcmax);
        strcat(ATCmd, "\"");
        if (NULL != strPasswd)
        {
            strcat(ATCmd, ",\"");
            strcat(ATCmd, strPasswd);
            strcat(ATCmd, "\"");
        }
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SetAcmMax */

/**
 * @brief Get the current call meter value
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_GetCurCallMeter(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CAOC?", NULL);
} /* end WaveC_WmAt0707_GetCurCallMeter */

/**
 * @brief Set or inquire call forward
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param AK_CCFC_FWDCALLS *cfData: call forward setting data
 * @return _VOID
 */
_VOID WaveC_WmAt0707_CallForward(_CHAR8* ATCmd, _UINT32* size, const RIL_CCFC_FWDCALLS *cfData)
{
    _CHAR8   strPart[10];
    _SHORT16  iLength, i;

    strcpy(ATCmd, "AT+CCFC=");
    strcat(ATCmd, Utl_Itoa(cfData->Reason, strPart, 10));/* reason*/

    strcat(ATCmd, ",");
    strcat(ATCmd, Utl_Itoa(cfData->Mode, strPart, 10));/* mode*/

    if(cfData->Number[0] != 0) /* phone number*/
    {
        strcat(ATCmd, ",");
        strcat(ATCmd, "\"");
        strcat(ATCmd, cfData->Number);
        strcat(ATCmd, "\"");
    }

    #if 0
    strcat(ATCmd, ",");
    if(cfData->Type != -1)
    {
        if(cfData->Number[0]=='+')
        {
            strcat(ATCmd, "145");
        }
        else if(cfData->Type == RIL_INTER) /* international number*/
        {
            strcat(ATCmd, "145");
        }
        else
        {
            strcat(ATCmd, "129");/* local number*/
        }
    }

    strcat(ATCmd, ",");
    if(cfData->Class != -1)/* class*/
    {
        Utl_Itoa(cfData->Class, strPart, 10);
        strcat(ATCmd, strPart);
    }

    strcat(ATCmd, ",");
    if(cfData->Time != -1)/* time*/
    {
        strcat(ATCmd, ",,");
        Utl_Itoa(cfData->Time, strPart, 10);
        strcat(ATCmd, strPart);
    }

    iLength = strlen(ATCmd);
    for(i = iLength; i > 0; i--)
    {
        if(ATCmd[i-1] == ',')
        {
            ATCmd[i-1] = 0;
        }
        else
        {
            break;
        }
    }
    #endif
    
    strcat(ATCmd, ATCMD_RETURN);
     *size = (_USHORT16)strlen(ATCmd);
} /* WaveC_WmAt0707_CallForward */

/**
 * @brief Inquire call forward
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireCallFwd(_CHAR8* ATCmd, _UINT32* size, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class)
{
    _CHAR8        strPart[10];

    strcpy(ATCmd, "AT+CCFC=");
    strcat(ATCmd, Utl_Itoa(reason, strPart, 10));/*reason*/

    strcat(ATCmd, ",2,,,");
    strcat(ATCmd, Utl_Itoa(Class, strPart, 10));

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Inquire Real Time Clock
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireClock(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CCLK?", NULL);
} /* end WaveC_WmAt0707_InquireClock */

/**
 * @brief Set Real Time Clock
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetClock(_CHAR8* ATCmd, _UINT32* size, const RIL_SYSTIME *SysTime)
{
    _CHAR8 strData[12], strTime[12];
    
    ConvertTimeSToStr(SysTime, strData, strTime);

    strcpy(ATCmd, "AT+CCLK=\"");
    strcat(ATCmd, strData);
    strcat(ATCmd, ",");
    strcat(ATCmd, strTime);

    strcat(ATCmd, "+00");        /*infineon,merge*/
    strcat(ATCmd, "\"");
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SetClock */

/**
 * @brief Set or inquire call waiting
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _BOOL bURCEnable: Sets/shows the presentation mode of URC, 0: disable
 * @param _BOOL bEnable: 0: disable, 1: enable
 * @param RIL_DATA_CLASS eClass: class
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetCallWait(_CHAR8* ATCmd, _UINT32* size, _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass)
{
    strcpy(ATCmd, "AT+CCWA=");

    if(bURCEnable)
    {
        strcat(ATCmd, "1");
    }
    else
    {
        strcat(ATCmd, "0");
    }

    if(bEnable)
    {
        strcat(ATCmd, ",1");
    }
    else
    {
        strcat(ATCmd, ",0");
    }

    if(RIL_dcNULL != eClass)
    {
        T_STR_100    strTemp;

        strcat(ATCmd, ",");
        Utl_Itoa(eClass, strTemp, 10);
        strcat(ATCmd, strTemp);
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Inquire call forward
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireCallWait(_CHAR8* ATCmd, _UINT32* size, RIL_DATA_CLASS eClass)
{
    strcpy(ATCmd, "AT+CCWA=1,2");

    if(RIL_dcNULL != eClass)
    {
        T_STR_100    strTemp;

        strcat(ATCmd, ",");
        Utl_Itoa(eClass, strTemp, 10);
        strcat(ATCmd, strTemp);
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}


/**
 * @brief Get extended error report
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_GetExtendedError(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CEER", NULL);
} /* end WaveC_WmAt0707_GetExtendedError */

/** 
 * @brief Set phone functionality.
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strFun: functionality
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetFunctionality(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strFun)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CFUN=", strFun);
} /* end WaveC_WmAt0707_SetFunctionality */

_VOID WaveC_WmAt0707_SetQSIMVOL(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMod)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QSIMVOL=", strMod);
}

_VOID M35_WmAt0707_SetRingBack(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QCFG=\"CALL/ProgDescRBT\",", strEn);
}

_VOID UC15_WmAt0707_SetVBatt(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QCFG=\"vbatt\",", strEn);
}

_VOID EC20_WmAt0707_SetNWScanMode(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QCFG=\"nwscanmode\",", strStr);
}

_VOID WaveC_WmAt0705_SetOpenTCPUDP(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIOPEN=", strStr);
}

_VOID WaveC_WmAt0705_SetQindi(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QINDI=", strParam);
}

_VOID WaveC_WmAt0705_SetQiDnsip(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIDNSIP=", strParam);
}

_VOID WaveC_WmAt0705_SendQiData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strLength)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QISEND=", strLength);
}

_VOID WaveC_WmAt0705_InquireQiStat(_CHAR8* ATCmd, _UINT32* size)
{
      WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QISTAT", NULL);
}

_VOID WaveC_WmAt0705_InquireQiSack(_CHAR8* ATCmd, _UINT32* size)
{
      WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QISACK", NULL);
}

_VOID WaveC_WmAt0705_InquireQiLocIP(_CHAR8* ATCmd, _UINT32* size)
{
      WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QILOCIP", NULL);
}

_VOID WaveC_WmAt0705_InquireQiRcvData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
      WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIRD=0,1,0,", strParam);
}

_VOID WaveC_WmAt0705_SetQiClose(_CHAR8* ATCmd, _UINT32* size)
{
      WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QICLOSE", NULL);
}

_VOID WaveC_WmAt0707_InquireQIRegApp(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIREGAPP?", NULL);
} 

_VOID WaveC_WmAt0707_SetQIRegApp(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIREGAPP=", strStr);
} 

_VOID SIM800_WmAt0707_SetAPN(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSTT=", strStr);
} 

_VOID WaveC_WmAt0707_SetQICsgp(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QICSGP=", strStr);
} 

_VOID WaveC_WmAt0707_SetQIAct(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIACT", NULL);
} 

_VOID SIM800_WmAt0707_SetCIICR(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIICR", NULL);
} 

_VOID WaveC_WmAt0707_SetQIDeact(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QIDEACT", NULL);
} 

_VOID SIM800_WmAtV25_SetCipStart(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPSTART=", strStr);
}

_VOID SIM800_WmAtV25_SendCipData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strLength)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPSEND=", strLength);
}

_VOID SIM800_WmAtV25_InquireCipStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPSTATUS", NULL);
}

_VOID SIM800_WmAtV25_InquireCipAck(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPACK", NULL);
}

_VOID SIM800_WmAtV25_SetCipRxget(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPRXGET=", strParam);
}

_VOID SIM800_WmAtV25_InquireCipRxget(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPRXGET=2,", strParam);
}

_VOID SIM800_WmAtV25_SetCipClose(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPCLOSE", NULL);
}

_VOID SIM800_WmAtV25_SetCipShut(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIPSHUT", NULL);
}

_VOID SIM800_WmAtV25_InquireQiLocIP(_CHAR8* ATCmd, _UINT32* size)
{
      WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIFSR", NULL);
}

_VOID SIM800_WmAtV25_InquireCCID(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CCID", NULL);
}

_VOID SIM800_WmAtV25_SetCnetScan(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNETSCAN=", strParam);
}

_VOID SIM800_WmAtV25_InquireCnetScan(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CNETSCAN", NULL);
}

_VOID UC15_WmAt0707_SetQFun(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strFun)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QFUN=", strFun);
}

_VOID WaveC_WmAt0707_GetFunctionality(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CFUN?", NULL);
} 

_VOID WaveC_WmAt0707_GetQSIMVOL(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QSIMVOL?", NULL);
} 

_VOID C202_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+RESET", NULL);
}

_VOID MC323_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT^RESET", NULL);
}

_VOID M10_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CFUN=1,1", NULL);
}

_VOID L810_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CFUN=15", NULL);
}


_VOID WaveC_WmAt0707_SetQMoStat(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QMOSTAT=", strEn);
} /* end WaveC_WmAt0707_SetQMoStat */

_VOID WaveC_WmAt0707_SetQCCINFOEn(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn)
{
    sprintf(ATCmd, "AT+QINDCFG=\"CCINFO\",%s,1", strEn);
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SetQMoStat */

_VOID WaveC_WmAt0707_SetQURCCFG(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QURCCFG=\"urcport\",\"uart1\"", NULL);
} /* end WaveC_WmAt0707_SetQURCCFG */


_VOID WaveC_WmAt0707_SetQJamDet(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QLJDC=", strEn);
} /* end WaveC_WmAt0707_SetQJamDet */

/**
 * @brief Call hold and multiparty
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* sValue: value
 * @return _VOID
 */
_VOID WaveC_WmAt0707_HoldAndMultiparty(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* sValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CHLD=", sValue);
} /* end WaveC_WmAt0707_HoldAndMultiparty */

/**
 * @brief Inquire international mobile subscriber identity
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireSubscriber(_CHAR8* ATCmd, _UINT32* size)
{
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIMI", NULL);
#else
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIMI?", NULL);  //weikk add 2016.2.24.
#endif
} /* end WaveC_WmAt0707_InquireSubscriber */

/**
 * @brief List current calls of ME
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLCC", NULL);
} /* end WaveC_WmAt0707_ListCurCalls */

/**
 * @brief List current calls of ME
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID C202_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLCC?", NULL);
} /* end C202_WmAt0707_ListCurCalls */

/**
 * @brief List current calls of ME
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID MC323_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLCC", NULL);
} /* end MC323_WmAt0707_ListCurCalls */


/**
 * @brief List current calls of ME
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID CM300_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$HSTATE", NULL);
} /* end MC323_WmAt0707_ListCurCalls */


/**
 * @brief Facility lock
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param RIL_FACILITY fac: Facility
 * @param mode: 0: unlock, 1: lock, 2: query status
 * @param _CHAR8* strPasswd: password
 * @param RIL_DATA_CLASS eClass: class
 * @return _VOID
 */
_VOID WaveC_WmAt0707_FacilityLock(_CHAR8* ATCmd, _UINT32* size, RIL_FACILITY fac, RIL_CLCK_MODE mode, const _CHAR8* strPasswd, RIL_DATA_CLASS eClass)
{
    _CHAR8 strFac[8], strMode[4];

    switch(fac) {
    case RIL_SC:
        strcpy(strFac, "SC");
        break;
    case RIL_PS:
        strcpy(strFac, "PS");
        break;
    case RIL_FD:
        strcpy(strFac, "FD");
        break;
    case RIL_PF:
        strcpy(strFac, "PF");
        break;
    case RIL_PN:
        strcpy(strFac, "PN");
        break;
    case RIL_PU:
        strcpy(strFac, "PU");
        break;
    case RIL_PP:
        strcpy(strFac, "PP");
        break;
    case RIL_PC:
        strcpy(strFac, "PC");
        break;
    case RIL_P2:
        strcpy(strFac, "P2");
        break;
    case RIL_AO:
        strcpy(strFac, "AO");
        break;
    case RIL_OI:
        strcpy(strFac, "OI");
        break;
    case RIL_OX:
        strcpy(strFac, "OX");
        break;
    case RIL_AI:
        strcpy(strFac, "AI");
        break;
    case RIL_IR:
        strcpy(strFac, "IR");
        break;
    case RIL_AB:
        strcpy(strFac, "AB");
        break;
    case RIL_AG:
        strcpy(strFac, "AG");
        break;
    case RIL_AC:
        strcpy(strFac, "AC");
        break;
    default:
        *size = 0;
        return;
    }

    switch(mode) {
    case RIL_CLCK_Unlock:
        strcpy(strMode, "0");
        break;
    case RIL_CLCK_Lock:
        strcpy(strMode, "1");
        break;
    case RIL_CLCK_Query:
        strcpy(strMode, "2");
        break;
    default:
        *size = 0;
        return;
    }

    strcpy(ATCmd, "AT+CLCK=");
    strcat(ATCmd, "\"");
    strcat(ATCmd, strFac);
    strcat(ATCmd, "\",");
    strcat(ATCmd, strMode);

    if ((NULL != strPasswd) && (strPasswd[0] != 0))
    {
        strcat(ATCmd, ",\"");
        strcat(ATCmd, strPasswd);
        strcat(ATCmd, "\"");
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_FacilityLock */

/**
 * @brief Calling line identification presentation
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: 1: display URC, 0: suppress
 * @return _VOID
 */
_VOID WaveC_WmAt0707_CallingLineId(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLIP=", strValue);
} /* end WaveC_WmAt0707_CallingLineId */

/**
 * @brief Check the state of Calling line identification presentation
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAT0707_CheckCLIP(_CHAR8* ATCmd, _UINT32* size)        
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLIP?", NULL);    
}/* end WaveC_WmAT0707_CheckCLIP */


/**
 * @brief Check the state of Calling line identification restriction
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAT0707_CheckCLIR(_CHAR8* ATCmd, _UINT32* size)    
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLIR?", NULL);    
}/* end the WaveC_WmAT0707_CheckCLIR */

/**
 * @brief Check the state of Calling line identification restriction
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAT0707_SetCLIRActive(_CHAR8* ATCmd, _UINT32* size)    
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLIR=1", NULL);    
}/* end the WaveC_WmAT0707_SetCLIRActive */

/**
 * @brief Check the state of Calling line identification restriction
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAT0707_SetCLIRDisable(_CHAR8* ATCmd, _UINT32* size)    
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLIR=2", NULL);    
}/* end the WaveC_WmAT0707_SetCLIRDisable */


/**
 * @brief Dailcall without ";"
 * dail supplement service number
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue
 * @return _VOID
 */
_VOID WaveC_WmAT0707_DailCLIR(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strValue)    
{
    strcpy(ATCmd, "ATD");
    strcat(ATCmd, strValue);

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}


/**
 * @brief Check the state of COLR
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAT0707_CheckCOLR(_CHAR8* ATCmd, _UINT32* size)            
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+COLR?", NULL);
} /* end WaveC_WmAT0707_CheckCOLR  */


/**
 * @brief Set loudspeaker volume level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 volume: volume level
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;
       _USHORT16    TabloudLevelToVolum[2][MAX_VOLUME_LEVEL+1] = 
    {{ 0, 1,  2,  3,  4,  5,  6,  7   },
    {  64, 80, 96, 112, 128, 144, 160, 176 }};
    _USHORT16            i;

    strcpy(ATCmd, "AT+VGR="); 
    for (i = 0; i <= MAX_VOLUME_LEVEL; i++)
    {
        if (volume == TabloudLevelToVolum[0][i])
        {
            Utl_Itoa(TabloudLevelToVolum[1][i], strTemp, 10);
            strcat(ATCmd, strTemp);
            strcat(ATCmd, ATCMD_RETURN);
            *size = (_USHORT16)strlen(ATCmd);
            return;
        }
    }
    *size = 0;    
} /* end WaveC_WmAt0707_SetSpeakerVol */

_VOID M10_WmAt0707_SetDualSIM(_CHAR8* ATCmd, _UINT32* size, _SHORT16 simslot)
{
    T_STR_20        strTemp;

    strcpy(ATCmd, "AT+QDSIM="); 
    if(0 == simslot)
    {
        strcat(ATCmd, "0,0");
    }
    else
    {
        strcat(ATCmd, "1,0");
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
    return;
}

/**
 * @brief Set loudspeaker volume level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 volume: volume level
 * @return _VOID
 */
 #if ((ACOM_PORT_NUM == 16) || (ACOM_PORT_NUM == 32))
_VOID M10_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;
       _USHORT16    TabloudLevelToVolum[2][MAX_VOLUME_LEVEL+1] = 
    {{ 0,   1,  2,  3,   4,   5,   6,  7   },
    {  15, 20, 25,  30,  35,  40,  45, 50 }};
    _USHORT16            i;

    strcpy(ATCmd, "AT+CLVL="); 
    for (i = 0; i <= MAX_VOLUME_LEVEL; i++)
    {
        if (volume == TabloudLevelToVolum[0][i])
        {
            Utl_Itoa(TabloudLevelToVolum[1][i], strTemp, 10);
            strcat(ATCmd, strTemp);
            strcat(ATCmd, ATCMD_RETURN);
            *size = (_USHORT16)strlen(ATCmd);
            return;
        }
    }
    *size = 0;    
} /* end M10_WmAt0707_SetSpeakerVol */
#else
_VOID M10_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;
       _USHORT16    TabloudLevelToVolum[2][MAX_VOLUME_LEVEL+1] = 
    {{ 0,   1,  2,  3,   4,   5,   6,  7   },
    {  4,   10, 15,  25,  30,  35, 40, 45 }};
    _USHORT16            i;

    strcpy(ATCmd, "AT+CLVL="); 
    for (i = 0; i <= MAX_VOLUME_LEVEL; i++)
    {
        if (volume == TabloudLevelToVolum[0][i])
        {
            Utl_Itoa(TabloudLevelToVolum[1][i], strTemp, 10);
            strcat(ATCmd, strTemp);
            strcat(ATCmd, ATCMD_RETURN);
            *size = (_USHORT16)strlen(ATCmd);
            return;
        }
    }
    *size = 0;    
} /* end M10_WmAt0707_SetSpeakerVol */
#endif

_VOID UC15_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;
       _USHORT16    TabloudLevelToVolum[2][MAX_VOLUME_LEVEL+1] = 
    {{ 0,   1,  2,  3,   4,   5,   6,  7   },
     { 0,   1,  2,  3,   4,   5,   6,  7   }};
    _USHORT16            i;

    strcpy(ATCmd, "AT+CLVL="); 
    for (i = 0; i <= MAX_VOLUME_LEVEL; i++)
    {
        if (volume == TabloudLevelToVolum[0][i])
        {
            Utl_Itoa(TabloudLevelToVolum[1][i], strTemp, 10);
            strcat(ATCmd, strTemp);
            strcat(ATCmd, ATCMD_RETURN);
            *size = (_USHORT16)strlen(ATCmd);
            return;
        }
    }
    *size = 0;    
}



_VOID M10_WmAt0707_SetFreqBand(_CHAR8* ATCmd, _UINT32* size, _SHORT16 bandindex)
{
    _UCHAR8 FreqBandMap[6][64] = {"\"GSM850_EGSM_DCS_PCS_MODE\"",
                                      "\"EGSM_MODE\"",
                                      "\"DCS_MODE\"",
                                      "\"PCS_MODE\"",
                                      "\"EGSM_DCS_MODE\"",
                                      "\"GSM850_PCS_MODE\""};
    

    strcpy(ATCmd, "AT+QBAND="); 
    strcat(ATCmd, FreqBandMap[bandindex]);
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
    return;
} /* end M10_WmAt0707_SetFreqBand */

/**
 * @brief Set QSFR Param
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SetQSFRParam(_CHAR8* ATCmd, _UINT32* size, const _SHORT16 sfrindex)
{
    sprintf(ATCmd, "AT+QSFR=%d", sfrindex);

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);

} /* end WaveC_WmAt0705_SetQSFRParam */


/**
 * @brief Set loudspeaker volume level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 volume: volume level
 * @return _VOID
 */
_VOID CM300_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$HVOL=", strTemp);
} /* end CM300_WmAt0707_SetSpeakerVol */


/**
 * @brief Set loudspeaker volume level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 volume: volume level
 * @return _VOID
 */
_VOID MC323_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CLVL=", strTemp);
} /* end MC323_WmAt0707_SetSpeakerVol */


_VOID C202_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+VGR=", strTemp);
} /* end MC323_WmAt0707_SetSpeakerVol */

/**
 * @brief Report mobile equipment error
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: 0: disable result code, 1: enable, use numeric values, 2: enable, use verbose values
 * @return _VOID
 */
_VOID WaveC_WmAt0707_ReportMobileError(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMEE=", strValue);
} /* end WaveC_WmAt0707_ReportMobileError */

/**
 * @brief enable report time zone base by BTS or not
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
     _BOOL enable: 
            EOS_TRUE:enable report time zone base by BTS
             EOS_FALSE:disable report time zone base by BTS(default)    
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetTimeZoneReporting(_CHAR8* ATCmd, _UINT32* size, _BOOL enable)
{
    if (enable)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CTZR=1", NULL);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CTZR=0", NULL);
    }
    return;
} /* end WaveC_WmAt0707_SetTimeZoneReporting */

/**
 * @brief Enter multiplex mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: mode
 * @return _VOID
 */
_VOID WaveC_WmAt0707_EnterMultiplex(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMode)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMUX=", strMode);    
} /* end WaveC_WmAt0707_EnterMultiplex */

/**
 * @brief Read operator names
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_ReadOperatorNames(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+COPN", NULL);    
} /* end WaveC_WmAt0707_ReadOperatorNames */

/**
 * @brief Inquire the current mode and the currently used operator(if registered).
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+COPS?", NULL);
} /* end WaveC_WmAt0707_InquireCurOperator */

_VOID WaveC_WmAt0707_InquireBSInfo(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QENG?", NULL);
} /* end WaveC_WmAt0707_InquirBSInfo */


/**
 * @brief Inquire the current mode and the currently used operator(if registered).
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID CM300_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSS?", NULL);
} /* end CM300_WmAt0707_InquireCurOperator */


_VOID C202_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+VMCCMNC?", NULL);
} /* end C202_WmAt0707_InquireCurOperator */

_VOID MC323_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIMI", NULL);
} /* end MC323_WmAt0707_InquireCurOperator */

/**
 * @brief Inquiry all the available operators
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireAvaiOperator(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+COPS=?", NULL);
} /* end WaveC_WmAt0707_InquireAvaiOperator */

/**
 * @brief Operator selection
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: mode
 * @param _CHAR8* strFormat: format
 * @param _CHAR8* strOper: Operator
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SelectOperator(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMode, const _CHAR8* strFormat, const _CHAR8* strOper)
{
    strcpy(ATCmd, "AT+COPS=");
    strcat(ATCmd, strMode);
    if (NULL != strFormat)
    {
        strcat(ATCmd, ",");
        strcat(ATCmd, strFormat);
        if (NULL != strOper)
        {
            strcat(ATCmd, ",");
            strcat(ATCmd, "\"");
            strcat(ATCmd, strOper);
            strcat(ATCmd, "\"");
        }
    }

#if 0    /* kinson.xiao 2011-09-30 */
    /* the follow is for ignore infineon's bug */
    /* Changed for WaveC, WaveC need 2 AT_END*/
    if (Utl_StrCmp(strMode, "0") == 0 && Utl_StrCmp(strFormat, "2") == 0)
    {
        strcat(ATCmd, ATCMD_RETURN);
    }
#endif    

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
    
} /* end WaveC_WmAt0707_SelectOperator */

_VOID WaveC_WmAt0707_SetEngMode(_CHAR8* ATCmd, _UINT32* size, const _UINT32 mode, const _UINT32 dump)
{    
    sprintf(ATCmd, "AT+QENG=%d,%d", mode, dump);

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
    
}


/**
 * @brief Read current phone book entries
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 begin: location number where reading starts
 * @param _SHORT16 end: location number where reading ends. Set it as 0 if needn't
 * @return _VOID
 */
_VOID WaveC_WmAt0707_ReadPbEntries(_CHAR8* ATCmd, _UINT32* size, _SHORT16 begin, _SHORT16 end)
{
    T_STR_20        strTemp;

    strcpy(ATCmd, "AT+CPBR=");
    Utl_Itoa(begin, strTemp, 10);
    strcat(ATCmd, strTemp);
    if (0 != end)
    {
        Utl_Itoa(end, strTemp, 10);
        strcat(ATCmd, ",");
        strcat(ATCmd, strTemp);
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_ReadPbEntries */

_VOID MC323_WmAt0707_ReadPbEntries(_CHAR8* ATCmd, _UINT32* size, _SHORT16 begin, _SHORT16 end)
{
    T_STR_20        strTemp;

    strcpy(ATCmd, "AT^CPBR=");
    Utl_Itoa(begin, strTemp, 10);
    strcat(ATCmd, strTemp);
    if (0 != end)
    {
        Utl_Itoa(end, strTemp, 10);
        strcat(ATCmd, ",");
        strcat(ATCmd, strTemp);
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);

} /* end WaveC_WmAt0707_ReadPbEntries */

/**
 * @brief Inquiry currently selected storage's memory (used and total)
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquirePbStorage(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPBS?", NULL);
} /* end WaveC_WmAt0707_InquirePbStorage */

/**
 * @brief Select phone book memory storage
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strStorage: phone book storage
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetPbStorage(_CHAR8* ATCmd, _UINT32* size, RIL_PB_STORAGE storage)
{
    strcpy(ATCmd, "AT+CPBS=");
    switch(storage) {
    case RIL_sME:
        strcat(ATCmd, "\"ME\"");
        break;
    case RIL_sSIM:
        strcat(ATCmd, "\"SM\"");
        break;
    case RIL_sFD:
        strcat(ATCmd, "\"FD\"");
        break;
    case RIL_sON:
        strcat(ATCmd, "\"ON\"");
        break;
    default:
        break;
    }
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SetPbStorage */

/**
 * @brief Inquire phone book entry space.
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquirePbSpace(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPBW=?", NULL);
} /* end WaveC_WmAt0707_InquirePbSpace */

/**
 * @brief Write a phone book entry to the specific phone book "index"
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 index: If "index" equal or less than 0, then write this entry to the first free space
 * @param _CHAR8* strNumber: telephone number. Set this parameter as NULL to delete a entry.
 * @param _USHORT16 *strText: name in unicode format
 * @return _VOID
 */
_VOID WaveC_WmAt0707_WritePbEntry(_CHAR8* ATCmd, _UINT32* size, _SHORT16 index, const _CHAR8* strNumber, _USHORT16 *strText)
{
    T_STR_200        strTemp;

    strcpy(ATCmd, "AT+CPBW=");
    if (index > 0)    /* Write the entry to the appointed location */
    {
        Utl_Itoa(index, strTemp, 10);
        strcat(ATCmd, strTemp);
    }

    if (NULL != strNumber)
    {
        strcat(ATCmd, ",\"");
        strcat(ATCmd, strNumber);
        strcat(ATCmd, "\"");
        if (NULL != strText)
        {
            strcat(ATCmd, ",129,\"");
            /*ConvUnicodeToUCS2Str((_SHORT16*)strText, strTemp);*/
            strcat(ATCmd, strText);
            strcat(ATCmd, "\"");
        }
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Write a phone book entry to the specific phone book "index"
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 index: If "index" equal or less than 0, then write this entry to the first free space
 * @param _CHAR8* strNumber: telephone number. Set this parameter as NULL to delete a entry.
 * @param _USHORT16 *strText: name in unicode format
 * @return _VOID
 */
_VOID C202_WmAt0707_WritePbEntry(_CHAR8* ATCmd, _UINT32* size, _SHORT16 index, const _CHAR8* strNumber, _USHORT16 *strText)
{
    T_STR_200        strTemp;

    strcpy(ATCmd, "AT+CPBW=");
    if (index > 0)    /* Write the entry to the appointed location */
    {
        Utl_Itoa(index, strTemp, 10);
        strcat(ATCmd, strTemp);
    }

    if (NULL != strNumber)
    {
        strcat(ATCmd, ",");
        strcat(ATCmd, strNumber);
        strcat(ATCmd, "");
        if (NULL != strText)
        {
            strcat(ATCmd, ",0,\"");
            /*ConvUnicodeToUCS2Str((_SHORT16*)strText, strTemp);*/
            strcat(ATCmd, strText);
            strcat(ATCmd, "\"");
        }
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

_VOID MC323_WmAt0707_WritePbEntry(_CHAR8* ATCmd, _UINT32* size, _SHORT16 index, const _CHAR8* strNumber, _USHORT16 *strText)
{
    T_STR_200        strTemp;

    strcpy(ATCmd, "AT^CPBW=");
    if (index > 0)    /* Write the entry to the appointed location */
    {
        Utl_Itoa(index, strTemp, 10);
        strcat(ATCmd, strTemp);
    }

    if (NULL != strNumber)
    {
        strcat(ATCmd, ",");
        strcat(ATCmd, "\"");
        strcat(ATCmd, strNumber);
        strcat(ATCmd, "\"");
        strcat(ATCmd, "");
        if (NULL != strText)
        {
            strcat(ATCmd, ",129,\"");
            /*ConvUnicodeToUCS2Str((_SHORT16*)strText, strTemp);*/
            strcat(ATCmd, strText);
            strcat(ATCmd, "\"");
        }
    }

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}


/**
 * @brief Inquire PIN status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPIN?", NULL);
} /* end WaveC_WmAt0707_InquirePinStatus */


_VOID WaveC_WmAt0707_InquireInitStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QINISTAT", NULL);
}

/**
 * @brief Inquire PIN status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID C202_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPIN?", NULL);
} /* end C202_WmAt0707_InquirePinStatus */

/**
 * @brief Inquire PIN status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID MC323_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPIN?", NULL);
} /* end MC323_WmAt0707_InquirePinStatus */


/**
 * @brief Inquire PIN status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID CM300_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$HPINS=P1", NULL);
} /* end CM300_WmAt0707_InquirePinStatus */

_VOID MC600_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+ICCID", NULL);
} /* end MC600_WmAt0707_InquirePinStatus */

/**
 * @brief Enter PIN
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strPIN: password
 * @param _CHAR8* strNewPIN: if strPIN is PUK, a new PIN need to be input here
 * @return _VOID
 */
_VOID WaveC_WmAt0707_EnterPIN(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPIN, const _CHAR8* strNewPIN)
{
    strcpy(ATCmd, "AT+CPIN=\"");
    strcat(ATCmd, strPIN);
    strcat(ATCmd, "\"");
    if (NULL != strNewPIN)
    {
        strcat(ATCmd, ",\"");
        strcat(ATCmd, strNewPIN);
        strcat(ATCmd, "\"");
    }

    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_EnterPIN */

/**
 * @brief Change password
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param RIL_FACILITY fac: Facility
 * @param _CHAR8* strOldPwd: Password specified for the facility
 * @param _CHAR8* strNewPwd: Password specified for the facility
 * @return _VOID
 */
_VOID WaveC_WmAt0707_ChangePassword(_CHAR8* ATCmd, _UINT32* size, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd)
{
    _CHAR8 strFac[8];

    switch(fac) {
    case RIL_SC:
        strcpy(strFac, "SC");
        break;
    case RIL_PS:
        strcpy(strFac, "PS");
        break;
    case RIL_FD:
        strcpy(strFac, "FD");
        break;
    case RIL_PF:
        strcpy(strFac, "PF");
        break;
    case RIL_PN:
        strcpy(strFac, "PN");
        break;
    case RIL_PU:
        strcpy(strFac, "PU");
        break;
    case RIL_PP:
        strcpy(strFac, "PP");
        break;
    case RIL_PC:
        strcpy(strFac, "PC");
        break;
    case RIL_P2:
        strcpy(strFac, "P2");
        break;
    case RIL_AO:
        strcpy(strFac, "AO");
        break;
    case RIL_OI:
        strcpy(strFac, "OI");
        break;
    case RIL_OX:
        strcpy(strFac, "OX");
        break;
    case RIL_AI:
        strcpy(strFac, "AI");
        break;
    case RIL_IR:
        strcpy(strFac, "IR");
        break;
    case RIL_AB:
        strcpy(strFac, "AB");
        break;
    case RIL_AG:
        strcpy(strFac, "AG");
        break;
    case RIL_AC:
        strcpy(strFac, "AC");
        break;
    default:
        *size = 0;
        return;
    }
    strcpy(ATCmd, "AT+CPWD=\"");
    strcat(ATCmd, strFac);
    strcat(ATCmd, "\",");

    if (NULL != strOldPwd)
    {
        strcat(ATCmd, "\"");
        strcat(ATCmd, strOldPwd);
        strcat(ATCmd, "\"");
    }
    strcat(ATCmd, ",\"");
    strcat(ATCmd, strNewPwd);
    strcat(ATCmd, "\"");
    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_ChangePassword */

/**
 * @brief Network registration
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: 0: disable URCs, 1,2: enable URC
 * @return _VOID
 */
_VOID WaveC_WmAt0707_NetworkRegistration(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGREG=", strValue);
} /* end WaveC_WmAt0707_NetworkRegistration */

/**
 * @brief Inquire Network registration status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: must be null
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    //WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CREG?", NULL);    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGREG?", NULL);    //weikaikai 
} /* end WaveC_WmAt0707_NetworkRegistration */

_VOID FIBOCOM_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGREG?", NULL);
} /* end WaveC_WmAt0707_NetworkRegistration */

/**
 * @brief Inquire Network registration status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue: must be null
 * @return _VOID
 */
_VOID CM300_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$HSTATE", NULL);
} /* end WaveC_WmAt0707_NetworkRegistration */

_VOID MC600_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CAD", NULL);
} /* end MC600_WmAt0707_InquireNetworkRegStatus */


/**
 * @brief Set TE character set
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strChset: character set
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetCharacterSet(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strChset)
{
    strcpy(ATCmd, "AT+CSCS=");
    if (NULL != strChset)
    {
        strcat(ATCmd, "\"");
        strcat(ATCmd, strChset);
        strcat(ATCmd, "\"");
    }

    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SetCharacterSet */

/**
 * @brief Get the signal quality
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAt0707_InquireSignalQuality(_CHAR8* ATCmd, _UINT32* size)
{
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSQ", NULL);
#else
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSQ?", NULL);
#endif
    //WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCESQ?", NULL);    //weikk add 2016.2.25
} /* end WaveC_WmAt0707_InquireSignalQuality */

/*weikk 2016.3.31.line=xx. add AT xreg*/
_VOID WaveC_WmAt0707_InquireNetType(_CHAR8* ATCmd, _UINT32* size)
{
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QNWINFO", NULL);
#else
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XREG?", NULL);
#endif
}

_VOID WaveC_WmAt0707_InquireCellLoc(_CHAR8* ATCmd, _UINT32* size)
{
    WIRELESS_INFO("WaveC_WmAt0707_InquireCellLoc:  %d SIM Existed to get location\r\n", 0);
    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QCELLLOC=1", NULL);
}

/*weikk 20161026 .line=xx. add AT ADC*/
_VOID WaveC_WmAt0707_InquireADC(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CADC?", NULL);
}

/*weikk 20160713 .line=xx. add AT mmad*/
_VOID WaveC_WmAt0707_InquireMMAD(_CHAR8* ATCmd, _UINT32* size)
{
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QADC=1", NULL);
#else
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+MMAD?", NULL);
#endif
}

_VOID Quectel_WmAt0707_InquireCallStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPAS", NULL);
}

/**
 * @brief Get the signal quality
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID C202_WmAt0707_InquireSignalQuality(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CSQ?", NULL);
} /* end C202_WmAt0707_InquireSignalQuality */

/**
 * @brief Set supplementary service notifications
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strCSSI: cssi value
 * @param _CHAR8* strCSSU: cssu value
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetSSNote(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strCSSI, _CHAR8* strCSSU)
{
    strcpy(ATCmd, "AT+CSSN=");
    strcat(ATCmd, strCSSI);
    if (NULL != strCSSU)
    {
        strcat(ATCmd, ",");
        strcat(ATCmd, strCSSU);
    }

    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SetSSNote*/

/**
 * @brief Send Unstructured supplementary service data
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strN: 0: Disable the result code, 1: Enable the result code, 2: Cancel session
 * @param _CHAR8* strStr: USSD-string
 * @param _CHAR8* strDcs: Cell Broadcast Data Coding Scheme
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SendUSSD(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs)
{
    strcpy(ATCmd, "AT+CUSD=");
    strcat(ATCmd, strN);
    if (NULL != strStr)
    {
        strcat(ATCmd, ",");
        strcat(ATCmd, strStr);
        if (NULL != strDcs)
        {
            strcat(ATCmd, ",");
            strcat(ATCmd, strDcs);
        }
    }
    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SendUSSD */

/**
 * @brief Send USSD data
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strUssdString: USSD string
 * @return _USHORT16*
 */
_VOID WaveC_WmAt0707_SendUssdData(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strUssdString)
{
    T_STR_PDU    strIndex;

    Utl_StrCpyN(ATCmd, "AT+CUSD=1,", MAX_CUSD_LEN);
    memset(strIndex, 0, MAX_CUSD_LEN);
    sprintf(strIndex, "\"%s\"", strUssdString);
    Utl_StrCatN(ATCmd, strIndex, MAX_CUSD_LEN);
    Utl_StrCatN(ATCmd, ",15", MAX_CUSD_LEN);
    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SendUssdData */

_VOID WaveC_WmAt0707_SendSIMData(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strSimData)
{    
    sprintf(ATCmd, "AT+SIMDATA=\"%s\"", strSimData);
    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);    
    
} /* end WaveC_WmAt0707_SendSIMData */


_VOID WaveC_WmAt0707_SendATOrigin(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strATString)
{
    strcpy(ATCmd, strATString);
    
    strcat(ATCmd, ATCMD_RETURN);
    
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_SendATOrigin */


/**
 * @brief Cancel USSD data
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _USHORT16*
 */
_VOID WaveC_WmAt0707_CancelUssdData(_CHAR8* ATCmd, _UINT32* size)
{
    Utl_StrCpyN(ATCmd, "AT+CUSD=2", MAX_CUSD_LEN);
    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAt0707_CancelUssdData */

/**
 * @brief Send DTMF string
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strDtmfStr: DTMF string
 * @return _VOID
 */
_VOID WaveC_WmAt0707_DialDTMF(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strDtmfStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+VTS=", strDtmfStr);
} /* end WaveC_WmAt0707_DialDTMF */

_VOID MC323_WmAt0707_DialDTMF(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strDtmfStr, _INT32 CallIndex)
{
    sprintf(ATCmd, "AT^DTMF=%d,%s", CallIndex, strDtmfStr);
    strcat(ATCmd, ATCMD_RETURN);

    *size = (_USHORT16)strlen(ATCmd);    
} /* end MC323_WmAt0707_DialDTMF */

_VOID C202_WmAt0707_SetDTMFPrd(_CHAR8 * ATCmd,_UINT32 * size,_CHAR8 * strDtmfStr)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+VTD=", strDtmfStr);
} /* end C202_WmAt0707_SetDTMFPrd */


_VOID CM300_WmAt0707_DialDTMF(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strDtmfStr)
{
    _UCHAR8 strTemp[16] = {0};
    /* dtmf string convert */
    if(strDtmfStr[0] == '0')
    {
        strcpy(strTemp, "10");
    }
    else if(strDtmfStr[0] == '*')
    {
        strcpy(strTemp, "11");
    }
    else if(strDtmfStr[0] == '#')
    {
        strcpy(strTemp, "12");
    }    
    else
    {
        strcpy(strTemp, strDtmfStr);        
    }
    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$HDTMF=", strTemp);
} /* end CM300_WmAt0707_DialDTMF */


/**
 * @brief Set Mux Mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strPara: CMUX para
 * @return _VOID
 */
_VOID WaveC_WmAt0707_SetMuxMode(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strPara)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMUX=", strPara);
} /* end WmAt0707_SetMuxMode */
/*#endif*/

/**
 * @brief when a ATCommand is executing, send this AT command to cancel
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetCancel(_CHAR8* ATCmd, _UINT32* size)
{
    strcpy(ATCmd, "AT\r");
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAtExt_SetCancel */

/**
 * @brief Set alarm time
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param AK_CALA_ALARM *alarmData: alarm parameter
 * @return _UCHAR8
 */
_VOID WaveC_WmAtExt_SetAlarm(_CHAR8* ATCmd, _UINT32* size, RIL_CALA_ALARM *alarmData)
{
    T_STR_20        date, time;
    
    /*means query the alarm*/
    if(alarmData->valid == 2)
    {
        strcpy(ATCmd, "AT+CALA?");        /*infineon,merge*/
    }
    /*means delete the alarm*/
    else if(alarmData->valid == 0)
    {
        strcpy(ATCmd, "AT+CALD=0");    /*Changed for WaveC ,SWWU*/
    }
    else    /*means set an alarm*/
    {
        ConvertTimeSToStr(&alarmData->time, date, time); 
        sprintf(ATCmd, "AT+CALA=\"%s,%s+00\",0,0,\"Alarm\"", date, time);    /*infineon,merge*/
    }
    
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
} /* end WaveC_WmAtExt_SetAlarm */

/**
 * @brief Get field test report
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_FieldTestReport(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strMode)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CGED=", "0");
}

/**
 * @brief Read SMS message and do not change the SMS stat
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strIndex: SMS index
 * @return _VOID
 */
_VOID WaveC_WmAtExt_ReadSMSUnchgStat(_CHAR8* ATCmd, _UINT32* size, _USHORT16 index)
{
    _CHAR8 temp[4];

       strcpy(ATCmd, "AT+CMGR=");
    Utl_Itoa(index, temp, 10);
    strcat(ATCmd, temp);
    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Ring detect interrupt configuration
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strValue
 *        0: Ring detect interrupt is available only for RING and LOW VOLTAGE.
 *        1: Ring detect interrupt is available for RING, LOW VOLTAGE, SMS, ALARM, EARPHONE INSERTED, etc (decide by user).
 * @return _VOID
 */
/*_VOID WaveC_WmAtExt_ConfigRingDetect(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
//    Wm_SendStandardAT(chID, "AT+CFGRI=", strValue);
//    Wm_SendStandardAT(chID, "AT", NULL);                //infineon,merge    


    return;
}*/ /* end WaveC_WmAtExt_ConfigRingDetect */

/**
 * @brief Switch off mobile station
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SwitchOffModule(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CPWROFF", NULL);
} /* end WaveC_WmAtExt_SwitchOffModule */

/**
 * @brief Set microphone gain level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _UCHAR8 channel: main microphone or earphone microphone
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{
#if 0
    strcpy(ATCmd, "AT+CMIC=");
    if(channel == 0)
        strcat(ATCmd, "0,8");
    else if(channel == 1)
        strcat(ATCmd, "1,8");

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
#else

#endif
}

/**
 * @brief Set microphone gain level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _UCHAR8 channel: main microphone or earphone microphone
 * @return _VOID
 */
_VOID M10_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{    
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);
    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QMIC=0,", strTemp);
}

_VOID UC15_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{    
    if(volume == 12)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QMIC=1,4500,4500", NULL);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QMIC=1,5000,5000", NULL);
    }
}

/**
 * @brief Set microphone gain level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _UCHAR8 channel: main microphone or earphone microphone
 * @return _VOID
 */
_VOID CM300_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{    
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);

    WaveC_Wm_SendStandardAT(ATCmd, size, "AT$HMICV=", strTemp);
}

/**
 * @brief Set microphone gain level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _UCHAR8 channel: main microphone or earphone microphone
 * @return _VOID
 */
_VOID MC323_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{    
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);

    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMIC=", strTemp);
}

_VOID C202_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 volume)
{    
    T_STR_20        strTemp;

    Utl_Itoa(volume, strTemp, 10);

    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+VGT=", strTemp);
}

/**
 * @brief Enable/disable earphone
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetAudioPath(_CHAR8* ATCmd, _UINT32* size, RIL_AUDIO_PATH_TYPE path)
{
    T_STR_100    param;
    _BOOL         bInvalidPath = EOS_TRUE;
    
    param[0] = 0;
    if(RIL_PATH_PHONE_NORMAL == path)
    {
        strcat( param, "0" );
    }
    else if(RIL_PATH_HANDFREE == path)
    {
        strcat( param, "1" );
    }

    if(bInvalidPath)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+SPEAKER=", param);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
    }
}

/**
 * @brief Enable/disable earphone
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID M10_WmAtExt_SetAudioPath(_CHAR8* ATCmd, _UINT32* size, RIL_AUDIO_PATH_TYPE path)
{
    T_STR_100    param;
    _BOOL         bInvalidPath = EOS_TRUE;
    
    param[0] = 0;
    if(RIL_PATH_PHONE_NORMAL == path)
    {
        strcat( param, "0" );
    }
    else if(RIL_PATH_HEADSET == path)
    {
        strcat( param, "1" );
    }
    else if(RIL_PATH_HANDFREE == path)
    {
        strcat( param, "1" );
    }

    if(bInvalidPath)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QAUDCH=", param);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
    }
}

_VOID UC15_WmAtExt_SetAudioMod(_CHAR8* ATCmd, _UINT32* size, RIL_AUDIO_MOD_TYPE path)
{
    T_STR_100    param;
    _BOOL         bInvalidPath = EOS_TRUE;
    
    param[0] = 0;
    if(RIL_AUDMOD_HANDSET == path)
    {
        strcat( param, "0" );
    }
    else if(RIL_AUDMOD_HEADSET == path)
    {
        strcat( param, "1" );
    }

    if(bInvalidPath)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QAUDMOD=", param);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
    }
}


_VOID S5320_WmAtExt_SetVoiceHangupCtrl(_CHAR8* ATCmd, _UINT32* size, _BOOL bEnable)
{
    T_STR_100    param;
    _BOOL         bInvalidPath = EOS_TRUE;
    
    param[0] = 0;
    if(EOS_TRUE == bEnable)
    {
        strcat( param, "0" );
    }
    else if(EOS_FALSE == bEnable)
    {
        strcat( param, "1" );
    }

    if(bInvalidPath)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CVHU=", param);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
    }
}

/**
 * @brief Enable/disable earphone
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID MC323_WmAtExt_SetAudioPath(_CHAR8* ATCmd, _UINT32* size, RIL_AUDIO_PATH_TYPE path)
{
    T_STR_100    param;
    _BOOL         bInvalidPath = EOS_TRUE;
    
    param[0] = 0;
    if(RIL_PATH_PHONE_NORMAL == path)
    {
        strcat( param, "0" );
    }
    else if(RIL_PATH_HANDFREE == path)
    {
        strcat( param, "1" );
    }

    if(bInvalidPath)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT^SWSPATH=", param);
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
    }
}



/**
 * @brief Inquire headset status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_InquireHeadsetStatus(_CHAR8* ATCmd, _UINT32* size)
{
    RIL_sXDRV_TYPE xdrvType;

    xdrvType.devName = RIL_NAME_HEADSET;
    xdrvType.ctrType = RIL_TYPE_INQUIRE;
    xdrvType.ctrEnable = EOS_FALSE;
    WaveC_WmAtExt_XDRVURCControl(ATCmd, size, &xdrvType);
} /* end WaveC_WmAtExt_InquireHeadsetStatus */

/**
 * @brief Set Headset URC enable / disable
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: Headset URC mode, 1: on, 0: off
 * @return _VOID
 */
/*_VOID WaveC_WmAtExt_SetHeadsetURC(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strMode)
{
    //Wm_SendStandardAT(chID, "AT+CEXTHS=", strMode);

    return;
}*/ /* end WaveC_WmAtExt_SetHeadsetURC */

/**
 * @brief Inquire headset button status
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_InquireHeadsetBtnStatus(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XIER=2", NULL);
} /* end WaveC_WmAtExt_InquireHeadsetBtnStatus */

/**
 * @brief Set headset button URC enable / disable
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: 1: on, 0: off
 * @return _VOID
 */
/*_VOID WaveC_WmAtExt_SetHeadsetBtnURC(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strMode)
{
    //Wm_SendStandardAT(chID, "AT+CEXTBUT=", strMode);    //infineon,merge,infineon have no this command

    return;
} *//* end WaveC_WmAtExt_SetHeadsetBtnURC */

/**
 * @brief Get the battery level
 *
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_InquireBatteryLevel(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CBC", NULL);
} /* end WaveC_WmAtExt_InquireBatteryLevel */

/**
 * @brief Inquire PIN remain count
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_InquirePinCount(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XPINCNT", NULL);    
} /* end WaveC_WmAtExt_InquirePinCount */

_VOID WaveC_WmAtExt_InquirePukCount(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XPINCNT", NULL);
} /* end WaveC_WmAtExt_InquirePukCount */

/**
 * @brief Dump BroadCast
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_DumpBroadCast(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
} /* end WaveC_WmAtExt_DumpBroadCast */

/**
 * @brief Ringer Sound Level
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _SHORT16 level: volume
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetRingerSoundLevel(_CHAR8* ATCmd, _UINT32* size, _SHORT16 level)
{
    strcpy(ATCmd, "AT+CRSL=");
    if (level == 0)
        strcat(ATCmd, "0");    
    else
        strcat(ATCmd, "100");    

    strcat(ATCmd, ATCMD_RETURN);
    *size = (_USHORT16)strlen(ATCmd);
}

/**
 * @brief Echo cancellation control
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strAudioParam: audio parameter
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetAudioOutput(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strAudioParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+ECHO=", strAudioParam);
} /* end WaveC_WmAtExt_SetAudioOutput */

/**
 * @brief Slow Clk Enable
 *
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
/*_VOID WaveC_WmAtExt_SlowClkEnable(_CHAR8* ATCmd, _UINT32* size)
{
    //Wm_SendStandardAT(chID, "AT+CSCLK=1", NULL);    //infineon,merge

    return;
} *//* WaveC_WmAtExt_SlowClkEnable */

/**
 * @brief SMS Full URC Enable
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetSmsFullUrc(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CIND=", strValue);
} /* end WaveC_WmAtExt_SmsFullUrcEnable */

_VOID WaveC_WmAtExt_QueryModuleState(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XIER=0", NULL);
}

/**
 * @brief Set Ring Line Mode
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strMode: 1: on
 * @return _VOID
 */
/*_VOID WaveC_WmAtExt_SetOtherRingLine(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strMode)
{
    //Wm_SendStandardAT(chID, "AT*TATRI=", strMode);

    return;
}*/

/**
 * @brief AT+COLP=?
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_InquireCOLP(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+COLP?", NULL);
} /* end WaveC_WmAtExt_InquireCOLP */

/**
 * @brief Set Colp Status
 *
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetColpStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+COLP=", strValue);
} /* end WaveC_WmAtExt_SetColpStatus */


/**AT command only for infineon,infineon,merge**/

/**
 * @Check latest URC event 
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_CheckLatestURCEvent(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strIndex)
{
    /*strIndex 
    0:reason for power on 1:headset    2:hook key 
    3:battery low4    4:charger    5:RTC  */
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XIER=", strIndex);
}


/**
 * @control whether URC indicate to AK23xx
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_ControlURCIndication(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue)
{
    /*strValue = pa1,pa2,pa3,pa4,pa5
    pa1: headset in/out    pa2: hootkey    pa3:bettery low 
    pa4: charge    pa5:RTC 
    */
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCDU=", strValue);
}


/**
 * @control module audio driver including path,handfree and mute 
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_ControlModuleDriver(_CHAR8* ATCmd, _UINT32* size, _UCHAR8 mode)
{
    _CHAR8 strMode[20];

    /*config module drive-path*/
    strcpy(strMode, "0,");    
    /*---------------control audio path*/
    if(mode == 0)
    {/*if earphone out,normal mode */
        strcat(strMode, "0,1,1");    /*infineon_ulc2*/
    }
    else if(mode == 1)
    {/*if earphone in*/
        strcat(strMode, "0,8,4");    /*infineon_ulc2*/
    }
    else if(mode == 2)
    {/*mute disable , unmute uplink*/
        strcat(strMode, "8,1,1");
    }
    else if(mode == 3)
    {/*mute enable downlink, unmute uplink*/
        strcat(strMode, "8,0,1");
    }
    else if(mode == 4)
    {/*handfree disable, to normal mode */
        strcat(strMode, "0,1,1");    /*infineon_ulc2*/
    }
    else if(mode == 5)
    {/*handfree enable,switch to headset path*/
        strcat(strMode, "0,8,5");    /*infineon_ulc2*/
    }
    else if(mode ==6)
    {/*audioloop test enable*/
        strcat(strMode, "15,4,4");
    }
    else if(mode ==7)
    {/*audioloop test disable*/
        strcat(strMode, "16,0,0");
    }
    else if(mode == 8)
    {/*enable PCM channel*/
        strcat(strMode, "0,4,2");    
    }
    else if(mode == 9)
    {/*enable PCM channel*/
        strcat(strMode, "0,2,1");    
    }
    /*---------------control speaker to generate tone */
    else if(mode == 10)
    {/*generate call waiting tone, volume: 10*/
        strcat(strMode, "5,1,2,10");    
    }
    else if(mode == 11)
    {/*generate call waiting tone, volume: 100*/
        strcat(strMode, "5,1,2,100");
    }
    else
    {/*others*/
        strcat(strMode, "0,0,0");
    }
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=", strMode);
}


/**
 * @Query the URC indicate to AK23xx
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_QueryURCIndication(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMER?", NULL);
}

/**
 * @brief Mobile equipment event reporting
 *
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_MobileEquipmentEvent(_CHAR8* ATCmd, _UINT32* size,_CHAR8* strValue)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CMER=", strValue);
}

/**
 * @brief only send "at"
 *
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SendAT(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT", NULL);
} 

_VOID WaveC_WmAtExt_PwrCtrl(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QPOWD=0", NULL);
} 

_VOID UC15_WmAtExt_PwrCtrl(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QPOWD", NULL);
} 

/**
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetMSStorageType(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strService)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCPMS=", strService);
    
} /* end WaveC_WmAt0705_SetMSStorageType */


/**
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetExtSMSStorageStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strService)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XTESM=", strService);
} /* end WaveC_WmAt0705_SetExtSMSStorageStatus */


/**forbid modem to sleep
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetModuleSleep(_CHAR8* ATCmd, _UINT32* size,_UCHAR8 enable)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XL1SET=", "\"psvoff\"");
}

/**get modem version information
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_GetModemVersionNum(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XGENDATA", NULL);
}

/**set charging current step
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @ param  _CHAR8*  '0'~'7' (400mA~1100mA)
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetChargingCurrent(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strStep)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=5,", strStep);

} /* end WaveC_WmAtExt_SetChargingCurrent */


/**
 * @brief Indicator control use for ulc2
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_ConfigURCMode(_CHAR8* ATCmd, _UINT32* size, RIL_URC_MODE mode)
{
    if(RIL_URC_SLEEP == mode)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCIND=", "128");
    }
    else if(RIL_URC_DATACONN == mode)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCIND=", "0");
    }
    else if(RIL_URC_BUSY == mode)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCIND=", "0");
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCIND=", "130");
    }
}

/**
 * @brief URC control
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_XDRVURCControl(_CHAR8* ATCmd, _UINT32* size, RIL_sXDRV_TYPE* xdrvType)
{
    if(xdrvType->devName == RIL_NAME_HEADSET)
    {
        if(xdrvType->ctrType == RIL_TYPE_SETURCRPT)
        {
            if(xdrvType->ctrEnable == EOS_TRUE)
            { 
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=6,0,1", NULL);        
            }
            else
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=6,0,0", NULL);        
            }
        }
        else if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=10,0,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT",NULL);        
        }        
    }
    else if(xdrvType->devName == RIL_NAME_HOOKKEY)
    {
        if(xdrvType->ctrType == RIL_TYPE_SETURCRPT)
        {
            if(xdrvType->ctrEnable == EOS_TRUE)
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=6,0,1", NULL);        
            }
            else
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=6,0,0", NULL);        
            }    
        }
        else if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=10,1,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT",NULL);        
        }    
    }
    else if(xdrvType->devName == RIL_NAME_POWERONREASON)
    {
        if(xdrvType->ctrType == RIL_TYPE_SETURCRPT)
        {
            if(xdrvType->ctrEnable == EOS_TRUE)
            {
                WaveC_Wm_SendStandardAT(ATCmd, size,  "AT", NULL);        
            }
            else
            {
                WaveC_Wm_SendStandardAT(ATCmd, size,  "AT", NULL);        
            }    
        }
        else if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=9,0,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }    
    }
    else if(xdrvType->devName == RIL_NAME_BATTERYLOW)
    {
        if(xdrvType->ctrType == RIL_TYPE_SETURCRPT)
        {
            if(xdrvType->ctrEnable == EOS_TRUE)
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=5,0,1", NULL);        
            }
            else
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=5,0,0", NULL);        
            }    
        }
        else if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT",NULL);        
        }    
    }
    else if(xdrvType->devName == RIL_NAME_BATTERYSHUTDOWN)
    {
        if(xdrvType->ctrType == RIL_TYPE_SETURCRPT)
        {
            if(xdrvType->ctrEnable == EOS_TRUE)
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=5,0,1", NULL);        
            }
            else
            {
                WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=5,0,0", NULL);        
            }
        }
        else if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }    
    }    
    else if(xdrvType->devName == RIL_NAME_BETTERYVOLTAGE)
    {
        if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT+XDRV=10,5,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT",NULL);        
        }    
    }    
    else if(xdrvType->devName == RIL_NAME_BATTERYTEMPER)
    {
        if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=10,6,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }    
    }    
    else if(xdrvType->devName == RIL_NAME_ENVIRTEMPER)
    {
        if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT+XDRV=10,7,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }    
    }        
    else if(xdrvType->devName == RIL_NAME_STATICEEP)
    {
        if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT+XDRV=9,3,", xdrvType->strValue1);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }    
    }    
    else if(xdrvType->devName == RIL_NAME_CHIPID)
    {
        if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT+XDRV=9,4,0", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }    
    }    
    else if(xdrvType->devName == RIL_NAME_BATTERYIDVOL)
    {
        if(xdrvType->ctrType == RIL_TYPE_SETURCRPT)
        {
            if(xdrvType->ctrEnable == EOS_TRUE)
            {
                T_STR_100  Cmd;
                   strcpy(Cmd, xdrvType->strValue1);
                strcat(Cmd, ",");
                strcat(Cmd, xdrvType->strValue2);
                WaveC_Wm_SendStandardAT(ATCmd, size,  "AT+XDRV=9,6,1", Cmd);        
            }
            else
            {
                WaveC_Wm_SendStandardAT(ATCmd, size,  "AT", NULL);        
            }
        }
        else if(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT+XDRV=9,5,1", NULL);        
        }
        else
        {
            WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
        }        
    }    
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size,  "AT",NULL);        
    }
}

/**
 * @brief control ULC2 audio volume  
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
     RIL_XDRV_AUDIO_CTRL_TYPE
     _CHAR8*
 * @return _VOID
 */
_VOID WaveC_WmAtExt_XDRVAudioControl(_CHAR8* ATCmd, _UINT32* size,RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue)
{
    T_STR_100            Cmd;

    strcpy(Cmd, "AT+XDRV=0,1,");    
    
    strcat(Cmd, strValue);
    
    if(audCtrlType == RIL_AUDIO_MASTER_VOLUME)
    {
        strcat(Cmd, ",1");
    }
    else if(audCtrlType == RIL_AUDIO_SOURCE_VOLUME)
    {
        strcat(Cmd, ",0");
    }
    else
    {
        
    }
    WaveC_Wm_SendStandardAT(ATCmd, size, Cmd, NULL);    
} /* end WmAt0705_SetExtSMSStorageStatus */


/**
 * @brief control infineon trace output 
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _BOOL bOnOff: EOS_TRUE-enable trace output, EOS_FALSE-disable trace output
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetTraceOutput(_CHAR8* ATCmd, _UINT32* size,_BOOL bOnOff)
{
    if(bOnOff)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+TRACE=", "1");
    }
    else
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+TRACE=", "0");
    }
}


/**
 * @brief read exception log of modem 
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @return _VOID
 */
_VOID WaveC_WmAtExt_GetModemExcLog(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XLOG",NULL);
} /* end WmAtExt_GetModemExcLog */



/**
 * @brief control new SMS Status 
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
     _BOOL enable: 
    "EOS_TRUE"-new message will keep unread after reading
    "EOS_FALSE"- new message state is read after reading
 * @return _VOID
 */
_VOID WaveC_WmAtExt_XDRVControlNewSMSStatus(_CHAR8* ATCmd, _UINT32* size, _BOOL enable)
{    
    if(enable == EOS_TRUE)    
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=9,2,1", NULL);    
    }
    else 
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=9,2,0", NULL);    
    }
}

/**
 * @brief set the signal strength report 
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
     _BOOL enable: 
     "EOS_TRUE"-enable the URC of signal strength "+XCIEV:"report 
    "EOS_FALSE"-disable the URC of signal strength "+XCIEV:"report 
 * @return _VOID
 */
_VOID WaveC_WmAtExt_SetSignalStrengthReport(_CHAR8* ATCmd, _UINT32* size, _BOOL enable)
{    
    if(enable == EOS_TRUE)    
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XMER=1", NULL);    
    }
    else if(enable == EOS_FALSE)
    {
        WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XMER=0", NULL);    
    }
    return;    
}

/**
 * @brief enable or disable audio loop test
 * @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _BOOL enable: "EOS_TRUE"-enable audio loop test, "EOS_FALSE"-disable audio loop test
 * @return _VOID
 * @retval
 */
_VOID WaveC_WmAtExt_XDRVControlAudioLoopTest(_CHAR8* ATCmd, _UINT32* size, _BOOL enable)
{ 
    T_STR_100            Cmd;

    strcpy(Cmd, "9,8,");
    if (enable)
    {
        strcat(Cmd, "1,");
    }
    else 
    {
        strcat(Cmd, "0,");
    }
    strcat(Cmd, "4,4,100,20000");

    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XDRV=", Cmd);
}
/*end for ulc2*/

_VOID WaveC_STK_SetProfile(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+CFUN=6", NULL);
}

_VOID M35_STK_SetAutoRsp(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QSTKAUTORSP=0,1", NULL);
}

_VOID M35_STK_SetTR(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+STKTR=\"810301250082028281830100\"", NULL);
}

_VOID WaveC_STK_SelectMainMenu(_CHAR8* ATCmd, _UINT32* size, _SHORT16 index)
{
    T_STR_20        strTemp;
    
    Utl_Itoa(index, strTemp, 10);
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+STKENV=211,", strTemp);
}

_VOID WaveC_STK_Response(_CHAR8* ATCmd, _UINT32* size, RIL_RES_PAR *par)
{
    if(par->cmdId != RIL_STK_Invalid)
    {        
        if((par->cmdId == RIL_STK_GetInput) && (par->res.r_get_input == RIL_GetInputOK))
        {
            _CHAR8    temp_content[1024] = {0};
            _CHAR8    strTemp[1024] = {0};
            if ((RIL_DigitsFromSMSUnpacked == par->para.u_get_input.response)
                || (RIL_SMSUnpacked == par->para.u_get_input.response) 
                || (RIL_SMSPacked == par->para.u_get_input.response))
            {
                ConvUnicodeToGSM(par->content, temp_content);
                ConvGSMToUCS2Str(temp_content, strTemp);
                sprintf(par->data,"0,0,0,%d,\"%s\"", RIL_Data8bit, strTemp);
            }
            else if ((RIL_DigitsFromUCS2 == par->para.u_get_input.response)
                    || (RIL_ipUCS2 == par->para.u_get_input.response)
                    || (RIL_SMSPackedUCS2 == par->para.u_get_input.response)
                    || (RIL_SMSUnpackedUCS2 == par->para.u_get_input.response))
            { 
                ConvUnicodeToUCS2Str(par->content, strTemp);
                sprintf(par->data,"0,0,0,%d,\"%s\"", RIL_UCS2, strTemp);
            }
            else
            {
                /*convert to 7bit data*/
            }
        }
        else if((par->cmdId == RIL_STK_GetInkey) && (par->res.r_get_inkey == RIL_GetInkeyOK))
        {    
            _CHAR8    temp_content[10] = {0};
            
            if(RIL_ikYesNo == par->para.u_get_inkey.response)
            {
                sprintf(par->data,",0,0,0,%d,01",RIL_Data8bit);    
            }
            if(RIL_ikDigits == par->para.u_get_inkey.response ||
                    RIL_ikSMS == par->para.u_get_inkey.response)
            {
                ConvUnicodeToUCS2Str(par->inkeycontent, temp_content);
                sprintf(par->data,",0,0,0,%d,%s", RIL_Data8bit, temp_content);
            }
            else if(RIL_ikUCS2 == par->para.u_get_inkey.response)
            {
                ConvUnicodeToUCS2Str(par->inkeycontent, temp_content);
                sprintf(par->data,",0,0,0,%d,%s",RIL_UCS2,temp_content);
            }
        }
        else if((par->cmdId == RIL_STK_PlayTone) && (par->res.r_play_tone == RIL_PlayToneOK))
        {
            sprintf(par->data, "0,0");
        }
        else if(par->cmdId == RIL_STK_SendSS)
        {
            strcpy(par->data, "0,1");
        }
        else if((par->cmdId == RIL_STK_SendSM) && (par->res.r_select_item != RIL_siTerminateProactive))
        {
            strcpy(par->data, "0,1");
        }
        else if(par->cmdId == RIL_STK_SendUSSD)
        {
            strcpy(par->data, "0,0");
        }
        else if((par->cmdId == RIL_STK_SetupCall) && (par->res.r_setupcall == RIL_AcceptedCall))
        {
            strcpy(par->data, "1,0");    
        }
        else if((par->cmdId == RIL_STK_SetupCall) && (par->res.r_setupcall == RIL_RejectedCall))
        {
            strcpy(par->data, "16");    
        }
        else if((par->cmdId == RIL_STK_SelectItem) && (par->res.r_select_item == RIL_SelectItemOK))
        {
            get_stk_id_str(par->select, par->data);
        }
        else 
        {
            par->data[0] = 0;
        }

        if(strlen(par->data) > 0)
        {
            sprintf(ATCmd, "AT+STKTR=%d,%s\r", par->cmdId, par->data);
        }
        else
        {
            switch(par->res.r_display_text)
            {
            case RIL_DisplayTextOK:
                break;
            case RIL_CallIsBusy:
                par->res.r_display_text = (RIL_STK_DISPLAYTEXT_RES)12;
                break;
            case RIL_TerminateProactive:
                par->res.r_display_text = (RIL_STK_DISPLAYTEXT_RES)16;
                break;
            default:
                par->res.r_display_text = (RIL_STK_DISPLAYTEXT_RES)17;
                break;
            }
            sprintf(ATCmd , "AT+STKTR=%d,%d\r", par->cmdId, par->res.r_display_text);
        }
        *size = (_USHORT16)strlen(ATCmd);
    }
    else
    {
        *size = 0;
    }
}

_VOID WaveC_WmAtExt_ReportCallStat(_CHAR8* ATCmd, _UINT32* size)
{    
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+XCALLSTAT=1", NULL);
}

/**
 * @brief Set Battery Param
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SetBatteryParam(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+WBCM=", strParam);
} /* end WaveC_WmAt0705_SetBatteryParam */

/**
 * @brief Set Tone Detect Param
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SetToneDetParam(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QTONEDET=", strParam);
} /* end WaveC_WmAt0705_SetToneDetParam */

_VOID WaveC_WmAt0705_SetDAIParam(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QDAI=", strParam);
} /* end WaveC_WmAt0705_SetDAIParam */

/**
 * @brief Select Voice Gain
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SelectVoiceGain(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+WSVG=", strParam);
} /* end WaveC_WmAt0705_SelectVoiceGain */

/**
 * @brief Set Side Tone
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SetSideTone(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+SIDET=", strParam);
} /* end WaveC_WmAt0705_SetSideTone */

/**
 * @brief Set Side Tone
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID M10_WmAt0705_SetSideTone(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QSIDET=", strParam);
} /* end M10_WmAt0705_SetSideTone */

_VOID M10_WmAt0705_InquireSideTone(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT+QSIDET?", NULL);
}

/**
 * @brief Save Configuration
* @param _CHAR8* ATCmd: Input buffer to store AT cmd
 * @param _UINT32*  size: size of ATCmd
 * @param _CHAR8* strParam: Battery Param
 * @return _VOID
 */
_VOID WaveC_WmAt0705_SaveConfiguration(_CHAR8* ATCmd, _UINT32* size)
{
    WaveC_Wm_SendStandardAT(ATCmd, size, "AT&W", NULL);
} /* end WaveC_WmAt0705_SaveConfiguration */


