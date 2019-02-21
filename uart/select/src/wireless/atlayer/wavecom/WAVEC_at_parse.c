
#include "eos.h"
#include "wireless_string.h"
#include "wireless_dataconvert.h"
#include "WAVEC_at_pack.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

extern WIRELESSROOTST     stWirelessRootSt;

_USHORT16 G_MaxTeleLen = MAX_TELE_LEN;
_USHORT16 G_MaxNameLen = MAX_NAME_LEN;


#define    T_S16_MAX            ((_SHORT16)(32767L))            /** maximum T_S16 value*/

static _INT32 _stk_min(_INT32 a, _INT32 b)
{
    return (a < b) ? a : b;
}

static _SHORT16 _ril_SearchEnd(const _CHAR8* str, _SHORT16 offset, _USHORT16 length)
{
    _SHORT16 i = 0;
    _SHORT16 j = 0;
    _SHORT16 k = 0;    
    _SHORT16 r = 0;
    
    i = Utl_StrFndL(str , AT_COMMA , offset , length);
    j = Utl_StrFndL(str , ATCMD_RETURN , offset , length);
    k = Utl_StrFndL(str , AT_NEW_LINE , offset , length);
    if((-1 == i) && (-1 == j) && (-1 == k))
        return -1;

    if(i < 0)    
    {
        i = T_S16_MAX;
    }
    if(j < 0)
    {
        j = T_S16_MAX;
    }
    if(k < 0)    
    {
            k = T_S16_MAX;
    }
    
    r = (_SHORT16)_stk_min(((_USHORT16)i) , ((_USHORT16)j));
    r = (_SHORT16)_stk_min(((_USHORT16)r) , ((_USHORT16)k));
    return r;
}

static _BOOL ParseErrorID(const _CHAR8* strResult, _USHORT16 length, RIL_PARSE_RES* errorType, _UINT32 *errorID)
{
    _CHAR8    temp[16];
    _SHORT16    iBegin, iEnd;
    
    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, length)) != -1)
    {
        iBegin += AK_CME_LEN_12;
        iEnd = _ril_SearchEnd(strResult, iBegin, length);
        Utl_StrMidL(temp, strResult, iBegin, (_SHORT16)(iEnd-1), length);
        *errorID = (_UINT32)Utl_Atoi(temp);
        *errorType = RIL_prCMEERROR;
        return EOS_TRUE;
    }
    else if((iBegin = Utl_StrFndL(strResult, "+CMS ERROR:", 0, length)) != -1)
    {
        iBegin += AK_CME_LEN_12;
        iEnd = _ril_SearchEnd(strResult, iBegin, length);
        Utl_StrMidL(temp, strResult, iBegin, (_SHORT16)(iEnd-1), length);
        *errorID = (_UINT32)Utl_Atoi(temp);
        *errorType = RIL_prCMSERROR;
        return EOS_TRUE;
    }
    else if(Utl_StrFndL(strResult, "ERROR", 0, length) != -1)
    {
        *errorType = RIL_prERROR;
        return EOS_TRUE;
    }
    return EOS_FALSE;
}

/**
 * @brief Get AT command response .
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_ParseOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16    offset, size1;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;
    
    if (iLength < AT_ERROR_LEN)    /* 20: include ERROR type and ERROR ID */
    {
        offset = 0;
        size1 = iLength;
    }
    else
    {
        offset = iLength - AT_ERROR_LEN;
        size1 = AT_ERROR_LEN;
    }
    
    if(ParseErrorID(strResult + offset, size1, result, strNum))
    {
        WIRELESS_INFO("Result:%d, strNum:%d,strResult:%s\r\n", *result, *strNum, (strResult + offset));
        return NULL;
    }
    
    if (Utl_StrFndL(strResult + offset, "OK\r\n", 0, size1) != -1) 
    { 
        *result = RIL_prOK;
        return NULL;
    }
    else
    {    
        *result = RIL_prERROR;
        return NULL;
    }
}

_VOID* WaveC_ParseQiRawDataOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16    offset, size1;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;
    
    if (iLength < AT_ERROR_LEN)    /* 20: include ERROR type and ERROR ID */
    {
        offset = 0;
        size1 = iLength;
    }
    else
    {
        offset = iLength - AT_ERROR_LEN;
        size1 = AT_ERROR_LEN;
    }
    
    if(ParseErrorID(strResult + offset, size1, result, strNum))
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Raw Data Send Err(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    
    if (Utl_StrFndL(strResult + offset, "SEND OK\r\n", 0, size1) != -1) 
    { 
        WIRELESS_INFO("Raw Data Send OK(%s)(%d)\r\n", __FILE__, __LINE__);
        *result = RIL_prOK;
        return NULL;
    }
    else
    {    
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Raw Data Send Err(%s)(%d)\r\n", __FILE__, __LINE__);
        *result = RIL_prERROR;
        return NULL;
    }
}

_VOID* WaveC_ParseOkError_TempForSetOpr(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16    offset, size1;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;

    if (iLength < AT_ERROR_LEN)    /* 20: include ERROR type and ERROR ID */
    {
        offset = 0;
        size1 = iLength;
    }
    else
    {
        offset = iLength - AT_ERROR_LEN;
        size1 = AT_ERROR_LEN;
    }
    
    if(ParseErrorID(strResult + offset, size1, result, strNum))
    {
        WIRELESS_TRACE;
        printf("SelectOpr Result:%s\r\n", strResult);
        return NULL;
    }
    
    if (Utl_StrFndL(strResult + offset, "OK\r\n", 0, size1) != -1) 
    { 
        *result = RIL_prOK;
        return NULL;
    }
    else
    {    
        WIRELESS_TRACE;
        printf("SelectOpr Result:%s\r\n", strResult);
        *result = RIL_prERROR;
        return NULL;
    }
}

_VOID *M10_WmPsGsmCd_QuerySideTone(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
      _CHAR8    strTemp[16];
    _INT32       *QSideTone = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+QSIDET", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, ":", iBegin, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }    

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
    QSideTone = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));

    *QSideTone = Utl_Atoi(strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return QSideTone;
}

_VOID* WaveC_OriginATResponse(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start = 0;
    _SHORT16    end = 0;
    _CHAR8*     ATResponse = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    ATResponse = (_CHAR8*)WIRELESS_MALLOC(iLength);
    
    memset(ATResponse, 0, iLength);
    
    memcpy(ATResponse, strResult, iLength);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)ATResponse;
}

/**
 * @brief get data connection result 
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 */
_VOID* WaveC_ParseDataConnect(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16   iLength = 0;
    
    *strNum = 0;
       *strSize = 0;
    iLength = (_USHORT16)strlen(strResult);

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        return NULL;
    }

    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) != -1) 
    { 
        *result = RIL_prOK;
           return NULL;
    }
    else if (Utl_StrFndL(strResult, "CONNECT", 0, iLength) != -1) 
    { 
        *result = RIL_prCONNECT;
           return NULL;
    }
    else if (Utl_StrFndL(strResult, "NO CARRIER", 0, iLength) != -1) 
    { 
        *result = RIL_prNOCARRIER;
           return NULL;
    }
    else
    {    
        *result = RIL_prERROR;
           return NULL;
    }
}

_VOID* WaveC_ParseDataResult(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16   iLength;

    *strNum = 0;
       *strSize = 0;
    iLength = (_USHORT16)strlen(strResult);

    if ((Utl_StrFndL(strResult, "OK\r\n", 0, iLength) != -1)
        || (Utl_StrFndL(strResult, "ERROR", 0, iLength) != -1)
        || (Utl_StrFndL(strResult, "NO CARRIER", 0, iLength) != -1)) 
    { 
        *result = RIL_prOK;
           return NULL;
    }
    else
    {    
        *result = RIL_prERROR;
           return NULL;
    }
}

/**
 * @brief get data connection result with CSD
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_ParseDataConnectCSD(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    *strNum = 0;
       *strSize = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        return NULL;
    }

    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) != -1) 
    { 
        *result = RIL_prOK;
           return NULL;
    }
    else if (Utl_StrFndL(strResult, "username:", 0, iLength) != -1) 
    { 
        *result = RIL_prOK;
           return NULL;
    }
    else if (Utl_StrFndL(strResult, "password:", 0, iLength) != -1) 
    { 
        *result = RIL_prOK;
           return NULL;
    }
    else if (Utl_StrFndL(strResult, "PPP mode.", 0, iLength) != -1) 
    { 
        *result = RIL_prCONNECT;
           return NULL;
    }
    else if (Utl_StrFndL(strResult, "NO CARRIER", 0, iLength) != -1) 
    { 
        *result = RIL_prNOCARRIER;
           return NULL;
    }
    else
    {    
        *result = RIL_prERROR;
           return NULL;
    }
}

/**
 * @brief Get AT+CMGS command response .
 * @brief parse ">" or "ERROR"
 * @param _CHAR8* strResult: received AT result
 * @param RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_ParseCMGSOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 start = 0;
    _USHORT16 iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        return NULL;
    }

    start = Utl_StrFndL(strResult, "AT+CMGS=", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    start = Utl_StrFndL(strResult, ">", start, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    else
    { 
        *result = RIL_prOK;
        return NULL;
    }
}

_VOID* WaveC_WmPs0705_InquireQiStat(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("WaveC_WmPs0705_InquireQiStat: strResult:\n %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    
    start = Utl_StrFndL(strResult, "CONNECT OK", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    *result = RIL_prOK;
    return NULL;
}

_VOID* WaveC_WmPs0705_InquireQiSack(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8      strTemp[1024];
    _CHAR8      *RcvData = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("WaveC_WmPs0705_InquireQiSack: strResult:\n %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    start = Utl_StrFndL(strResult, " 0, 0, 0", 0, iLength);
    if (start != -1)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+QISACK: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + strlen("+QISACK: ");
    end = Utl_StrFndL(strResult, "\n", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));

    RcvData = (char*)WIRELESS_MALLOC(sizeof(strTemp));

    strcpy(RcvData, strTemp);
        
    char *SendSack;
    SendSack=strtok(strTemp,",");
    SendSack=strtok(NULL,",");
    SendSack=strtok(NULL,",");

    if((SendSack != NULL) && (SendSack[1] == '0'))
    {
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = strlen(RcvData);
        return RcvData;
    }
    else
    {
        *result = RIL_prCONNECT;
        *strNum = 0;
        *strSize = strlen(RcvData);
        return RcvData;
    }
}

_VOID* WaveC_WmPs0705_InquireQiLocIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("WaveC_WmPs0705_InquireQiLocIP: strResult:\n %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    
    start = Utl_StrFndL(strResult, "AT+QILOCIP", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    *result = RIL_prOK;
    return NULL;
}

_VOID* WaveC_WmPs0705_InquireQiRcvData(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8      strTemp[1024];
    _CHAR8      *RcvData = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _SHORT16 head_s = 0, head_e= 0;

    WIRELESS_INFO("WaveC_WmPs0705_InquireQiRcvData-strResult:\n %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    if((start = Utl_StrFndL(strResult, "ERROR", 0, iLength)) != -1)
    {
        if('\n' == strResult[start - 1])
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            return NULL;
        }
    }
    
    start = Utl_StrFndL(strResult, "+QIRD: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prCONNECT;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    start = start + strlen("+QIRD: ");
    head_s = start;
    start = Utl_StrFndL(strResult, "\n", start, iLength);
    head_e = start;
    {
        char *ptr = NULL;
        int len = 0;
        char tmp_buf[128];
        
        memset(tmp_buf, 0,sizeof(tmp_buf));
        if(head_e - head_s < sizeof(tmp_buf))
        {
            memcpy(tmp_buf, strResult+head_s, head_e - head_s);
        }
        else
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            return NULL;
        }
        
        ptr = strstr(tmp_buf, ",TCP,");
        
        if(NULL == ptr)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            return NULL;
        }
        ptr += strlen(",TCP,");
        len = atoi(ptr);
        
        WIRELESS_INFO("WaveC_WmPs0705_InquireQiRcvData-strResult:\n ptr:%s, len:%d, tmp_buf:%s\r\n", ptr,len,tmp_buf);
        end = start+1 + len;
    }

    RcvData = (char*)WIRELESS_MALLOC(sizeof(strTemp));
    memcpy(RcvData, strResult + start+1, end -(start+1));

    *result = RIL_prOK;
    *strNum = end -(start+1);
    *strSize = end -(start+1);
    return RcvData;
}

_VOID* WaveC_ParseQISendOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 start = 0;
    _USHORT16 iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        return NULL;
    }

    start = Utl_StrFndL(strResult, "AT+QISEND=", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    start = Utl_StrFndL(strResult, ">", start, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    else
    { 
        *result = RIL_prOK;
        return NULL;
    }
}

_VOID* SIM800_WmPsV25_InquireCipAck(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8      strTemp[1024];
    _CHAR8      *RcvData = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("SIM800_WmPsV25_InquireCipAck: strResult:\n %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    start = Utl_StrFndL(strResult, " 0,0,0", 0, iLength);
    if (start != -1)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+CIPACK: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + strlen("+CIPACK: ");
    end = Utl_StrFndL(strResult, "\n", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));

    RcvData = (char*)WIRELESS_MALLOC(sizeof(strTemp));

    strcpy(RcvData, strTemp);
        
    char *SendSack;
    SendSack=strtok(strTemp,",");
    SendSack=strtok(NULL,",");
    SendSack=strtok(NULL,",");

    if((SendSack != NULL) && (SendSack[0] == '0'))
    {
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = strlen(RcvData);
        return RcvData;
    }
    else
    {
        *result = RIL_prCONNECT;
        *strNum = 0;
        *strSize = strlen(RcvData);
        return RcvData;
    }
}

_VOID* SIM800_WmPsV25_InquireCipRxget(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8      *RcvData = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _INT32 iPos1 = 0;
    _INT32 iBegin= 0;
    _INT32 iEnd = 0;
    _INT32 DataLenInt = 0;
    _CHAR8 DataLenBuff[8];

    WIRELESS_INFO("SIM800_WmPsV25_InquireCipRxget-strResult:\n %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if((start = Utl_StrFndL(strResult, "ERROR", 0, iLength)) != -1)
    {
        if('\n' == strResult[start - 1])
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            return NULL;
        }
    }
    
    start = Utl_StrFndL(strResult, "+CIPRXGET: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prCONNECT;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    start = start + strlen("+CIPRXGET: ");

    iBegin = Utl_StrFndChr(strResult, ',', start);
    if(iBegin > (strlen(strResult) - 1))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    iPos1 = Utl_StrFndChr(strResult, ',', (iBegin +1));
    if(iPos1 > (strlen(strResult) - 1))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    iEnd = Utl_StrFndChr(strResult, '\n', (iPos1 + 1));
    if(iEnd > (strlen(strResult) - 1))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    memset(DataLenBuff, 0, sizeof(DataLenBuff));
    Utl_StrMid(DataLenBuff, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iPos1 - 1));
    DataLenInt = atoi(DataLenBuff);   
    if(DataLenInt == 0)
    {
        *result = RIL_prCONNECT;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    else if((DataLenInt < 0) || (DataLenInt > 2048))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    RcvData = (char*)WIRELESS_MALLOC(DataLenInt);

    memcpy(RcvData, (strResult+iEnd+1), DataLenInt);
    *result = RIL_prOK;
    *strNum = DataLenInt;
    *strSize = DataLenInt;
    return RcvData;
}

_VOID* SIM800_WmPsV25_SendCipData(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 start = 0;
    _USHORT16 iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        return NULL;
    }

    start = Utl_StrFndL(strResult, "AT+CIPSEND=", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    start = Utl_StrFndL(strResult, ">", start, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    else
    { 
        *result = RIL_prOK;
        return NULL;
    }
}

_VOID* WaveC_WmPs0707_InquireCellLoc(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[32];
    _CHAR8   *cellLoc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("WaveC_WmPs0707_InquireCellLoc: strResult:\n %s SIM Existed to get location\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    
    start = Utl_StrFndL(strResult, "+QCELLLOC: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + strlen("+QCELLLOC: ");
    end = Utl_StrFndL(strResult, "\n", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));

    cellLoc = (char*)WIRELESS_MALLOC(sizeof(strTemp));

    strcpy(cellLoc, strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(cellLoc);
    WIRELESS_INFO("cellLoc:%s ; Len:%d.\n", cellLoc, *strSize);
    return cellLoc;
}

_VOID* MC323_ParseHCMGSOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 start = 0;
    _USHORT16 iLength = (_USHORT16)strlen(strResult);
    
    *strNum = 0;
    *strSize = 0;

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        WIRELESS_TRACE;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        WIRELESS_TRACE;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "AT^HCMGS=", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        WIRELESS_TRACE;
        return NULL;
    }
    start = Utl_StrFndL(strResult, ">", start, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        WIRELESS_TRACE;
        return NULL;
    }
    else
    { 
        *result = RIL_prOK;
        return NULL;
    }
}


/**
 * @brief Get AT+CMGW command response .
 * @brief parse ">" or "ERROR"
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_ParseCMGWOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);    
    *strNum = 0;
    *strSize = 0;
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        return NULL;
    }

    start = Utl_StrFndL(strResult, "AT+CMGW=", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    start = Utl_StrFndL(strResult, ">", start, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    else
    { 
        *result = RIL_prOK;
           return NULL;
    }
}


/**
 * @brief InquirePINStatus
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _SHORT16   iEnd = 0;    
    _CHAR8    iStatus[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CPIN_STATUS *strPIN = NULL;
    strPIN = (RIL_CPIN_STATUS *)WIRELESS_MALLOC(sizeof(RIL_CPIN_STATUS));
    memset(strPIN, 0, sizeof(RIL_CPIN_STATUS));

    //WIRELESS_INFO("WaveC_WmPs0707_InquirePinStatus: %s\r\n", strResult);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

#if 0 /* kinson.xiao 2011-09-27 */    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }
#endif

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    /* after use ATD**05*PUK*PIN*PIN to unlock PUK, send AT+CPIN?, module return many 0x0d,0x0a */    
    /*    if ((Utl_StrFnd(strResult, "AT+CPIN", AK_STRING_BEG)) != AK_STRING_BEG)*/
    if ((iBegin = Utl_StrFndL(strResult, "AT+CPIN", AK_STRING_BEG, iLength)) == -1)    
    {  
        WIRELESS_ERROR(WIRELESS_ERRORNO, "InquireInitStatus strResult: %s\r\n", strResult);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CPIN", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {
        /* Skip command string to get to first char of data.
        Find <CR> after the data string. */
        /*iBegin += AK_CPIN_SKIPCPIN_7;*/ /*Parse PIN2 response, error*/
        iBegin = Utl_StrFndL(strResult, " ", (_SHORT16)(iBegin + 6), iLength);
        iBegin++;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /* Verify length of data string is not empty. */
        if ((iEnd - iBegin) < 0)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        Utl_StrMid(iStatus, strResult, iBegin, (_SHORT16)(iEnd - 1));

        /* Check to see if PIN status is READY. */
        if (Utl_StrCmp((_CHAR8*)iStatus, "READY") == 0)
        {    
            *strPIN = RIL_READY;
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_CPIN_STATUS);
            return    (_VOID*)strPIN;
        }

        if ((Utl_StrFndL(strResult, "SIM", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN") == 0)
            {    
                *strPIN = RIL_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK") == 0)
            {    
                *strPIN = RIL_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN2") == 0)
            {    
                *strPIN = RIL_SIM_PIN2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK2") == 0)
            {
                *strPIN = RIL_SIM_PUK2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PIN") == 0)
            {
                *strPIN = RIL_PH_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PUK") == 0)
            {    
                *strPIN = RIL_PH_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
                
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PIN") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PUK") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NET", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PIN") == 0)
            {    
                *strPIN = RIL_PH_NET_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PUK") == 0)
            {
                *strPIN = RIL_PH_NET_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NS", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PIN") == 0)
            {    
                *strPIN = RIL_PH_NS_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PUK") == 0)
            {    
                *strPIN = RIL_PH_NS_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        if ((Utl_StrFndL(strResult, "PH-SP", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PUK") == 0)
            {
                *strPIN = RIL_PH_SP_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PIN") == 0)
            {    
                *strPIN = RIL_PH_SP_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-C", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PIN") == 0)
            {    
                *strPIN = RIL_PH_C_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PUK") == 0)
            {    
                *strPIN = RIL_PH_C_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        WIRELESS_TRACE;
        return NULL;
    }
} /* end WaveC_WmPs0707_InquirePinStatus */

_VOID* WaveC_WmPs0707_InquireInitStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _SHORT16   iEnd = 0;    
    _CHAR8    iStatus[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _UINT32    uiStatus = 0;

    RIL_INIT_STATUS *strInit = NULL;
    strInit = (RIL_INIT_STATUS *)WIRELESS_MALLOC(sizeof(RIL_INIT_STATUS));
    memset(strInit, 0, sizeof(RIL_INIT_STATUS));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strInit);
        return NULL;
    }

    WIRELESS_INFO("WaveC_WmPs0707_InquireInitStatus strResult : %s\r\n", strResult);
    
    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strInit);
        WIRELESS_TRACE;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *result = RIL_prGERROR;
        *strSize = 0;
        WIRELESS_FREE(strInit);
        return NULL;
    }

#if 0 /* kinson.xiao 2011-09-27 */    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }
#endif

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    /* changed by ZouMai: after use ATD**05*PUK*PIN*PIN to unlock PUK, send AT+CPIN?, module return many 0x0d,0x0a */    
    /*    if ((Utl_StrFnd(strResult, "AT+CPIN", AK_STRING_BEG)) != AK_STRING_BEG)*/
    if ((iBegin = Utl_StrFndL(strResult, "AT+QINISTAT", AK_STRING_BEG, iLength)) == -1)    
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireInitStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strInit);
        return NULL;
        
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+QINISTAT", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {
        /* Skip command string to get to first char of data.
        Find <CR> after the data string. */
        /*iBegin += AK_CPIN_SKIPCPIN_7; *//*Parse PIN2 response, error*/
        iBegin = Utl_StrFndL(strResult, " ", (_SHORT16)(iBegin + 6), iLength);
        iBegin++;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strInit);
            return NULL;
        }
        /* Verify length of data string is not empty. */
        if ((iEnd - iBegin) < 0)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strInit);
            return NULL;
        }
        Utl_StrMid(iStatus, strResult, iBegin, (_SHORT16)(iEnd - 1));

        uiStatus = (_SHORT16)Utl_Atoi(iStatus);

        *strInit = uiStatus;
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_INIT_STATUS);
        return    (_VOID*)strInit;
    }

    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strInit);
        WIRELESS_TRACE;
        return NULL;
    }
}


/**
 * @brief InquirePINStatus
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* C202_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _SHORT16   iEnd = 0;    
    _CHAR8    iStatus[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CPIN_STATUS *strPIN = NULL;
    strPIN = (RIL_CPIN_STATUS *)WIRELESS_MALLOC(sizeof(RIL_CPIN_STATUS));
    memset(strPIN, 0, sizeof(RIL_CPIN_STATUS));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *result = RIL_prGERROR;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

#if 0 /* kinson.xiao 2011-09-27 */    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }
#endif

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    /* changed by ZouMai: after use ATD**05*PUK*PIN*PIN to unlock PUK, send AT+CPIN?, module return many 0x0d,0x0a */    
    /*    if ((Utl_StrFnd(strResult, "AT+CPIN", AK_STRING_BEG)) != AK_STRING_BEG)*/
    if ((iBegin = Utl_StrFndL(strResult, "AT+CPIN", AK_STRING_BEG, iLength)) == -1)    
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CPIN", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {
        /* Skip command string to get to first char of data.
        Find <CR> after the data string. */
        /*iBegin += AK_CPIN_SKIPCPIN_7; *//*Parse PIN2 response, error*/
        iBegin = Utl_StrFndL(strResult, " ", (_SHORT16)(iBegin + 6), iLength);
        iBegin++;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /* Verify length of data string is not empty. */
        if ((iEnd - iBegin) < 0)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        Utl_StrMid(iStatus, strResult, iBegin, (_SHORT16)(iEnd - 1));

        /* Check to see if PIN status is READY. */
        if ((Utl_StrCmp((_CHAR8*)iStatus, "READY") == 0) || (Utl_StrCmp((_CHAR8*)iStatus, "Ready") == 0))
        {    
            *strPIN = RIL_READY;
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_CPIN_STATUS);
               return    (_VOID*)strPIN;
        }

        if ((Utl_StrFndL(strResult, "SIM", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN") == 0)
            {    
                *strPIN = RIL_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK") == 0)
            {    
                *strPIN = RIL_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN2") == 0)
            {    
                *strPIN = RIL_SIM_PIN2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK2") == 0)
            {
                *strPIN = RIL_SIM_PUK2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PIN") == 0)
            {
                *strPIN = RIL_PH_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PUK") == 0)
            {    
                *strPIN = RIL_PH_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
                
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PIN") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PUK") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NET", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PIN") == 0)
            {    
                *strPIN = RIL_PH_NET_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PUK") == 0)
            {
                *strPIN = RIL_PH_NET_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NS", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PIN") == 0)
            {    
                *strPIN = RIL_PH_NS_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PUK") == 0)
            {    
                *strPIN = RIL_PH_NS_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        if ((Utl_StrFndL(strResult, "PH-SP", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PUK") == 0)
            {
                *strPIN = RIL_PH_SP_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PIN") == 0)
            {    
                *strPIN = RIL_PH_SP_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-C", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PIN") == 0)
            {    
                *strPIN = RIL_PH_C_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PUK") == 0)
            {    
                *strPIN = RIL_PH_C_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
    }

    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
} /* end C202_WmPs0707_InquirePinStatus */

/**
 * @brief InquirePINStatus
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* MC323_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _SHORT16   iEnd = 0;    
    _CHAR8    iStatus[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CPIN_STATUS *strPIN = NULL;
    strPIN = (RIL_CPIN_STATUS *)WIRELESS_MALLOC(sizeof(RIL_CPIN_STATUS));
    memset(strPIN, 0, sizeof(RIL_CPIN_STATUS));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("MC323 Get Pin Error: %d\r\n", strResult);
        *result = RIL_prGERROR;  
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
#if 0 /* kinson.xiao 2012-06-17 */    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
#endif    

#if 0 /* kinson.xiao 2011-09-27 */    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }
#endif

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    /* changed by ZouMai: after use ATD**05*PUK*PIN*PIN to unlock PUK, send AT+CPIN?, module return many 0x0d,0x0a */    
    /*    if ((Utl_StrFnd(strResult, "AT+CPIN", AK_STRING_BEG)) != AK_STRING_BEG)*/
    if ((iBegin = Utl_StrFndL(strResult, "AT+CPIN", AK_STRING_BEG, iLength)) == -1)    
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CPIN", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {
        /* Skip command string to get to first char of data.
        Find <CR> after the data string. */
        /*iBegin += AK_CPIN_SKIPCPIN_7; *//*Parse PIN2 response, error*/
        /* MC323 Response No Space 
        iBegin = Utl_StrFndL(strResult, " ", (_SHORT16)(iBegin + 6), iLength);
        iBegin++;
        */
        iBegin += 6;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /* Verify length of data string is not empty. */
        if ((iEnd - iBegin) < 0)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        Utl_StrMid(iStatus, strResult, iBegin, (_SHORT16)(iEnd - 1));

        /* Check to see if PIN status is READY. */
        if (Utl_StrCmp((_CHAR8*)iStatus, "READY") == 0)
        {    
            *strPIN = RIL_READY;
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_CPIN_STATUS);
               return    (_VOID*)strPIN;
        }
        else
        {
            WIRELESS_INFO("Cann't find ready\r\n");
        }

        if ((Utl_StrFndL(strResult, "SIM", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN") == 0)
            {    
                *strPIN = RIL_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK") == 0)
            {    
                *strPIN = RIL_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN2") == 0)
            {    
                *strPIN = RIL_SIM_PIN2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK2") == 0)
            {
                *strPIN = RIL_SIM_PUK2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PIN") == 0)
            {
                *strPIN = RIL_PH_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PUK") == 0)
            {    
                *strPIN = RIL_PH_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
                
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PIN") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PUK") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NET", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PIN") == 0)
            {    
                *strPIN = RIL_PH_NET_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PUK") == 0)
            {
                *strPIN = RIL_PH_NET_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NS", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PIN") == 0)
            {    
                *strPIN = RIL_PH_NS_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PUK") == 0)
            {    
                *strPIN = RIL_PH_NS_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }
        if ((Utl_StrFndL(strResult, "PH-SP", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PUK") == 0)
            {
                *strPIN = RIL_PH_SP_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PIN") == 0)
            {    
                *strPIN = RIL_PH_SP_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-C", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PIN") == 0)
            {    
                *strPIN = RIL_PH_C_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PUK") == 0)
            {    
                *strPIN = RIL_PH_C_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            WIRELESS_TRACE;
            return NULL;
        }
    }

    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        WIRELESS_TRACE;
        return NULL;
    }
} /* end MC323_WmPs0707_InquirePinStatus */

/**
 * @brief InquirePINStatus
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* CM300_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _SHORT16   iEnd = 0;    
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CPIN_STATUS *strPIN = NULL;
    strPIN = (RIL_CPIN_STATUS *)WIRELESS_MALLOC(sizeof(RIL_CPIN_STATUS));
    memset(strPIN, 0, sizeof(RIL_CPIN_STATUS));
    WIRELESS_INFO("Call CM300_WmPs0707_InquirePinStatus, strResult = %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("CM300 Get Pin Error: %d\r\n", strResult);
        
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
#if 0 /* kinson.xiao 2012-06-17 */    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
#endif    

#if 0 /* kinson.xiao 2011-09-27 */    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }
#endif

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    /* changed by ZouMai: after use ATD**05*PUK*PIN*PIN to unlock PUK, send AT+CPIN?, module return many 0x0d,0x0a */    
    /*    if ((Utl_StrFnd(strResult, "AT+CPIN", AK_STRING_BEG)) != AK_STRING_BEG)*/
    if ((iBegin = Utl_StrFndL(strResult, "AT$HPINS", AK_STRING_BEG, iLength)) == -1)    
    {  
        AK_FUNCTION_RET_INT("CM300_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "$HPINS", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {
        /* Skip command string to get to first char of data.
        Find <CR> after the data string. */
        /*iBegin += AK_CPIN_SKIPCPIN_7; *//*Parse PIN2 response, error*/
        /* MC323 Response No Space 
        iBegin = Utl_StrFndL(strResult, " ", (_SHORT16)(iBegin + 6), iLength);
        iBegin++;
        */
        iBegin += 6;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /* Verify length of data string is not empty. */
        if ((iEnd - iBegin) < 0)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /*
        Utl_StrMid(iStatus, strResult, iBegin, (_SHORT16)(iEnd - 1));
        */

        /* Check to see if PIN status is READY. */
        if (strstr((_CHAR8*)strResult, "ROMSIM") != NULL)
        {    
            WIRELESS_INFO("No Sim\r\n");
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            return NULL;        
        }
        else
        {
            *strPIN = RIL_READY;
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_CPIN_STATUS);
            WIRELESS_INFO("SIM Exist\r\n");
               return    (_VOID*)strPIN;    
        }
        #if 0
        if ((Utl_StrFndL(strResult, "SIM", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN") == 0)
            {    
                *strPIN = RIL_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK") == 0)
            {    
                *strPIN = RIL_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN2") == 0)
            {    
                *strPIN = RIL_SIM_PIN2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK2") == 0)
            {
                *strPIN = RIL_SIM_PUK2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PIN") == 0)
            {
                *strPIN = RIL_PH_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PUK") == 0)
            {    
                *strPIN = RIL_PH_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
                
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PIN") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PUK") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NET", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PIN") == 0)
            {    
                *strPIN = RIL_PH_NET_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PUK") == 0)
            {
                *strPIN = RIL_PH_NET_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NS", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PIN") == 0)
            {    
                *strPIN = RIL_PH_NS_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PUK") == 0)
            {    
                *strPIN = RIL_PH_NS_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }
        if ((Utl_StrFndL(strResult, "PH-SP", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PUK") == 0)
            {
                *strPIN = RIL_PH_SP_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PIN") == 0)
            {    
                *strPIN = RIL_PH_SP_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-C", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PIN") == 0)
            {    
                *strPIN = RIL_PH_C_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PUK") == 0)
            {    
                *strPIN = RIL_PH_C_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            WIRELESS_TRACE;
            return NULL;
        }
        #endif
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        WIRELESS_TRACE;
        return NULL;
    }
}

_VOID* MC600_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _SHORT16   iEnd = 0;    
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CPIN_STATUS *strPIN = NULL;
    strPIN = (RIL_CPIN_STATUS *)WIRELESS_MALLOC(sizeof(RIL_CPIN_STATUS));
    memset(strPIN, 0, sizeof(RIL_CPIN_STATUS));
    WIRELESS_INFO("Call MC600_WmPs0707_InquirePinStatus, strResult = %s\r\n", strResult);


    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("CM300 Get Pin Error: %d\r\n", strResult);
        
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
    }


    
    *strPIN = RIL_READY;
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_CPIN_STATUS);
    WIRELESS_INFO("SIM Exist\r\n");
       return     (_VOID*)strPIN;    

#if 0 /* kinson.xiao 2012-06-17 */    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
    }
#endif    

#if 0 /* kinson.xiao 2011-09-27 */    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }
#endif

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    /* changed by ZouMai: after use ATD**05*PUK*PIN*PIN to unlock PUK, send AT+CPIN?, module return many 0x0d,0x0a */    
    /*    if ((Utl_StrFnd(strResult, "AT+CPIN", AK_STRING_BEG)) != AK_STRING_BEG)*/
    if ((iBegin = Utl_StrFndL(strResult, "AT$HPINS", AK_STRING_BEG, iLength)) == -1)    
    {  
        AK_FUNCTION_RET_INT("CM300_WmPs0707_InquirePinStatus", RIL_prERROR);
        
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPIN);
        return NULL;
        
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "$HPINS", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {
        /* Skip command string to get to first char of data.
        Find <CR> after the data string. */
        /*iBegin += AK_CPIN_SKIPCPIN_7; *//*Parse PIN2 response, error*/
        /* MC323 Response No Space 
        iBegin = Utl_StrFndL(strResult, " ", (_SHORT16)(iBegin + 6), iLength);
        iBegin++;
        */
        iBegin += 6;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /* Verify length of data string is not empty. */
        if ((iEnd - iBegin) < 0)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strPIN);
            return NULL;
        }
        /*
        Utl_StrMid(iStatus, strResult, iBegin, (_SHORT16)(iEnd - 1));
        */

        /* Check to see if PIN status is READY. */
        if (strstr((_CHAR8*)strResult, "ROMSIM") != NULL)
        {    
            WIRELESS_INFO("No Sim\r\n");
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            return NULL;        
        }
        else
        {
            *strPIN = RIL_READY;
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_CPIN_STATUS);
            WIRELESS_INFO("SIM Exist\r\n");
               return    (_VOID*)strPIN;    
        }
        #if 0
        if ((Utl_StrFndL(strResult, "SIM", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN") == 0)
            {    
                *strPIN = RIL_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK") == 0)
            {    
                *strPIN = RIL_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PIN2") == 0)
            {    
                *strPIN = RIL_SIM_PIN2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "SIM PUK2") == 0)
            {
                *strPIN = RIL_SIM_PUK2;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PIN") == 0)
            {
                *strPIN = RIL_PH_SIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SIM PUK") == 0)
            {    
                *strPIN = RIL_PH_SIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
                
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PIN") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-FSIM PUK") == 0)
            {    
                *strPIN = RIL_PH_FSIM_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NET", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PIN") == 0)
            {    
                *strPIN = RIL_PH_NET_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NET PUK") == 0)
            {
                *strPIN = RIL_PH_NET_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-NS", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PIN") == 0)
            {    
                *strPIN = RIL_PH_NS_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-NS PUK") == 0)
            {    
                *strPIN = RIL_PH_NS_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }
        if ((Utl_StrFndL(strResult, "PH-SP", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PUK") == 0)
            {
                *strPIN = RIL_PH_SP_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                WIRELESS_TRACE;
                *strSize = sizeof(RIL_CPIN_STATUS);
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-SP PIN") == 0)
            {    
                *strPIN = RIL_PH_SP_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strPIN);
                WIRELESS_TRACE;
                return NULL;
            }
        }

        if ((Utl_StrFndL(strResult, "PH-C", iBegin, iLength)) != -1)
        {
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PIN") == 0)
            {    
                *strPIN = RIL_PH_C_PIN;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            if (Utl_StrCmp((_CHAR8*)iStatus, "PH-C PUK") == 0)
            {    
                *strPIN = RIL_PH_C_PUK;
                *result = RIL_prOK;
                *strNum = 1;
                *strSize = sizeof(RIL_CPIN_STATUS);
                WIRELESS_TRACE;
                return    (_VOID*)strPIN;
            }
            else
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strPIN);
                return NULL;
            }
        }
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strPIN);
            WIRELESS_TRACE;
            return NULL;
        }
        #endif
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strPIN);
        WIRELESS_TRACE;
        return NULL;
    }
}

_VOID* WaveC_WmPs0707_SetPin(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16  iLength = (_USHORT16)strlen(strResult);
    
    //WIRELESS_INFO("WaveC_WmPs0707_SetPin, strResult = %s\r\n", strResult);
    if (strResult == NULL)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    if(Utl_StrFndL(strResult, "ERROR", 0, iLength) != -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    *result = RIL_prOK;
    *strNum = 0;
    *strSize = 0;
    WIRELESS_TRACE;
    return NULL;
}

_VOID* WaveC_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _CHAR8      RegStatus[8];
    _INT32      iPos1 = 0;
    RIL_NET_STAT *strNet = NULL;
    
    //WIRELESS_INFO("Call InquireRegisterStatus, strResult = %s\r\n", strResult);
    memset(RegStatus, 0, sizeof(RegStatus));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("M35 Get Reg Error: %d\r\n", strResult);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if ((iBegin = Utl_StrFndL(strResult, "AT+CREG", AK_STRING_BEG, iLength)) == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //printf("Call InquireRegisterStatus, strResult = %s\r\n", strResult);
       // WIRELESS_TRACE;
        return NULL;        
    }
    
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 10));
    if(iPos1 > (strlen(strResult) - 1))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    Utl_StrMid(RegStatus, strResult, (_SHORT16)(iPos1 + 1), (_SHORT16)(iPos1 + 1));
    if(RegStatus[0] > '5' || RegStatus[0] < '0')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    strNet = (RIL_NET_STAT *)WIRELESS_MALLOC(sizeof(RIL_NET_STAT));
    memset(strNet, 0, sizeof(RIL_NET_STAT));
    
    *strNet = (RIL_NET_MODE)Utl_Atoi(RegStatus);
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_NET_STAT);
    return      (_VOID*)strNet;
}

_VOID* FIBOCOM_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _CHAR8      RegStatus[8];
    _INT32      iPos1 = 0;
    RIL_NET_STAT *strNet = NULL;
    
    //WIRELESS_INFO("Call FibInquireRegisterStatus, strResult = %s\r\n", strResult);
    memset(RegStatus, 0, sizeof(RegStatus));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("L810 Get Reg Error: %d\r\n", strResult);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        return NULL;
    }

    if ((iBegin = Utl_StrFndL(strResult, "AT+CGREG", AK_STRING_BEG, iLength)) == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //printf("Call InquireRegisterStatus, strResult = %s\r\n", strResult);
       // WIRELESS_TRACE;
        return NULL;        
    }
    
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 10));
    if(iPos1 > (strlen(strResult) - 1))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    Utl_StrMid(RegStatus, strResult, (_SHORT16)(iPos1 + 1), (_SHORT16)(iPos1 + 1));
    if(RegStatus[0] > '5' || RegStatus[0] < '0')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    strNet = (RIL_NET_STAT *)WIRELESS_MALLOC(sizeof(RIL_NET_STAT));
    memset(strNet, 0, sizeof(RIL_NET_STAT));
    
    *strNet = (RIL_NET_MODE)Utl_Atoi(RegStatus);
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_NET_STAT);
    return      (_VOID*)strNet;
}

_VOID* FIBOCOM_WmPs0707_InquireRegisterStatus_With_Cid(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _CHAR8      RegStatus[8];
    _INT32      iPos1 = 0;
    _INT32      iPos2 = 0;
    RIL_NET_STAT_WITH_CID *strNetWithCid = NULL;
    _CHAR8        strTemp[256];
    _UINT32 uiLac = 0;
    _UINT32 uiCellId = 0;
    
    memset(RegStatus, 0, sizeof(RegStatus));
    strNetWithCid = (RIL_NET_STAT_WITH_CID *)WIRELESS_MALLOC(sizeof(RIL_NET_STAT_WITH_CID));
    memset(strNetWithCid, 0, sizeof(RIL_NET_STAT_WITH_CID));

    WIRELESS_INFO("InquireRegisterStatus_With_Cid, strResult = %s\r\n", strResult);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        WIRELESS_TRACE;
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("L810 Get Reg Error: %d\r\n", strResult);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        //WIRELESS_TRACE;
        return NULL;
    }

    if ((iBegin = Utl_StrFndL(strResult, "AT+CGREG", AK_STRING_BEG, iLength)) == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        //printf("Call InquireRegisterStatus, strResult = %s\r\n", strResult);
       // WIRELESS_TRACE;
        return NULL;        
    }
    
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 10));
    if(iPos1 > (strlen(strResult) - 1))
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        WIRELESS_TRACE;
        return NULL;
    }

    Utl_StrMid(RegStatus, strResult, (_SHORT16)(iPos1 + 1), (_SHORT16)(iPos1 + 1));
    if(RegStatus[0] > '5' || RegStatus[0] < '0')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        WIRELESS_TRACE;
        return NULL;
    }
    
    strNetWithCid->netstat = (RIL_NET_MODE)Utl_Atoi(RegStatus);
    
    iPos2 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 1));
    if(iPos2 == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        WIRELESS_TRACE;
        return NULL;
    }

    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos2 + 1));
    if(iPos1 == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        WIRELESS_TRACE;
        return NULL;
    }    

    memset(strTemp, 0, sizeof(strTemp));    
    Utl_StrMid(strTemp, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iPos1 - 2));
    sscanf(strTemp, "%x", &uiLac);
    strNetWithCid->lac = uiLac;

    iPos2 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iPos1 + 2));
    if(iPos2 == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strNetWithCid);
        WIRELESS_TRACE;
        return NULL;
    }    
    
    memset(strTemp, 0, sizeof(strTemp));    
    Utl_StrMid(strTemp, strResult, (_SHORT16)(iPos1 + 2), (_SHORT16)(iPos2 - 1));
    sscanf(strTemp, "%x", &uiCellId);

    sprintf(strNetWithCid->cellid, "%d", uiCellId);
    
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_NET_STAT_WITH_CID);
    return      (_VOID*)strNetWithCid;
}


_VOID* CM300_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("Call CM300_WmPs0707_InquireRegisterStatus, strResult = %s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("CM300 Get Reg Error: %d\r\n", strResult);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    if ((iBegin = Utl_StrFndL(strResult, "AT$HSTATE", AK_STRING_BEG, iLength)) == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;        
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "$HSTATE: 2", (_SHORT16)(AK_CPIN_SKIPCPIN_7 + iBegin), iLength);

    if (iBegin != -1)
    {    
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = 0;
        WIRELESS_INFO("CM300 Register\r\n");
        return    NULL;    
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_INFO("CM300 didn't Register\r\n");
        return NULL;
    }
}

_VOID* MC600_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin = 0;
    _USHORT16    iLength = (_USHORT16)strlen(strResult);
    _CHAR8        RegStatus[8];
    _INT32        iPos1 = 0;
    RIL_NET_STAT *strNet = NULL;
    
    WIRELESS_INFO("Call MC600_WmPs0707_InquireRegisterStatus, strResult = %s\r\n", strResult);
    memset(RegStatus, 0, sizeof(RegStatus));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if(iLength < AK_OK_POS_5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if((iBegin = Utl_StrFndL(strResult, "+CME ERROR:", 0, iLength)) != -1)
    {
        WIRELESS_INFO("MC600 Get Reg Error: %d\r\n", strResult);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    if ((iBegin = Utl_StrFndL(strResult, "+CAD:", AK_STRING_BEG, iLength)) == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;        
    }
    
    Utl_StrMid(RegStatus, strResult, (_SHORT16)(iBegin + 6), (_SHORT16)(iBegin + 6));

    
    strNet = (RIL_NET_STAT *)WIRELESS_MALLOC(sizeof(RIL_NET_STAT));
    memset(strNet, 0, sizeof(RIL_NET_STAT));
    
    *strNet = (RIL_NET_MODE)Utl_Atoi(RegStatus);
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_NET_STAT);
    return      (_VOID*)strNet;
}


/**
 * @brief ListCurrentCalls
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd;
    _CHAR8    iTemp[16];
    _UCHAR8    iNum = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CLCC_ENTRY    *callEntry = NULL;
    
    callEntry = (RIL_CLCC_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    memset(callEntry, 0, sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    if (Utl_StrFndL(strResult, "AT+CLCC", 0, iLength) == -1)
    {    
        WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(callEntry);
        return NULL;
    }


    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(callEntry);
        return NULL;
    }
    else
    {/*if have found "OK"*/
        if((iLength < 12) && (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1))
        {/*wavecom return error CCLC*/
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

    }

    /*
    printf("strResult:%s\r\n", strResult);
    */
    iBegin = Utl_StrFndL(strResult, "+CLCC:", AK_CLCC_ATCLCCLEN_7, iLength);

    while(iBegin != -1)
    {
        /*Get index*/
        iBegin = iBegin + AK_CLCC_CLCCLEN_7;
        iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
        if(iEnd <= iBegin)
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

        Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].idx = (_SHORT16)Utl_Atoi(iTemp);

        iBegin = iEnd+1;
        if(strResult[iBegin] != '0' && strResult[iBegin] != '1')
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].dir = (RIL_TELE_ORIGNATE)(strResult[iBegin] - '0');

        iBegin++;
        if(strResult[iBegin] != ',')
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin]  > '5' || strResult[iBegin] < '0')
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].stat = (RIL_CALL_STATUS)(strResult[iBegin] - '0');

        /*Get mode*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(!((strResult[iBegin] <= '2' && strResult[iBegin] >= '0') || strResult[iBegin] == '9'))
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        if(strResult[iBegin] == '9')
        {
            callEntry[iNum].mode = RIL_UNKNOWN;
        }
        else
        {
            callEntry[iNum].mode = (RIL_CALL_MODE)(strResult[iBegin] - '0');
        }
        /*Get the mpty of the entry*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin] > '1' || strResult[iBegin] < '0')
        {
            WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].mpty = (RIL_CALL_PARTY)(strResult[iBegin] - '0');

        iBegin++;
        /*This clcc line has number info*/
        if(strResult[iBegin] == ',')
        {
            iBegin++;
            if(strResult[iBegin] != '\"')
            {
                WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if (iEnd - iBegin > G_MaxTeleLen/*gb_pb.ucMaxTeleLen*/)
            {
                Utl_StrMidL(callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen/*gb_pb.ucMaxTeleLen*/ - 1), iLength);
            }
            else
            {
                Utl_StrMidL(callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }
    
            /*get number type*/
            iBegin = iEnd + 1;
            if(strResult[iBegin] != ',')
            {
                WIRELESS_ERROR(WIRELESS_ERRORNO, "ParseClcc Error(%s),(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iBegin + AK_CLCC_TYPELEN_3 - 1), iLength);
            if(Utl_StrCmp(iTemp, "145") == 0)
            {
                callEntry[iNum].type = RIL_INTER;
            }
            else
            {
                callEntry[iNum].type = RIL_OTHERS;
            }
        }
        else
        {
            callEntry[iNum].number[ 0 ] = 0;
            callEntry[iNum].type = RIL_INTER;
        }

        iNum++;
        iBegin = Utl_StrFndL(strResult, "+CLCC:", iBegin, iLength);
    }

    *strNum = iNum;
    *result = RIL_prOK;
    *strSize = sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM;
    return (_VOID*)callEntry;
} /* end WaveC_WmPs0707_ListCurCalls */

/**
 * @brief ListCurrentCalls
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* C202_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin = 0;
    _CHAR8    iTemp[5];
    _UCHAR8    iNum = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CLCC_ENTRY    *callEntry = NULL;
    
    WIRELESS_INFO("C202_WmPs0707_ListCurCalls (%s)(%d)\r\n", __FILE__, __LINE__);
    callEntry = (RIL_CLCC_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    memset(callEntry, 0, sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    WIRELESS_INFO("C202_WmPs0707_ListCurCalls (%s)(%d)\r\n", __FILE__, __LINE__);
    #if 1
    if (Utl_StrFndL(strResult, "AT+CLCC", 0, iLength) == -1)
    {
           *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }


    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }
    else
    {/*if have found "OK"*/
        if((iLength < 12) && (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1))
        {/*wavecom return error CCLC*/
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

    }
    #endif

    iBegin = Utl_StrFndL(strResult, "+CLCC:", AK_CLCC_ATCLCCLEN_7, iLength);

#if 0
    while(iBegin != -1)
    {
        /*Get index*/
        iBegin = iBegin + AK_CLCC_CLCCLEN_7;
        iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
        if(iEnd <= iBegin)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

        Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].idx = (_SHORT16)Utl_Atoi(iTemp);

        iBegin = iEnd+1;
        if(strResult[iBegin] != '0' && strResult[iBegin] != '1')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].dir = (RIL_TELE_ORIGNATE)(strResult[iBegin] - '0');

        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin]  > '5' || strResult[iBegin] < '0')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].stat = (RIL_CALL_STATUS)(strResult[iBegin] - '0');

        /*Get mode*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(!((strResult[iBegin] <= '2' && strResult[iBegin] >= '0') || strResult[iBegin] == '9'))
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        if(strResult[iBegin] == '9')
        {
            callEntry[iNum].mode = RIL_UNKNOWN;
        }
        else
        {
            callEntry[iNum].mode = (RIL_CALL_MODE)(strResult[iBegin] - '0');
        }
        /*Get the mpty of the entry*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin] > '1' || strResult[iBegin] < '0')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].mpty = (RIL_CALL_PARTY)(strResult[iBegin] - '0');

        iBegin++;
        /*This clcc line has number info*/
        if(strResult[iBegin] == ',')
        {
            iBegin++;
            if(strResult[iBegin] != '\"')
            {
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if (iEnd - iBegin > G_MaxTeleLen/*gb_pb.ucMaxTeleLen*/)
            {
                Utl_StrMidL((_CHAR8*)callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen/*gb_pb.ucMaxTeleLen*/ - 1), iLength);
            }
            else
            {
                Utl_StrMidL((_CHAR8*)callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }
    
            /*get number type*/
            iBegin = iEnd + 1;
            if(strResult[iBegin] != ',')
            {
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iBegin + AK_CLCC_TYPELEN_3 - 1), iLength);
            if(Utl_StrCmp(iTemp, "145") == 0)
            {
                callEntry[iNum].type = RIL_INTER;
            }
            else
            {
                callEntry[iNum].type = RIL_OTHERS;
            }
        }
        else
        {
            callEntry[iNum].number[ 0 ] = 0;
            callEntry[iNum].type = RIL_INTER;
        }

        iNum++;
        iBegin = Utl_StrFndL(strResult, "+CLCC:", iBegin, iLength);
    }
#endif

    *strNum = iNum;
    *result = RIL_prOK;
    *strSize = sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM;
    return (_VOID*)callEntry;
} /* end C202_WmPs0707_ListCurCalls */



/**
 * @brief ListCurrentCalls
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* CM300_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin = 0;
    _CHAR8    iTemp[5];
    _UCHAR8    iNum = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CLCC_ENTRY    *callEntry = NULL;
    
    WIRELESS_INFO("CM300_WmPs0707_ListCurCalls (%s)(%d)\r\n", __FILE__, __LINE__);
    callEntry = (RIL_CLCC_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    memset(callEntry, 0, sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    WIRELESS_INFO("CM300_WmPs0707_ListCurCalls (%s)(%d)\r\n", __FILE__, __LINE__);
    #if 0
    if (Utl_StrFndL(strResult, "AT+CLCC", 0, iLength) == -1)
    {
           *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }


    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }
    else
    {/*if have found "OK"*/
        if((iLength < 12) && (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1))
        {/*wavecom return error CCLC*/
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

    }
    #endif

    iBegin = Utl_StrFndL(strResult, "+CLCC:", AK_CLCC_ATCLCCLEN_7, iLength);

#if 0
    while(iBegin != -1)
    {
        /*Get index*/
        iBegin = iBegin + AK_CLCC_CLCCLEN_7;
        iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
        if(iEnd <= iBegin)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

        Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].idx = (_SHORT16)Utl_Atoi(iTemp);

        iBegin = iEnd+1;
        if(strResult[iBegin] != '0' && strResult[iBegin] != '1')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].dir = (RIL_TELE_ORIGNATE)(strResult[iBegin] - '0');

        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin]  > '5' || strResult[iBegin] < '0')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].stat = (RIL_CALL_STATUS)(strResult[iBegin] - '0');

        /*Get mode*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(!((strResult[iBegin] <= '2' && strResult[iBegin] >= '0') || strResult[iBegin] == '9'))
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        if(strResult[iBegin] == '9')
        {
            callEntry[iNum].mode = RIL_UNKNOWN;
        }
        else
        {
            callEntry[iNum].mode = (RIL_CALL_MODE)(strResult[iBegin] - '0');
        }
        /*Get the mpty of the entry*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin] > '1' || strResult[iBegin] < '0')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].mpty = (RIL_CALL_PARTY)(strResult[iBegin] - '0');

        iBegin++;
        /*This clcc line has number info*/
        if(strResult[iBegin] == ',')
        {
            iBegin++;
            if(strResult[iBegin] != '\"')
            {
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if (iEnd - iBegin > G_MaxTeleLen/*gb_pb.ucMaxTeleLen*/)
            {
                Utl_StrMidL((_CHAR8*)callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen/*gb_pb.ucMaxTeleLen*/ - 1), iLength);
            }
            else
            {
                Utl_StrMidL((_CHAR8*)callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }
    
            /*get number type*/
            iBegin = iEnd + 1;
            if(strResult[iBegin] != ',')
            {
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iBegin + AK_CLCC_TYPELEN_3 - 1), iLength);
            if(Utl_StrCmp(iTemp, "145") == 0)
            {
                callEntry[iNum].type = RIL_INTER;
            }
            else
            {
                callEntry[iNum].type = RIL_OTHERS;
            }
        }
        else
        {
            callEntry[iNum].number[ 0 ] = 0;
            callEntry[iNum].type = RIL_INTER;
        }

        iNum++;
        iBegin = Utl_StrFndL(strResult, "+CLCC:", iBegin, iLength);
    }
#endif

    *strNum = iNum;
    *result = RIL_prOK;
    *strSize = sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM;
    return (_VOID*)callEntry;
}



/**
 * @brief ListCurrentCalls
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* MC323_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd;
    _CHAR8    iTemp[16];
    _UCHAR8    iNum = 0;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CLCC_ENTRY    *callEntry = NULL;
    
    WIRELESS_INFO("MC323_WmPs0707_ListCurCalls:%s\r\n", strResult);
    callEntry = (RIL_CLCC_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    memset(callEntry, 0, sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    if (Utl_StrFndL(strResult, "AT+CLCC", 0, iLength) == -1)
    {
           *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }

    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(callEntry);
        return NULL;
    }
    else
    {/*if have found "OK"*/
        if((iLength < 12) && (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1))
        {/*wavecom return error CCLC*/
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

    }

    iBegin = Utl_StrFndL(strResult, "+CLCC:", AK_CLCC_ATCLCCLEN_7, iLength);

    while(iBegin != -1)
    {
        /*Get index*/
        iBegin = iBegin + AK_CLCC_CLCCLEN_6;
        iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
        if(iEnd <= iBegin)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }

        Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].idx = (_SHORT16)Utl_Atoi(iTemp);
        WIRELESS_INFO("CALL INDEX(%d):%d\r\n", iNum, callEntry[iNum].idx);

        iBegin = iEnd+1;
        if(strResult[iBegin] != '0' && strResult[iBegin] != '1')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].dir = (RIL_TELE_ORIGNATE)(strResult[iBegin] - '0');

        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin]  > '5' || strResult[iBegin] < '0')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].stat = (RIL_CALL_STATUS)(strResult[iBegin] - '0');

        /*Get mode*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(!((strResult[iBegin] <= '2' && strResult[iBegin] >= '0') || strResult[iBegin] == '9'))
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        if(strResult[iBegin] == '9')
        {
            callEntry[iNum].mode = RIL_UNKNOWN;
        }
        else
        {
            callEntry[iNum].mode = (RIL_CALL_MODE)(strResult[iBegin] - '0');
        }
        /*Get the mpty of the entry*/
        iBegin++;
        if(strResult[iBegin] != ',')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        iBegin++;
        if(strResult[iBegin] > '1' || strResult[iBegin] < '0')
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(callEntry);
            return NULL;
        }
        callEntry[iNum].mpty = (RIL_CALL_PARTY)(strResult[iBegin] - '0');

        iBegin++;
        /*This clcc line has number info*/
        if(strResult[iBegin] == ',')
        {
            iBegin++;
            if(strResult[iBegin] != '\"')
            {
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if ((iEnd - iBegin) > G_MaxTeleLen)
            {
                Utl_StrMidL(callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen - 1), iLength);
            }
            else
            {
                Utl_StrMidL(callEntry[iNum].number, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }

            /*get number type*/
            iBegin = iEnd + 1;
            if(strResult[iBegin] != ',')
            {
                *result = RIL_prERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(callEntry);
                return NULL;
            }
            iBegin++;
            Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iBegin + AK_CLCC_TYPELEN_3 - 1), iLength);
            if(Utl_StrCmp(iTemp, "145") == 0)
            {
                callEntry[iNum].type = RIL_INTER;
            }
            else
            {
                callEntry[iNum].type = RIL_OTHERS;
            }
        }
        else
        {
            callEntry[iNum].number[ 0 ] = 0;
            callEntry[iNum].type = RIL_INTER;
        }

        iNum++;
        iBegin = Utl_StrFndL(strResult, "+CLCC:", iBegin, iLength);
    }

    *strNum = iNum;
    *result = RIL_prOK;
    *strSize = sizeof(RIL_CLCC_ENTRY) * MAX_CALL_NUM;
    return (_VOID*)callEntry;
}/* end MC323_WmPs0707_ListCurCalls */


/**
 * @brief InquireClock
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_InquireClock(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   iBegin;
    _CHAR8    iyear[8], imon[8], iday[8];
    _CHAR8    ihour[8], imin[8], isec[8];
    _SHORT16    year, mon, day, hour, min, sec;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_SYSTIME *strClock = NULL;
    strClock = (RIL_SYSTIME *)WIRELESS_MALLOC(sizeof(RIL_SYSTIME));
    memset(strClock, 0, sizeof(RIL_SYSTIME));
    if (strResult == NULL)
     { 
         *result = RIL_prNULL;
           *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strClock);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strClock);
        return NULL;
    }

    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if (Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength) == -1)
    {   AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireClock", RIL_prERROR);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strClock);
        return NULL;
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CCLK:", AK_STRING_BEG, iLength);
    if (iBegin != -1)
    {
        /* Start checking on date format.*/
        /* Find first quotation mark in beginning of date. Else, return error. */
        iBegin += 7;
        if(strResult[ iBegin ] != '\"')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        iBegin++;
        Utl_StrMidL(iyear, strResult, iBegin, (_SHORT16)(iBegin + 1), iLength);

        /* Verify year data are valid digits. */
        if ((iyear[0] < '0') || (iyear[0] > '9') || (iyear[1] < '0') || (iyear[1] > '9'))
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Convert year data from string to integer. */
        year = (_SHORT16)Utl_Atoi(iyear);
        year += INITIALYEAR;

        iBegin += 2;
        if(strResult[ iBegin++ ] != '/')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Check validity of month.*/
        Utl_StrMidL(imon, strResult, iBegin, (_SHORT16)(iBegin + 1), iLength);

        /* Verify month data are valid digits. */
        if ((imon[0] < '0') || (imon[0] > '9') || (imon[1] < '0') || (imon[1] > '9'))
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Convert month data from string to integer. Verify valid month. */
        mon = (_SHORT16)Utl_Atoi(imon);
        if (mon<1 || mon>12)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        iBegin += 2;
        if(strResult[ iBegin++ ] != '/')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Check validity of day.*/
        Utl_StrMidL(iday, strResult, iBegin, (_SHORT16)(iBegin + 1), iLength);
        /* Verify day data are valid digits. */
        if ((iday[0] < '0') || (iday[0] > '9') || (iday[1] < '0') || (iday[1] > '9'))
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Convert day data from string to integer. Verify valid day of month. */
        day = (_SHORT16)Utl_Atoi(iday);
        if (day<1 || day> 31)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        iBegin += 2;
        if(strResult[ iBegin++ ] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Check validity of hour.*/
        Utl_StrMidL(ihour, strResult, iBegin, (_SHORT16)(iBegin + 1), iLength);
        /* Verify hour data are valid digits. */
        if ((ihour[0] < '0') || (ihour[0] > '9') || (ihour[1] < '0') || (ihour[1] > '9'))
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }
        /* Convert hour data from string to integer. Verify valid hour. */
        hour = (_SHORT16)Utl_Atoi(ihour);
        if (hour < 0 || hour > 23)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        iBegin += 2;
        if(strResult[ iBegin++ ] != ':')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Check validity of minute.*/
        Utl_StrMidL(imin, strResult, iBegin, (_SHORT16)(iBegin + 1), iLength);
        /* Verify minute data are valid digits. */
        if ((imin[0] < '0') || (imin[0] > '9') || (imin[1] < '0') || (imin[1] > '9'))
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }
        /* Convert minute data from string to integer. Verify valid minute. */
        min = (_SHORT16)Utl_Atoi(imin);
        if (min < 0 || min > 59)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        iBegin += 2;
        if(strResult[ iBegin++ ] != ':')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Check validity of second.*/
        Utl_StrMidL(isec, strResult, iBegin, (_SHORT16)(iBegin + 1), iLength);

        /* Verify second data are valid digits. */
        if ((isec[0] < '0') || (isec[0] > '9') || (isec[1] < '0') || (isec[1] > '9'))
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* Convert second data from string to integer. Verify valid second. */
        sec = (_SHORT16)Utl_Atoi(isec);
        if (sec < 0 || sec > 59)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strClock);
            return NULL;
        }

        /* If all formats are valid, store into data. Return OK. */
        strClock->year = (_USHORT16)year;
        strClock->month = (_UCHAR8)mon;
        strClock->day = (_UCHAR8)day;
        strClock->hour = (_UCHAR8)hour;
        strClock->minute = (_UCHAR8)min;
        strClock->second = (_UCHAR8)sec;

        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireClock", RIL_prOK);
        *result = RIL_prOK;
        *strNum = 1;
         *strSize = sizeof(RIL_SYSTIME);
         return (_VOID*)strClock;
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strClock);
        return NULL;
    }
    
} /* end WmPs0707_InquireClock */


/**
 * @brief InquireLock
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_InquireLock(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
      _SHORT16   iBegin, iEnd, i;
    _UINT32    iNum = 0;
    _SHORT16    iOffset = AK_STRING_BEG;
       _CHAR8    iStat[8];
    _CHAR8    iOper[8];
    _CHAR8    iClass[8];
    RIL_CLCK_LOCK *strLock = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strLock = (RIL_CLCK_LOCK *)WIRELESS_MALLOC(sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM);
    memset(strLock, 0,  sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM);
    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }
    
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireLock", RIL_prERROR);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "AT+CLCK", AK_STRING_BEG, iLength)) != AK_STRING_BEG)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireLock", RIL_prERROR);
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if Module returns ME related error. If it does, function returns error. */
    if ((Utl_StrFndL(strResult, "+CME ERROR: ", AK_STRING_BEG, iLength)) != -1)
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireLock", RIL_prERROR);
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if OK is the only returned parameter. */
    if ((Utl_StrFndL(strResult, "OK\r\n", AK_STRING_BEG, iLength)) == (AK_STRING_BEG + AK_CLCK_SKIPATCLCK_9))
    {  
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireLock", RIL_prOK);
        *result =  RIL_prOK;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CLCK:", AK_STRING_BEG, iLength);
    if (iBegin != -1)
    {
        /* Loop through whole string to get different classes of locks. */
        while ((Utl_StrFndL(strResult, "+CLCK:", iOffset, iLength)) != -1)
        {
            /* Skip to beginning of status parameter. */
            iOffset = Utl_StrFndL(strResult, "+CLCK:", iOffset, iLength);
            iBegin = iOffset + AK_CLCK_SKIPCLCK_7;
            iOffset = iBegin;

            iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
            if (iEnd - iBegin == 2)
            {
                Utl_StrMid(iOper, strResult, iBegin, (_SHORT16)(iBegin+1));
                if (Utl_StrCmp(iOper,"AO") == 0)
                    strLock[iNum].Fac = RIL_AO;
                else if (Utl_StrCmp(iOper, "OI") == 0)
                    strLock[iNum].Fac = RIL_OI;
                else if (Utl_StrCmp(iOper, "OX") == 0)
                    strLock[iNum].Fac = RIL_OX;
                else if (Utl_StrCmp(iOper, "AI") == 0)
                    strLock[iNum].Fac = RIL_AI;
                else if (Utl_StrCmp(iOper, "IR") == 0)
                    strLock[iNum].Fac = RIL_IR;
                else 
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                iBegin = iEnd + 1;
            }

            iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
            if (iEnd == -1)
            {
                *result =  RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strLock);
                return NULL;
            }

            if ((iEnd - iBegin) < 1)
            {
                *result =  RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strLock);
                return NULL;
            }

            if ((iEnd - iBegin) == 1)
            {
                /* Verify validity of status parameter, 0 or 1. Else return error. */
                Utl_StrMid(iStat, strResult, iBegin, iBegin);
                if ((iStat[0] != '0') && (iStat[0] != '1'))
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* If status parameter is 0, set lock off. Else set lock on. */
                if (iStat[0] == '0')
                    strLock[iNum].mode = RIL_CLCK_Unlock;
                else
                    strLock[iNum].mode = RIL_CLCK_Lock;
            }
            else
            {    /*Find comma at end of status parameter. */
                iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
                if (iEnd == -1)
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* Verify length of status parameter is 1 char. */
                if ((iEnd - iBegin) != 1)
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* Verify validity of status parameter, 0 or 1. Else return error. */
                Utl_StrMid(iStat, strResult, iBegin, iBegin);
                if ((iStat[0] != '0') && (iStat[0] != '1'))
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* If status parameter is 0, set lock off. Else set lock on. */
                if (iStat[0] == '0')
                    strLock[iNum].mode = RIL_CLCK_Unlock;

                else
                    strLock[iNum].mode = RIL_CLCK_Lock;        /* status*/
                /* Skip to beginning of class parameter.
                Find <CR> at end of class parameter. */
                /*iBegin = Utl_StrFnd(strResult, ATCMD_RETURN, ++iEnd);*/
                iBegin = Utl_StrFndL(strResult, AT_NEW_LINE, ++iEnd, iLength);    /*wavecom,merge*/
                if(iBegin == -1)
                {
                    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, iEnd, iLength);
                }

                if (iBegin == -1)
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /*Utl_StrMid(iClass, strResult, iEnd, --iBegin);*/
                Utl_StrMid(iClass, strResult, iEnd, (_SHORT16)(iBegin - 2));        /*wavecom,merge*/
                /* Verify class parameter are valid digits. */
                i = 0;
                while ((iClass[i] >= '0') && (iClass[i] <= '9'))
                    i++;
                /*
                if ((i-1) != (iEnd - iBegin))
                    return RIL_prGERROR;
                */

                /* Convert class data from string to integer. Store it and keep looping. */
                strLock[iNum].Class = (_UCHAR8)(_SHORT16)Utl_Atoi(iClass);        /* class */
            }

            /* Increment number of locks. */
            iNum++;
        }
        /* Store number of locks. Return OK. */
        *strNum = iNum;
        AK_FUNCTION_RET_INT("WaveC_WmPs0707_InquireLock", RIL_prOK);
        *result = RIL_prOK;
        *strSize = sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM;
        return (_VOID*)strLock;
    }
    else
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }
    

}  /* end WaveC_WmPs0707_InquireLock */

/**
 * @brief Inquire phonebook memory storage
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */ 
_VOID* WaveC_WmPs0707_InquirePbStorage(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16  iBegin, iEnd,i;
    _CHAR8   strtemp[8];
    RIL_SIMPB_MEMORY *strMemory = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strMemory = (RIL_SIMPB_MEMORY *)WIRELESS_MALLOC(sizeof(RIL_SIMPB_MEMORY));
    memset(strMemory, 0,  sizeof(RIL_SIMPB_MEMORY));
    
    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }

    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {   
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "AT+CPBS?", 0, iLength);
    if (iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    iBegin += 10; /*length of AT+CPBS?*/
    iBegin = Utl_StrFndL(strResult, "+CPBS:", iBegin, iLength);
    if (iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }

    iBegin += 6;    /*ngth of +CPBS:*/
    iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
    if(-1 == iEnd)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    iBegin = iEnd;
    iBegin++;
    iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
    if(-1 == iEnd)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
    for(i=0; i<2; i++)
    {
        strMemory->strType[i]  = (_UCHAR8)strtemp[i];
    }
    strMemory->strType[2] = '\0';
    iBegin = iEnd;
    iBegin++;
    iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
    if(-1 == iEnd)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    iBegin = iEnd;
    iBegin++;
    iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
    if(-1 == iEnd)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
    strMemory->UsedNnm = (_SHORT16)Utl_Atoi(strtemp);
    iBegin = iEnd;
    iBegin++;
    iEnd = Utl_StrFndL(strResult, "\r", iBegin, iLength);
    if(-1 == iEnd)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strMemory);
        return NULL;
    }
    Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
    strMemory->MaxNum = (_SHORT16)Utl_Atoi(strtemp);
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_SIMPB_MEMORY);
    return (_VOID*)strMemory;
}
/**
 * @brief Inquire SIM card address book storage value
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */ 
_VOID* WaveC_WmPs0707_InquirePbSpace(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
     _SHORT16  iBegin, iEnd;
    _CHAR8    strtemp[8];
    RIL_SIMPB_SPACE *strEntry = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strEntry = (RIL_SIMPB_SPACE *)WIRELESS_MALLOC(sizeof(RIL_SIMPB_SPACE));
    memset(strEntry, 0,  sizeof(RIL_SIMPB_SPACE));
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strEntry);
        return NULL;
    }

    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;            
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strEntry);
        return NULL;
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CPBW:", AK_STRING_BEG, iLength);
    if (iBegin != -1)
    {
        /* Move "cursors" to appropriate places.*/
        iBegin += AK_CPBW_SKIPCPBW;

        iBegin++;
        iEnd = Utl_StrFndL(strResult, "-", iBegin, iLength);
        Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
        strEntry->MinID = (_USHORT16)Utl_Atoi(strtemp);

        iBegin = iEnd+1;
        iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }

        /* Get range of index for SIM card entries.*/
        Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 2));
        strEntry->MaxID = (_USHORT16)Utl_Atoi(strtemp);
        if(strEntry->MaxID < strEntry->MinID)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }

        /* Move "cursors" to appropriate places.*/
        iBegin = iEnd + 1;
        iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }

        /* Get limit of name length. Change from ASCII code to integer.*/
        Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
        strEntry->TeleLength = (_UCHAR8)(_SHORT16)Utl_Atoi(strtemp);
        G_MaxTeleLen = strEntry->TeleLength > MAX_TELE_LEN ? MAX_TELE_LEN : strEntry->TeleLength;
/*new added begin*/
        iBegin = iEnd + 1;
        iEnd = Utl_StrFndL(strResult, "(", iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }
        iBegin = iEnd + 1;
        iEnd = Utl_StrFndL(strResult, ")", iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }
        iBegin = iEnd + 1;
        iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }
/*new added end*/

        /* Move "cursors" to appropriate places.*/
        iBegin = iEnd + 1;
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;            
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strEntry);
            return NULL;
        }

        /* Get limit of telephone length. Change from ASCII code to integer.*/
        Utl_StrMid(strtemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
        strEntry->NameLength = (_UCHAR8)(_SHORT16)Utl_Atoi(strtemp);
        G_MaxNameLen = strEntry->NameLength > MAX_NAME_LEN ? MAX_NAME_LEN : strEntry->NameLength;
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_SIMPB_SPACE);
        return (_VOID*)strEntry;
    }
    {
        *result = RIL_prGERROR;
        *strNum = 0;            
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strEntry);
        return NULL;
    }
    

} /* end WaveC_WmPs0707_InquirePbSpace */

/**
 * @brief Read phonebook Entry
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_ReadPbEntries(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16            iBegin, iEnd;
       _CHAR8            strTemp[256];
    _USHORT16            iNum = 0;
    _USHORT16           pbReadCnt = 0;
    RIL_PB_SIM_ENTRY *str_entry = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
 
      if (strResult == NULL)
    {  
         *result = RIL_prNULL;
        *strNum = 0;            
        *strSize = 0;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }

    /*check the number of the valid phone-book entries*/
    iBegin = Utl_StrFndL(strResult, "+CPBR:", 0, iLength);

    while(iBegin != -1)
    {
        pbReadCnt++;
        iBegin += AK_CPBR_CPBRLEN_7;
        iBegin = Utl_StrFndL(strResult, "+CPBR:", iBegin, iLength);
    }

    if(pbReadCnt > 0)
    {
        str_entry = (RIL_PB_SIM_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_PB_SIM_ENTRY)*pbReadCnt);

        iBegin = Utl_StrFndL(strResult, "+CPBR:", 0, iLength);
        while(iBegin != -1)
        {
            /*Get entry index*/
            iBegin += AK_CPBR_CPBRLEN_7;
            iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            str_entry[iNum].index = (_USHORT16)Utl_Atoi(strTemp);
            
            /*Get telephone number*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if(iEnd - iBegin > G_MaxTeleLen)
            {
                Utl_StrMidL(str_entry[iNum].Tele, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen - 1), iLength);
            }
            else
            {
                Utl_StrMidL(str_entry[iNum].Tele, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }
            
            /*Get number type*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
            
            /*Get name*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            /*abort the empty entry*/
            if (Utl_StrIsEmpty(strTemp) && Utl_StrIsEmpty(str_entry[iNum].Tele))
            {
                iBegin = Utl_StrFndL(strResult, "+CPBR:", iEnd, iLength);
                continue;
            }

            if(0 == Utl_StrCmp("00FF", strTemp))
            {
                str_entry[ iNum ].Name[0] = 0;
                str_entry[ iNum ].Name[1] = ' ';
                str_entry[ iNum ].Name[2] = 0;
                str_entry[ iNum ].Name[3] = 0;
            }
            else
            {
                /*
                ConvUCS2StrToUnicode(strTemp, strName);
                memcpy(str_entry[ iNum ].Name, strName, G_MaxNameLen*2);
                */
                memcpy(str_entry[iNum].Name, strTemp, G_MaxNameLen);
            }
            iBegin = Utl_StrFndL(strResult, "+CPBR:", iEnd, iLength);
            iNum++;
        }
    }
    *result = RIL_prOK;
    *strNum = iNum;
    *strSize = sizeof(RIL_PB_SIM_ENTRY)*pbReadCnt;
    return str_entry;
} /* end WaveC_WmPs0707_ReadPbEntries */

/**
 * @brief Read phonebook Entry
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* C202_WmPs0707_ReadPbEntries(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16            iBegin, iEnd;
       _CHAR8            strTemp[256];
    _USHORT16            iNum = 0;
    _USHORT16           pbReadCnt = 0;
    RIL_PB_SIM_ENTRY *str_entry = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
 
      if (strResult == NULL)
    {  
         *result = RIL_prNULL;
        *strNum = 0;            
        *strSize = 0;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }

    /*check the number of the valid phone-book entries*/
    iBegin = Utl_StrFndL(strResult, "+CPBR:", 0, iLength);

    while(iBegin != -1)
    {
        pbReadCnt++;
        iBegin += AK_CPBR_CPBRLEN_7;
        iBegin = Utl_StrFndL(strResult, "+CPBR:", iBegin, iLength);
    }

    if(pbReadCnt > 0)
    {
        str_entry = (RIL_PB_SIM_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_PB_SIM_ENTRY)*pbReadCnt);

        iBegin = Utl_StrFndL(strResult, "+CPBR:", 0, iLength);
        while(iBegin != -1)
        {
            /*Get entry index*/
            iBegin += AK_CPBR_CPBRLEN_6;
            iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            str_entry[iNum].index = (_USHORT16)Utl_Atoi(strTemp);
            
            /*Get telephone number*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if(iEnd - iBegin > G_MaxTeleLen)
            {
                Utl_StrMidL(str_entry[iNum].Tele, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen - 1), iLength);
            }
            else
            {
                Utl_StrMidL(str_entry[iNum].Tele, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }
                        
            /*Get name*/
            iBegin = iEnd + 3;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);

            #if 0 /* Don't need this */
            /*Get number type*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
            #endif
            
            /*abort the empty entry*/
            if (Utl_StrIsEmpty(strTemp) && Utl_StrIsEmpty(str_entry[iNum].Tele))
            {
                iBegin = Utl_StrFndL(strResult, "+CPBR:", iEnd, iLength);
                continue;
            }

            if(0 == Utl_StrCmp("00FF", strTemp))
            {
                str_entry[ iNum ].Name[0] = 0;
                str_entry[ iNum ].Name[1] = ' ';
                str_entry[ iNum ].Name[2] = 0;
                str_entry[ iNum ].Name[3] = 0;
            }
            else
            {
                /*
                ConvUCS2StrToUnicode(strTemp, strName);
                memcpy(str_entry[ iNum ].Name, strName, G_MaxNameLen*2);
                */
                memcpy(str_entry[iNum].Name, strTemp, G_MaxNameLen);
            }
            iBegin = Utl_StrFndL(strResult, "+CPBR:", iEnd, iLength);
            iNum++;
        }
    }
    *result = RIL_prOK;
    *strNum = iNum;
    *strSize = sizeof(RIL_PB_SIM_ENTRY)*pbReadCnt;
    return str_entry;
} /* end C202_WmPs0707_ReadPbEntries */

_VOID* MC323_WmPs0707_ReadPbEntries(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16            iBegin, iEnd;
       _CHAR8            strTemp[256];
    _USHORT16            iNum = 0;
    _USHORT16           pbReadCnt = 0;
    RIL_PB_SIM_ENTRY *str_entry = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
 
    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;            
        *strSize = 0;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *result = RIL_prOK;
        *strNum = 0;            
        *strSize = 0;
        return NULL;
    }
    
    /*check the number of the valid phone-book entries*/
    iBegin = Utl_StrFndL(strResult, "^CPBR:", 0, iLength);

    while(iBegin != -1)
    {
        pbReadCnt++;
        iBegin += AK_CPBR_CPBRLEN_7;
        iBegin = Utl_StrFndL(strResult, "^CPBR:", iBegin, iLength);
    }

    if(pbReadCnt > 0)
    {
        str_entry = (RIL_PB_SIM_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_PB_SIM_ENTRY)*pbReadCnt);

        iBegin = Utl_StrFndL(strResult, "^CPBR:", 0, iLength);
        while(iBegin != -1)
        {
            /*Get entry index*/
            iBegin += AK_CPBR_CPBRLEN_6;
            iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            str_entry[iNum].index = (_USHORT16)Utl_Atoi(strTemp);
            
            /*Get telephone number*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            if(iEnd - iBegin > G_MaxTeleLen)
            {
                Utl_StrMidL(str_entry[iNum].Tele, strResult, iBegin, (_SHORT16)(iBegin + G_MaxTeleLen - 1), iLength);
            }
            else
            {
                Utl_StrMidL(str_entry[iNum].Tele, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);
            }
                        
            /*Get name*/
            iBegin = iEnd + 7;
            iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd - 1), iLength);

            #if 0 /* Don't need this */
            /*Get number type*/
            iBegin = iEnd + 2;
            iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
            #endif
            
            /*abort the empty entry*/
            if (Utl_StrIsEmpty(strTemp) && Utl_StrIsEmpty(str_entry[iNum].Tele))
            {
                iBegin = Utl_StrFndL(strResult, "^CPBR:", iEnd, iLength);
                continue;
            }

            if(0 == Utl_StrCmp("00FF", strTemp))
            {
                str_entry[ iNum ].Name[0] = 0;
                str_entry[ iNum ].Name[1] = ' ';
                str_entry[ iNum ].Name[2] = 0;
                str_entry[ iNum ].Name[3] = 0;
            }
            else
            {
                /*
                ConvUCS2StrToUnicode(strTemp, strName);
                memcpy(str_entry[ iNum ].Name, strName, G_MaxNameLen*2);
                */
                memcpy(str_entry[iNum].Name, strTemp, G_MaxNameLen);
            }
            iBegin = Utl_StrFndL(strResult, "^CPBR:", iEnd, iLength);
            iNum++;
        }
    }
    *result = RIL_prOK;
    *strNum = iNum;
    *strSize = sizeof(RIL_PB_SIM_ENTRY)*pbReadCnt;
    return str_entry;
}

/**
 * @brief Read known network ID and name
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_ReadOperatorNames(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16           iBegin, iEnd, iOldBegin;
    _SHORT16           iPos1, iPos2;
    _USHORT16            iNum = 0;
    RIL_COPN_NAME* str_operator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    /*only parse front  300 cmd*/
    str_operator = (RIL_COPN_NAME *)WIRELESS_MALLOC(sizeof(RIL_COPN_NAME) * MAX_NET_NUM);
    /*clear first*/
    memset(str_operator, 0, sizeof(RIL_COPN_NAME) * MAX_NET_NUM);
    if (strResult == NULL)
    {  
         *result = RIL_prNULL;
        *strNum = 0;            
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str_operator);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str_operator);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "\r\nOK\r\n", (_SHORT16)(iLength - 10), iLength);
    if (iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;            
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str_operator);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+COPN:", 0, iLength);
    while (iBegin != -1)
    {
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);

        iOldBegin = iBegin;
        iBegin = Utl_StrFndL(strResult, "+COPN:", iEnd, iLength);

        /*parse COPN line*/
        iPos1 = iOldBegin + AK_COPS_COPSLEN_7;
        if(strResult[ iPos1 ] != '\"')
        {
            continue;
        }

        iPos2 = Utl_StrFndL(strResult, "\"", (_SHORT16)(iPos1 + 1), iLength);

        if((iPos2 - iPos1) - 1 > MAX_NETID_LEN)
        {
            continue;
        }
        Utl_StrMidL(str_operator[iNum].numericn, strResult, (_SHORT16)(iPos1 + 1), (_SHORT16)(iPos2 - 1), iLength);

        if (strResult[ iPos2 + 1 ] != ',' || strResult[ iPos2 + 2 ] != '\"')
        {
            continue;
        }

        iPos1 = iPos2 + 3;
        iPos2 = Utl_StrFndL(strResult, "\"", iPos1, iLength);
        if (iPos2 - iPos1 <= MAX_NETNAME_LEN)   /* changed by ZouMai */
        {
            Utl_StrMidL(str_operator[iNum].alphan, strResult, iPos1, (_SHORT16)(iPos2-1), iLength);
        }
        else
        {
            Utl_StrMidL(str_operator[iNum].alphan, strResult, iPos1, (_SHORT16)(iPos1 + MAX_NETNAME_LEN - 1), iLength);
        }

        iNum++;
        if (iNum >= MAX_NET_NUM)        /* only malloc MAX_NET_NUM * entry before call this function */
            break;
    }

    *strNum = iNum;
    *result = RIL_prOK;
    *strSize = sizeof(RIL_COPN_NAME) * MAX_NET_NUM;
    return (_VOID*)str_operator;
} /* end WaveC_WmPs0707_ReadOperatorNames */

/**
 * @brief read the current operator
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
_VOID* WaveC_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1, iPos2, iPos3;
    _CHAR8            tmode[8];
    _CHAR8            tformat[8];
    RIL_COPS_CURRENTNAME * strCurrentOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strCurrentOperator = (RIL_COPS_CURRENTNAME *)WIRELESS_MALLOC(sizeof(RIL_COPS_CURRENTNAME));
    WIRELESS_INFO("WaveC_WmPs0707_InquireCurOperator :%s\r\n", strResult);
    /*clear first*/
    memset(strCurrentOperator, 0, sizeof(RIL_COPS_CURRENTNAME));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    tmode[0] = 0;
    tformat[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

    //kinson 20160126 iBegin = Utl_StrFndL(strResult, "+COPS:", AK_COPS_ATCOPSLEN_7, iLength);
    iBegin = Utl_StrFndL(strResult, "+COPS:", 0, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin +AK_COPS_COPSLEN_7), iLength);
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + AK_COPS_COPSLEN_7));
    if(iPos1 > iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(iPos1 == -1)
    {/* if only returning mode value*/
        if((iEnd - (iBegin + AK_COPS_COPSLEN_7)) > 1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        Utl_StrMid(tmode, strResult, (_SHORT16)(iBegin + AK_COPS_COPSLEN_7), (_SHORT16)(iEnd - 1));
        if(tmode[0] > '4' || tmode[0] < '0')
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        strCurrentOperator->operat[0] = '\0';
        strCurrentOperator->format = 0;
        strCurrentOperator->mode = (RIL_NET_MODE)Utl_Atoi(tmode);
        *result =  RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COPS_CURRENTNAME);
        return (_VOID*)strCurrentOperator;    
    }
    Utl_StrMid(tmode, strResult, (_SHORT16)(iBegin + AK_COPS_COPSLEN_7), (_SHORT16)(iPos1 - 1));
    if(tmode[0] > '4' || tmode[0] < '0')
    {/* returning value mode is between 0 and 4*/
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    strCurrentOperator->mode = (RIL_NET_MODE)Utl_Atoi(tmode);

    iPos2 = iPos1 + 1 + 1;
    if(strResult[iPos2] !=',' && iPos2 != iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(iPos2 == iEnd)
    {
        Utl_StrMid(tformat, strResult, (_SHORT16)(iPos2 - 1), (_SHORT16)(iPos2 - 1));
        if(tformat[0] != '0' && tformat[0] != '2')
        {/* returning value format is 0 or 2*/
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        strCurrentOperator->format = (_SHORT16)Utl_Atoi(tformat);
        *result =  RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COPS_CURRENTNAME);
        return (_VOID*)strCurrentOperator;    
    }
    /* iPos2 is the position of the second comma to seperate format and operator*/
    Utl_StrMid(tformat, strResult, (_SHORT16)(iPos2 - 1), (_SHORT16)(iPos2 - 1));
    if(tformat[0] != '0' && tformat[0] !='2')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    /*strCurrentOperator->format = 2;   For SIM200, the format must be 2*/
    strCurrentOperator->format = (_SHORT16)Utl_Atoi(tformat);

    /* kinson
    if(strResult[iEnd-1] != '\"' || strResult[iPos2 + 1] != '\"')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    */
    /* copy operator to the structure, (_SHORT16)(iPos2 + 1) representing the position of '\"', (_SHORT16)(iEnd - 1)*/
    /*representing the position of the second '\"'*/
    iPos3 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos2 + 1));
    if(iPos3 > iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(strCurrentOperator->format == 2)
    {
        if ((iEnd - 1) - (iPos2 + 2) < MAX_NETNAME_LEN)
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iEnd - 2));    //weikk add 20160705
            //Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd -1));
        else
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos2 + 1 + MAX_NETNAME_LEN - 1));
    }
    else
    {
        if ((iEnd -2) - (iPos2+2) < MAX_NETNAME_LEN)
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iEnd -2));
        else
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iPos2 + 2 + MAX_NETNAME_LEN - 1));
    }

    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COPS_CURRENTNAME);

    return (_VOID*)strCurrentOperator;    
}
#else
 _VOID* WaveC_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1, iPos2, iPos3;
    _CHAR8            tmode[8];
    _CHAR8            tformat[8];
    RIL_COPS_CURRENTNAME * strCurrentOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strCurrentOperator = (RIL_COPS_CURRENTNAME *)WIRELESS_MALLOC(sizeof(RIL_COPS_CURRENTNAME));
    WIRELESS_INFO("WaveC_WmPs0707_InquireCurOperator :%s\r\n", strResult);
    /*clear first*/
    memset(strCurrentOperator, 0, sizeof(RIL_COPS_CURRENTNAME));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    tmode[0] = 0;
    tformat[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

    //kinson 20160126 iBegin = Utl_StrFndL(strResult, "+COPS:", AK_COPS_ATCOPSLEN_7, iLength);
    iBegin = Utl_StrFndL(strResult, "+COPS:", 0, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin +AK_COPS_COPSLEN_7), iLength);
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + AK_COPS_COPSLEN_7));
    if(iPos1 > iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(iPos1 == -1)
    {/* if only returning mode value*/
        if((iEnd - (iBegin + AK_COPS_COPSLEN_7)) > 1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        Utl_StrMid(tmode, strResult, (_SHORT16)(iBegin + AK_COPS_COPSLEN_7), (_SHORT16)(iEnd - 1));
        if(tmode[0] > '4' || tmode[0] < '0')
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        strCurrentOperator->operat[0] = '\0';
        strCurrentOperator->format = 0;
        strCurrentOperator->mode = (RIL_NET_MODE)Utl_Atoi(tmode);
        *result =  RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COPS_CURRENTNAME);
        return (_VOID*)strCurrentOperator;    
    }
    Utl_StrMid(tmode, strResult, (_SHORT16)(iBegin + AK_COPS_COPSLEN_7), (_SHORT16)(iPos1 - 1));
    if(tmode[0] > '4' || tmode[0] < '0')
    {/* returning value mode is between 0 and 4*/
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    strCurrentOperator->mode = (RIL_NET_MODE)Utl_Atoi(tmode);

    iPos2 = iPos1 + 1 + 1;
    if(strResult[iPos2] !=',' && iPos2 != iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(iPos2 == iEnd)
    {
        Utl_StrMid(tformat, strResult, (_SHORT16)(iPos2 - 1), (_SHORT16)(iPos2 - 1));
        if(tformat[0] != '0' && tformat[0] != '2')
        {/* returning value format is 0 or 2*/
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        strCurrentOperator->format = (_SHORT16)Utl_Atoi(tformat);
        *result =  RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COPS_CURRENTNAME);
        return (_VOID*)strCurrentOperator;    
    }
    /* iPos2 is the position of the second comma to seperate format and operator*/
    Utl_StrMid(tformat, strResult, (_SHORT16)(iPos2 - 1), (_SHORT16)(iPos2 - 1));
    if(tformat[0] != '0' && tformat[0] !='2')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    /*strCurrentOperator->format = 2;   For SIM200, the format must be 2*/
    strCurrentOperator->format = (_SHORT16)Utl_Atoi(tformat);

    /* kinson
    if(strResult[iEnd-1] != '\"' || strResult[iPos2 + 1] != '\"')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    */
    /* copy operator to the structure, (_SHORT16)(iPos2 + 1) representing the position of '\"', (_SHORT16)(iEnd - 1)*/
    /*representing the position of the second '\"'*/
    iPos3 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos2 + 1));
    if(iPos3 > iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(strCurrentOperator->format == 2)
    {
        if ((iEnd - 1) - (iPos2 + 1) < MAX_NETNAME_LEN)
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iPos3 - 2));    //weikk add 20160705
            //Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd -1));
        else
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos2 + 1 + MAX_NETNAME_LEN - 1));
    }
    else
    {
        if ((iEnd -2) - (iPos2+2) < MAX_NETNAME_LEN)
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iEnd -2));
        else
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iPos2 + 2 + MAX_NETNAME_LEN - 1));
    }

    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COPS_CURRENTNAME);

    return (_VOID*)strCurrentOperator;    
}
#endif

_VOID* UC15_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1, iPos2;
    _CHAR8            tmode[8];
    _CHAR8            tformat[8];
    RIL_COPS_CURRENTNAME * strCurrentOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strCurrentOperator = (RIL_COPS_CURRENTNAME *)WIRELESS_MALLOC(sizeof(RIL_COPS_CURRENTNAME));
    WIRELESS_INFO("UC15_WmPs0707_InquireCurOperator :%s\r\n", strResult);
    /*clear first*/
    memset(strCurrentOperator, 0, sizeof(RIL_COPS_CURRENTNAME));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    tmode[0] = 0;
    tformat[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+COPS:", AK_COPS_ATCOPSLEN_7, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin +AK_COPS_COPSLEN_7), iLength);
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + AK_COPS_COPSLEN_7));
    if(iPos1 > iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(iPos1 == -1)
    {/* if only returning mode value*/
        if((iEnd - (iBegin + AK_COPS_COPSLEN_7)) > 1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        Utl_StrMid(tmode, strResult, (_SHORT16)(iBegin + AK_COPS_COPSLEN_7), (_SHORT16)(iEnd - 1));
        if(tmode[0] > '4' || tmode[0] < '0')
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        strCurrentOperator->operat[0] = '\0';
        strCurrentOperator->format = 0;
        strCurrentOperator->mode = (RIL_NET_MODE)Utl_Atoi(tmode);
        *result =  RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COPS_CURRENTNAME);
        return (_VOID*)strCurrentOperator;    
    }
    Utl_StrMid(tmode, strResult, (_SHORT16)(iBegin + AK_COPS_COPSLEN_7), (_SHORT16)(iPos1 - 1));
    if(tmode[0] > '4' || tmode[0] < '0')
    {/* returning value mode is between 0 and 4*/
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    strCurrentOperator->mode = (RIL_NET_MODE)Utl_Atoi(tmode);

    iPos2 = iPos1 + 1 + 1;
    if(strResult[iPos2] !=',' && iPos2 != iEnd)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    if(iPos2 == iEnd)
    {
        Utl_StrMid(tformat, strResult, (_SHORT16)(iPos2 - 1), (_SHORT16)(iPos2 - 1));
        if(tformat[0] != '0' && tformat[0] != '2')
        {/* returning value format is 0 or 2*/
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCurrentOperator);
            return NULL;
        }
        strCurrentOperator->format = (_SHORT16)Utl_Atoi(tformat);
        *result =  RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COPS_CURRENTNAME);
        return (_VOID*)strCurrentOperator;    
    }
    /* iPos2 is the position of the second comma to seperate format and operator*/
    Utl_StrMid(tformat, strResult, (_SHORT16)(iPos2 - 1), (_SHORT16)(iPos2 - 1));
    if(tformat[0] != '0' && tformat[0] !='2')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    /*strCurrentOperator->format = 2;   For SIM200, the format must be 2*/
    strCurrentOperator->format = (_SHORT16)Utl_Atoi(tformat);

    /* kinson
    if(strResult[iEnd-1] != '\"' || strResult[iPos2 + 1] != '\"')
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    */
    /* copy operator to the structure, (_SHORT16)(iPos2 + 1) representing the position of '\"', (_SHORT16)(iEnd - 1)*/
    /*representing the position of the second '\"'*/
    if(strCurrentOperator->format == 2)
    {
        if ((iEnd - 1) - (iPos2 + 1) < MAX_NETNAME_LEN)
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd -3));
        else
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos2 + 1 + MAX_NETNAME_LEN - 1));
    }
    else
    {
        if ((iEnd -2) - (iPos2+2) < MAX_NETNAME_LEN)
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iEnd - 4));
        else
            Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iPos2 + 2 + MAX_NETNAME_LEN - 1));
    }
    WIRELESS_INFO("operat:%s\r\n", strCurrentOperator->operat);

    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COPS_CURRENTNAME);

    return (_VOID*)strCurrentOperator;    
}


_VOID* WaveC_WmPs0707_InquireBSInfo(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    _CHAR8        strTemp[256];
    _UINT32      uiTemp = 0;
    _UINT32        uiCurBaseId = 0;
    _INT32        iCurBasedBm = 0;
    _UINT32     NegborBaseId[6] = {0};
    _INT32         NegborBaseddBm[6] = {0};
    _UINT32     uiBaseIndex = 0;
    _UINT32     i = 0;
    
    RIL_BS_INFO * strBSInfo = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strBSInfo = (RIL_BS_INFO *)WIRELESS_MALLOC(sizeof(RIL_BS_INFO));
    
    /*clear first*/
    memset(strBSInfo, 0, sizeof(RIL_BS_INFO));
    memset(strTemp, 0, sizeof(strTemp));

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }

    
    iBegin = Utl_StrFndL(strResult, "+QENG:", AK_COPS_ATCOPSLEN_7, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin +AK_COPS_COPSLEN_7), iLength);
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }    
    iBegin = Utl_StrFndL(strResult, "+QENG:", (_SHORT16)(iEnd+1), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }    

    for(i = 0; i < 5; i++)
    {
        iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iBegin == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
    }
    
    iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));    
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }    
    
    Utl_StrMid(strTemp, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));
    sscanf(strTemp, "%d", &uiCurBaseId);

    /* Current Basestation dBm*/
    for(i = 0; i < 2; i++)
    {
        iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iBegin == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
    }
    iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }    
    
    Utl_StrMid(strTemp, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));
    sscanf(strTemp, "%d", &iCurBasedBm);
    

    iBegin = Utl_StrFndL(strResult, "+QENG:", (_SHORT16)(iEnd+1), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
        WIRELESS_FREE(strBSInfo);
        return NULL;
    }        

    uiBaseIndex = 0;
    do
    {    
        iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iBegin == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
        iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iEnd == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
        Utl_StrMid(strTemp, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));
        sscanf(strTemp, "%d", &uiTemp);
        if(uiTemp != (uiBaseIndex + 1))
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;        
        }
        
        
        iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iBegin == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }            
        iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iEnd == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
        Utl_StrMid(strTemp, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));
        sscanf(strTemp, "%d", &NegborBaseId[uiBaseIndex]);
        if(NegborBaseId[uiBaseIndex] > 1024) /* if use 1900 band, must decrease 1024    */
        {
            NegborBaseId[uiBaseIndex] = NegborBaseId[uiBaseIndex] - 1024;
        }

        iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iBegin == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
        iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
        if(iEnd == -1)
        {
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
            WIRELESS_FREE(strBSInfo);
            return NULL;
        }    
        Utl_StrMid(strTemp, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));
        sscanf(strTemp, "%d", &NegborBaseddBm[uiBaseIndex]);

        if(uiBaseIndex < 5) /* if it is the last node, don't need */
        {
            for(i = 0; i < 7; i++)
            {
                iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
                if(iBegin == -1)
                {
                    *result =  RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    printf("(%s)(%d)strResult:%s\r\n", __FILE__, __LINE__, strResult);
                    WIRELESS_FREE(strBSInfo);
                    return NULL;
                }    
            }
        }
        
        uiBaseIndex++;
    }while(uiBaseIndex < 6);
    
    if(NegborBaseId[0] != 0)
    {
        strBSInfo->bGetNegborBSInfo = EOS_TRUE;
    }
    else
    {
        strBSInfo->bGetNegborBSInfo = EOS_FALSE;
    }

    strBSInfo->CurrBSId = uiCurBaseId;
    sprintf(strBSInfo->BSInfo, "%d;%d,%d;%d,%d;%d,%d;%d,%d;%d,%d;%d,%d;%d", uiCurBaseId, iCurBasedBm, 
                      NegborBaseId[0], NegborBaseddBm[0], NegborBaseId[1], NegborBaseddBm[1],
                      NegborBaseId[2], NegborBaseddBm[2], NegborBaseId[3], NegborBaseddBm[3],
                      NegborBaseId[4], NegborBaseddBm[4], NegborBaseId[5], NegborBaseddBm[5]);

    WIRELESS_INFO("BsInfo:%s\r\n", strBSInfo->BSInfo);
    
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_BS_INFO);
    return (_VOID*)strBSInfo;
}

/**
 * @brief read the current operator
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* CM300_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    _CHAR8            tmode[2];
    _CHAR8            tformat[2];
    RIL_COPS_CURRENTNAME * strCurrentOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strCurrentOperator = (RIL_COPS_CURRENTNAME *)WIRELESS_MALLOC(sizeof(RIL_COPS_CURRENTNAME));
    /*clear first*/
    memset(strCurrentOperator, 0, sizeof(RIL_COPS_CURRENTNAME));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    tmode[0] = 0;
    tformat[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

#if 0    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
#endif

    iBegin = Utl_StrFndL(strResult, "+CSS:", AK_COPS_ATCOPSLEN_7, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 5));

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 5), iLength);
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
 
    strCurrentOperator->format = 2;  
    Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iBegin + 2), (_SHORT16)(iEnd - 1));

    WIRELESS_INFO("strCurrentOperator->operat = %s\r\n", strCurrentOperator->operat);
     

    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COPS_CURRENTNAME);

    return (_VOID*)strCurrentOperator;    
}

_VOID* C202_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    _CHAR8            tmode[2];
    _CHAR8            tformat[2];
    RIL_COPS_CURRENTNAME * strCurrentOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _CHAR8            strTemp[64];

    strCurrentOperator = (RIL_COPS_CURRENTNAME *)WIRELESS_MALLOC(sizeof(RIL_COPS_CURRENTNAME));
    
    /*clear first*/
    memset(strCurrentOperator, 0, sizeof(RIL_COPS_CURRENTNAME));
    memset(strTemp, 0, sizeof(strTemp));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    tmode[0] = 0;
    tformat[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

#if 0    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
#endif

    iBegin = Utl_StrFndL(strResult, "+VMCCMNC:", AK_COPS_ATCOPSLEN_7, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iBegin = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 5));
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

    iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

    Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));

    iBegin = iEnd;
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    Utl_StrMid(strTemp, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));
    
    strCurrentOperator->format = 2;
    strcat(strCurrentOperator->operat, strTemp);

    WIRELESS_INFO("strCurrentOperator->operat = %s\r\n", strCurrentOperator->operat);
     
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COPS_CURRENTNAME);

    return (_VOID*)strCurrentOperator;    
}

_VOID* MC323_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    _CHAR8            tmode[2];
    _CHAR8            tformat[2];
    RIL_COPS_CURRENTNAME * strCurrentOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _CHAR8            strTemp[64];

    strCurrentOperator = (RIL_COPS_CURRENTNAME *)WIRELESS_MALLOC(sizeof(RIL_COPS_CURRENTNAME));

    /*clear first*/
    memset(strCurrentOperator, 0, sizeof(RIL_COPS_CURRENTNAME));
    memset(strTemp, 0, sizeof(strTemp));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    tmode[0] = 0;
    tformat[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }

#if 0    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
#endif

    iBegin = Utl_StrFndL(strResult, "AT+CIMI", 0, iLength);

    if(iBegin == -1)
    {
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCurrentOperator);
        return NULL;
    }
    
    iBegin = iBegin + 9;
    iEnd = iBegin + 6;
    
    Utl_StrMid(strCurrentOperator->operat, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 1));

    strCurrentOperator->format = 2;

    WIRELESS_INFO("strCurrentOperator->operat = %s\r\n", strCurrentOperator->operat);
     
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COPS_CURRENTNAME);

    return (_VOID*)strCurrentOperator;    
}



/**
 * @brief InquireOperator
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */

_VOID* WaveC_WmPs0707_InquireAvaiOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1, iPos2;
    _USHORT16 iNum = 0;
    RIL_COPS_NETWORK *strOperator = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strOperator = (RIL_COPS_NETWORK *)WIRELESS_MALLOC(sizeof(RIL_COPS_NETWORK) * MAX_NETWORK_NUM);
    /*clear first*/
    memset(strOperator, 0, sizeof(RIL_COPS_NETWORK) * MAX_NETWORK_NUM);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strOperator);
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strOperator);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strOperator);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+COPS=?", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strOperator);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+COPS:", AK_COPS_ATCOPSLEN_9, iLength);
    if(iBegin != -1)
    {
        iBegin = iBegin + AK_COPS_COPSLEN_7;
        iEnd = Utl_StrFndL(strResult, "\")", (_SHORT16)(iBegin + 1), iLength);
        while(iEnd != -1)
        {
            if(strResult[iBegin] != '(')
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            iPos1 = Utl_StrFndChr(strResult, '(', (_SHORT16)(iBegin + 1));
            if(iPos1 < iEnd && iPos1 != -1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            iPos1 = iBegin + 1;

            if(strResult[iPos1 + 1] != ',')
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            if(strResult[iPos1] > '3' || strResult[iPos1] < '0')
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            strOperator[iNum].stat = (RIL_NETWORK_STAT)(strResult[iPos1]- '0');
            /* parse AT+COPS=?,AT has changed*/
            iPos2= Utl_StrFndL(strResult, ",\"", (_SHORT16)(iPos1 + 1), iLength);
            if(iPos2 != iPos1 + 1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            iPos1 = Utl_StrFndL(strResult, "\",", (_SHORT16)(iPos2 + 1), iLength);
            if(iPos1 > iEnd || iPos1 == -1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            iPos2 = Utl_StrFndL(strResult, ",\"", (_SHORT16)(iPos1 + 1), iLength);
            if(iPos2 > iEnd || iPos2 == -1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            iPos1 = Utl_StrFndL(strResult, "\",", (_SHORT16)(iPos2 + 1), iLength);
            if(iPos1 > iEnd || iPos1 == -1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }

            iPos2= Utl_StrFndL(strResult, ",\"", (_SHORT16)(iPos1 + 1), iLength);
            if(iPos2 != iPos1 + 1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }

            iPos1 = Utl_StrFndL(strResult, "\")", (_SHORT16)(iPos2 + 1), iLength);
            if(iPos1 > iEnd || iPos1 == -1)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }

            Utl_StrMid(strOperator[iNum].operatn, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iPos1 - 1));
    
            if(Utl_StrDigital(strOperator[iNum].operatn) == EOS_FALSE)
            {
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strOperator);
                return NULL;
            }
            iNum ++;

            iBegin = iEnd + strlen("\"),");

            iEnd = Utl_StrFndL(strResult, "\")", (_SHORT16)(iBegin + 1), iLength);
        }

        *strNum= iNum;
        *result =  RIL_prOK;
        *strSize = sizeof(RIL_COPS_NETWORK) * MAX_NETWORK_NUM;
        return strOperator;
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strOperator);
        return NULL;
    }

} /* end WaveC_WmPs0707_InquireAvaiOperator */

/**
 * @brief InquireSubscriber
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data 
 * @retval
 */
_VOID* WaveC_WmPs0707_InquireSubscriber(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin = 0;
    _CHAR8* strImsi = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;
    strImsi = (_CHAR8 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(strImsi, 0, iLength);

    if (strResult == NULL)
    {
       *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strImsi);
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(strImsi);
        return NULL;
    }

    /* Check to see if Module returns OK at end of string. If it does not, function returns error.*/
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strImsi);
        return NULL;
    }

    /*changed for WaveC*/
    iBegin = Utl_StrFndL(strResult, "AT+CIMI\r\r\n", AK_STRING_BEG, iLength);
    if (iBegin != -1)
    {
        iBegin += 10;
        Utl_StrMid_N(strImsi, iLength, strResult, iBegin, (_SHORT16)(iBegin + 14));

        *result = RIL_prOK;
        *strNum = 1;
        *strSize = iLength;
        return (_VOID*)strImsi;
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strImsi);
        return NULL;
    
    }
    
} /* end WaveC_WmPs0707_InquireSubscriber*/


/**
 * @brief: Inquire extended ERROR report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0707_GetExtendedError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16  iBegin, iEnd;
    _CHAR8    iTemp[16];
    RIL_ERR_REPORT    tempErrorRp;
    RIL_ERR_REPORT *errorRp = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    errorRp = (RIL_ERR_REPORT *)WIRELESS_MALLOC(sizeof(RIL_ERR_REPORT));
    /*clear first*/
    memset(errorRp, 0, sizeof(RIL_ERR_REPORT));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CEER:", 0, iLength);    /* no +CMEE: */
    if (iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    if (Utl_StrFndL(strResult, "OK\r\n", iBegin, iLength) == -1)            /* no OK */
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    iBegin = iBegin + AK_CEER_LEN_7;
    iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1));
    if (iEnd == -1 || iEnd - iBegin > 5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    Utl_StrMid(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
    if (Utl_StrDigital(iTemp) == EOS_FALSE)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    tempErrorRp.repID = (_SHORT16)Utl_Atoi(iTemp);

    if (tempErrorRp.repID == 0)        /* unuseful location ID */
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
//        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }
    /* got the first number: location ID */

    iBegin = iEnd + 1;

    iEnd = Utl_StrFndL(strResult, "OK", 0, iLength);
    /* iEnd = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + 1)); */
    if (iEnd == -1 || (iEnd - iBegin) > 5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    Utl_StrMid(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 3));

    if (Utl_StrDigital(iTemp) == EOS_FALSE)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    tempErrorRp.reason = (_SHORT16)Utl_Atoi(iTemp);
    /* got the second number: reason */
#if 0
    iBegin = iEnd + 1;
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if (iEnd == -1 || iEnd - iBegin > 5)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    Utl_StrMid(iTemp, strResult, iBegin, (_SHORT16)(iEnd - 1));
    if (Utl_StrDigital(iTemp) == EOS_FALSE)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    if (tempErrorRp.reason == 0)
    {
        tempErrorRp.reason = (_SHORT16)Utl_Atoi(iTemp);
        tempErrorRp.repType = eRIL_EXT_SS_ERROR;
    }
    else
    {
        tempErrorRp.repType = eRIL_EXT_ERROR;
    }
    /* got the third number: ss_release */
#endif
    tempErrorRp.repType = 0;

    if (tempErrorRp.reason == 0)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(errorRp);
        return NULL;
    }

    *errorRp = tempErrorRp;
    *result =  RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_ERR_REPORT);

    return (_VOID*)errorRp;
}


/**
 * @brief: Parse signal quality
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 **/
_VOID* WaveC_WmPs0707_InquireSignalQuality(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[16];
    _INT32   *signal = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    WIRELESS_INFO("WaveC_WmPs0707_InquireSignalQuality:%s\r\n", strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+CSQ:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + 6;
    end   = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    signal = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));

#if 0
    value = Utl_Atoi(strTemp);
    if (value == 99)
        *signal = 0;
    else if (value <= tabRxLevToSignalDsp[1][0])
        *signal = 0;
    else if (value <= tabRxLevToSignalDsp[1][1])
        *signal = 1;
    else if (value <= tabRxLevToSignalDsp[1][2])
        *signal = 2;
    else if (value <= tabRxLevToSignalDsp[1][3])    
        *signal = 3;
    else
        *signal = 4;    
#else
    *signal = Utl_Atoi(strTemp);
#endif

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return signal;
}
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
//weikk 2016.11.28 AT QNWINFO
_VOID* WaveC_WmPs0707_InquireNetType(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[32];
    _CHAR8   *netType = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    //+QNWINFO: "FDD LTE","46011","LTE BAND 1",75     //get"FDD LTE"
    start = Utl_StrFndL(strResult, "+QNWINFO: \"", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + strlen("+QNWINFO: \"");
    end = Utl_StrFndL(strResult, "\"", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));

    netType = (char*)WIRELESS_MALLOC(sizeof(strTemp));

    strcpy(netType, strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(netType);
    WIRELESS_INFO("test netType:%s ; Len:%d.\n", netType, *strSize);
    return netType;
}

#else
//weikk 2016.3.31 AT xreg
_VOID* WaveC_WmPs0707_InquireNetType(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[32];
    _CHAR8   *netType = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    //+XREG: 0,1,7,BAND_LTE_3,"FFFF","06161003",get the '7'
    start = Utl_StrFndL(strResult, "+XREG:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + 6;
    end = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    start = end + 1;
    end = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = end + 1;
    end = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    netType = (char *)WIRELESS_MALLOC(32);
    strcpy(netType, strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(netType);
    return netType;
}
#endif

//weikk 20160713 AT ADC
_VOID* WaveC_WmPs0707_InquireADC(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end, i;
      _CHAR8    strTemp[16];
    _INT32   *mmadVal = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    //+MMAD: 1,111
    start = Utl_StrFndL(strResult, "+MMAD:", 0, iLength);
    if(start == -1)
        start = Utl_StrFndL(strResult, "+QADC:", 0, iLength);
    if(start == -1)
        start = Utl_StrFndL(strResult, "+CADC:", 0, iLength);
    
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + 6;
    end = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    start = end + 1;

    i = 0;
    while(i < 4)
    {
        if((*(strResult + start + i) >= '0') && (*(strResult + start + i) <= '9'))
        {
            *(strTemp + i) = *(strResult + start + i);
        }
        else
        {
            *(strTemp + i) = 0;
            break;
        }
        i++;
    }
    
    mmadVal = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));
    *mmadVal = Utl_Atoi(strTemp);
    //printf("test mmad:%s\n", strTemp);
    //printf("test mmad1:%d\n", *mmadVal);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return mmadVal;
}

_VOID* Quectel_WmPs0707_InquireCallStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start;
      _CHAR8        strTemp[16];
    _INT32*        pCallStatus = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        //WIRELESS_TRACE;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+CPAS:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    start = start + 7;

    Utl_StrMid(strTemp, strResult, start, start);
    pCallStatus = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));

    *pCallStatus = Utl_Atoi(strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return pCallStatus;
}




/**
 * @brief: Parse signal quality
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 **/
_VOID* MC323_WmPs0707_InquireSignalQuality(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[16];
    _INT32   *signal = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+CSQ:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = start + 5;
    end   = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    signal = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));

#if 0
    value = Utl_Atoi(strTemp);
    if (value == 99)
        *signal = 0;
    else if (value <= tabRxLevToSignalDsp[1][0])
        *signal = 0;
    else if (value <= tabRxLevToSignalDsp[1][1])
        *signal = 1;
    else if (value <= tabRxLevToSignalDsp[1][2])
        *signal = 2;
    else if (value <= tabRxLevToSignalDsp[1][3])    
        *signal = 3;
    else
        *signal = 4;    
#else
    *signal = Utl_Atoi(strTemp);
#endif

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return signal;
}


/**
 * @brief Inquire call meter
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0707_GetCurrentAcm(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize)
{
    _SHORT16    start, end;
    _CHAR8 *strACM = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;
    strACM = (_CHAR8 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(strACM, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strACM);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strACM);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+CACM:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strACM);
        return NULL;
    }

    start = start + 6;
    /*end   = Utl_StrFnd(strResult, ATCMD_RETURN, start);*/
    end   = Utl_StrFndL(strResult, "\"", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strACM);
        return NULL;
    }
    start = end;
    start++;
    end   = Utl_StrFndL(strResult, "\"", start, iLength);
    Utl_StrMid_N(strACM, iLength, strResult, start, (_SHORT16)(end - 1));

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)strACM;

}
/**
 * @brief Inquire call max meter
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0707_GetACMMax(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8 *strACMMax = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;
    strACMMax = (_CHAR8 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(strACMMax, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strACMMax);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strACMMax);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+CAMM:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strACMMax);
        return NULL;
    }

    start = start + 6;
    /*end   = Utl_StrFnd(strResult, ATCMD_RETURN, start);*/
    end   = Utl_StrFndL(strResult, "\"", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strACMMax);
        return NULL;
    }
    start = end;
    start++;
    end   = Utl_StrFndL(strResult, "\"", start, iLength);
    Utl_StrMid_N(strACMMax, iLength, strResult, start, (_SHORT16)(end - 1));

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)strACMMax;

}
/**
 * @brief Get the current call meter value
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0707_GetCurCallMeter(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8 *curCallMeter = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    curCallMeter = (_CHAR8 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(curCallMeter, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(curCallMeter);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(curCallMeter);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+CAOC:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(curCallMeter);
        return NULL;
    }

    start = start + 6;
    end   = Utl_StrFndL(strResult, ATCMD_RETURN, start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(curCallMeter);
        return NULL;
    }
    Utl_StrMid_N(curCallMeter, iLength, strResult, start, (_SHORT16)(end - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)curCallMeter;

}
/**
 * @brief Read SMS storage
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0705_InquireSmsStorage(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1, iPos2;
    RIL_CPMS    Storage;
    _USHORT16    iNum = 0;
    _CHAR8    number[16];
    RIL_CPMS *strStorage = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    printf("strResult:%s\r\n", strResult);
    strStorage = (RIL_CPMS *)WIRELESS_MALLOC(sizeof(RIL_CPMS) * MAX_SMSSTORAGE_LEN);
    /*clear first*/
    memset(strStorage, 0, sizeof(RIL_CPMS) * MAX_SMSSTORAGE_LEN);

    if (strResult == NULL)
    {
        *result =  RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strStorage);
        return NULL;
    }

    number[0] = 0;
    Storage.usedx = 0;
    Storage.totalx = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strStorage);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *strStorage = Storage;
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strStorage);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+CPMS", 0, iLength);
    if(iBegin == -1)
    {
        *strStorage = Storage;
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strStorage);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CPMS:", (_SHORT16)(iBegin + AK_CPMS_ATLEN_7), iLength);
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);

    if(iBegin == -1)
    {
        *strStorage = Storage;
        *result =  RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strStorage);
        return NULL;
    }
    iBegin = iBegin + AK_CPMS_CPMSLEN_7 ;    /* Changed for GPHONE*/
    for (;;)
    {

        iPos1 = Utl_StrFndChr(strResult, ',', iBegin);
        if(iPos1 == -1)
        {
            *strStorage = Storage;
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strStorage);
            return NULL;
        }
        Utl_StrMid(number, strResult, iBegin, (_SHORT16)(iPos1 - 1));
        if(Utl_StrDigital(number) == EOS_FALSE)
        {

            *strStorage = Storage;
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strStorage);
            return NULL;
        }
        strStorage[iNum].usedx = (_SHORT16)Utl_Atoi(number);

        iPos1 = iPos1 + 1;
        iPos2 = Utl_StrFndChr(strResult, ',', iPos1);
        if(iPos2 == -1)
        {
            Utl_StrMid(number, strResult, iPos1, (_SHORT16)(iEnd - 1));
            if(Utl_StrDigital(number) == EOS_FALSE)
            {
                *strStorage = Storage;
                *result =  RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_FREE(strStorage);
                return NULL;
            }
            strStorage[iNum].totalx = (_SHORT16)Utl_Atoi(number);
            iNum++;
            break;
        }
        if(iPos2 > iEnd - 1)
        {
            *strStorage = Storage;
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strStorage);
            return NULL;
        }
        Utl_StrMid(number, strResult, iPos1, (_SHORT16)(iPos2 - 1));
        if(Utl_StrDigital(number) == EOS_FALSE)
        {
            *strStorage = Storage;
            *result =  RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strStorage);
            return NULL;
        }
        strStorage[iNum].totalx = (_SHORT16)Utl_Atoi(number);
        iNum ++;
        iBegin = iPos2 + 1;
        break;
    }
    *strNum = iNum;
    printf("CPMS,USED:%d, Total:%d\r\n", Storage.usedx, Storage.totalx);
    *result = RIL_prOK;
    *strSize = sizeof(RIL_CPMS) * MAX_SMSSTORAGE_LEN ;
    return (_VOID*)strStorage;
} /*WaveC_WmPs0705_InquireSmsStorage*/


/**
 * @brief Parse SMS content.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0705_ReadSMS(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        iBegin, iEnd, iPos1, iPos2;
    _CHAR8        iTemp[10];
    RIL_SMS_ENTRY *strSMS = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strSMS = (RIL_SMS_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_SMS_ENTRY));

    WIRELESS_INFO("WaveC_WmPs0705_ReadSMS:%s\r\n", strResult);
    /*clear first*/
    memset(strSMS, 0, sizeof(RIL_SMS_ENTRY));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "AT^SMGR", 0);*/
    iBegin = Utl_StrFndL(strResult, "AT+CMGR", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "^SMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7));*/
    iBegin = Utl_StrFndL(strResult, "+CMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7), iLength);

    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    /*parsing the indicators of this message firstly, finding the first ATCMD_RETURN for this message*/
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + AK_CMGR_CMGRLEN_7), iLength);

    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

#if 0
    iPos1 = iBegin + AK_CMGR_CMGRLEN_7;

    iPos2 = iPos1 + 1;
    if(strResult[iPos2] !=',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    /*Utl_StrMid(iTemp, strResult, iPos1, iPos1);*/
    if(strResult[iPos1] > '3' || strResult[iPos1] < '0')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    strSMS->stat = (RIL_SMS_MODE)(strResult[iPos1] - '0');
    /* Getting the stat of the message*/

    if(strResult[iPos2+1] !='\"')
    {
        strSMS->name[0] =0;
        if(strResult[iPos2+1] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        iPos1 = iPos2 + 1 + 1;
        Utl_StrMid(iTemp, strResult, iPos1, (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
        /*Getting the length of the message and the name of the message default*/

    }
    else
    {
        iPos2 = iPos2 + 1;
        /*the position of the first '\"'*/
        iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iPos2 + 1));
        if(iPos1 > iEnd || iPos1 == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }

        Utl_StrMid(strSMS->name, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos1 - 1));

    
        if(strResult[ iPos1+1 ] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        iPos2 = iPos1 + 1;
        /* the next comma positon*/
        Utl_StrMid(iTemp, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
    }
#endif

    /* finding the the end of the message, also the end of the PDU packet*/
    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iEnd + 1), iLength);

    if(iBegin == iEnd + 1 + 1)
    {

        strcpy(strSMS->strPDU, "");
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    Utl_StrMid(strSMS->strPDU, strResult, (_SHORT16)(iEnd + 1 + 1), (_SHORT16)(iBegin -1));
    strSMS->idx = 0;        /* only one package, and index is unknow here */
    /*Getting pDU packet*/
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_SMS_ENTRY) ;
    WIRELESS_INFO("Read PDU:%s\r\n", strSMS->strPDU);
    
    return (_VOID*)strSMS;
} /* end WaveC_WmPs0705_ReadSMS */

_VOID* M35_WmPs0705_ReadSMS(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        iBegin = 0, iEnd = 0, iPos1 = 0, iPos2 = 0;
    _CHAR8        iTemp[10];
    RIL_SMS_ENTRY *strSMS = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strSMS = (RIL_SMS_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_SMS_ENTRY));

    WIRELESS_INFO("M35_WmPs0705_ReadSMS:%s\r\n", strResult);
    /*clear first*/
    memset(strSMS, 0, sizeof(RIL_SMS_ENTRY));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "AT^SMGR", 0);*/
    iBegin = Utl_StrFndL(strResult, "AT+CMGR", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "^SMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7));*/
    iBegin = Utl_StrFndL(strResult, "+CMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    /*parsing the indicators of this message firstly, finding the first ATCMD_RETURN for this message*/
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + AK_CMGR_CMGRLEN_7), iLength);
    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }    

    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iBegin + AK_CMGR_CMGRLEN_7));
    if(iPos1 > iEnd || iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    iPos2 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 2));
    if(iPos2 > iEnd || iPos2 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }    
    
    Utl_StrMid(strSMS->TPA, strResult, (_SHORT16)(iPos1 + 2), (_SHORT16)(iPos2 - 2));

    /*    SCTS    */
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos2 + 1));
    if(iPos1 > iEnd || iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
        
    iPos2 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 2));
    if(iPos2 > iEnd || iPos2 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }    
    
    Utl_StrMid(strSMS->TP_SCTS, strResult, (_SHORT16)(iPos1 + 2), (_SHORT16)(iPos2 - 2));
    
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos2 + 1));
    if(iPos1 > iEnd || iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 1));
    if(iPos1 > iEnd || iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 1));
    if(iPos1 > iEnd || iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    strSMS->TP_DCS = (RIL_SMS_MODE)(strResult[iPos1+1] - '0');
    
    /*     SCA    */
    iPos1 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 1));
    if(iPos1 > iEnd || iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    iPos2 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 2));
    if(iPos2 > iEnd || iPos2 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }    
    Utl_StrMid(strSMS->SCA, strResult, (_SHORT16)(iPos1 + 2), (_SHORT16)(iPos2 - 2));

#if 0
    iPos1 = iBegin + AK_CMGR_CMGRLEN_7;

    iPos2 = iPos1 + 1;
    if(strResult[iPos2] !=',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    /*Utl_StrMid(iTemp, strResult, iPos1, iPos1);*/
    if(strResult[iPos1] > '3' || strResult[iPos1] < '0')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    strSMS->stat = (RIL_SMS_MODE)(strResult[iPos1] - '0');
    /* Getting the stat of the message*/

    if(strResult[iPos2+1] !='\"')
    {
        strSMS->name[0] =0;
        if(strResult[iPos2+1] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        iPos1 = iPos2 + 1 + 1;
        Utl_StrMid(iTemp, strResult, iPos1, (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
        /*Getting the length of the message and the name of the message default*/

    }
    else
    {
        iPos2 = iPos2 + 1;
        /*the position of the first '\"'*/
        iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iPos2 + 1));
        if(iPos1 > iEnd || iPos1 == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }

        Utl_StrMid(strSMS->name, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos1 - 1));

    
        if(strResult[ iPos1+1 ] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        iPos2 = iPos1 + 1;
        /* the next comma positon*/
        Utl_StrMid(iTemp, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
    }
#endif

    /* finding the the end of the message, also the end of the PDU packet*/
    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iEnd + 1), iLength);

    if(iBegin == iEnd + 1 + 1)
    {

        strcpy(strSMS->strPDU, "");
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    Utl_StrMid(strSMS->strPDU, strResult, (_SHORT16)(iEnd + 1 + 1), (_SHORT16)(iBegin -1));
    strSMS->idx = 0;        /* only one package, and index is unknow here */
    /*Getting pDU packet*/
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_SMS_ENTRY) ;
    WIRELESS_INFO("Read PDU:%s\r\n", strSMS->strPDU);
    
    return (_VOID*)strSMS;
}


_VOID* MC323_WmPs0705_ReadSMS(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        iBegin, iEnd, iPos1, iPos2;
    _CHAR8        iTemp[10];
    RIL_SMS_ENTRY *strSMS = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strSMS = (RIL_SMS_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_SMS_ENTRY));
    _BOOL  bFindOK = EOS_TRUE;

    WIRELESS_INFO("MC323_WmPs0705_ReadSMS:%s\r\n", strResult);
    if(0)
    {
        _UINT32 i = 0;
        for(i = 0; i < strlen(strResult); i++)
        {
            WIRELESS_INFO("Data[%d] = 0x%x\r\n", i, strResult[i]);
        }
    }
    
    /*clear first*/
    memset(strSMS, 0, sizeof(RIL_SMS_ENTRY));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        bFindOK = EOS_FALSE;
    }
    /*iBegin = Utl_StrFnd(strResult, "AT^SMGR", 0);*/
    iBegin = Utl_StrFndL(strResult, "AT^HCMGR", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "^SMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7));*/
    iBegin = Utl_StrFndL(strResult, "^HCMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7), iLength);

    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    /*parsing the indicators of this message firstly, finding the first ATCMD_RETURN for this message*/
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + AK_CMGR_CMGRLEN_7), iLength);

    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

#if 0
    iPos1 = iBegin + AK_CMGR_CMGRLEN_7;

    iPos2 = iPos1 + 1;
    if(strResult[iPos2] !=',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    /*Utl_StrMid(iTemp, strResult, iPos1, iPos1);*/
    if(strResult[iPos1] > '3' || strResult[iPos1] < '0')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    strSMS->stat = (RIL_SMS_MODE)(strResult[iPos1] - '0');
    /* Getting the stat of the message*/

    if(strResult[iPos2+1] !='\"')
    {
        strSMS->name[0] =0;
        if(strResult[iPos2+1] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        iPos1 = iPos2 + 1 + 1;
        Utl_StrMid(iTemp, strResult, iPos1, (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
        /*Getting the length of the message and the name of the message default*/

    }
    else
    {
        iPos2 = iPos2 + 1;
        /*the position of the first '\"'*/
        iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iPos2 + 1));
        if(iPos1 > iEnd || iPos1 == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }

        Utl_StrMid(strSMS->name, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos1 - 1));

    
        if(strResult[ iPos1+1 ] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        iPos2 = iPos1 + 1;
        /* the next comma positon*/
        Utl_StrMid(iTemp, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strSMS);
            WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
    }
#endif

    /* finding the the end of the message, also the end of the PDU packet*/
    if(bFindOK)
    {
        iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iEnd + 1 + 1), iLength);
    }
    else
    {
        iBegin = strlen(strResult);
    }
    
    if(iBegin == iEnd + 1 + 1)
    {
        strcpy(strSMS->strPDU, "");
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strSMS);
        WIRELESS_INFO("ReadSMS(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }

    Utl_StrMid(strSMS->strPDU, strResult, (_SHORT16)(iEnd + 1 + 1), (_SHORT16)(iBegin -1));
    strSMS->idx = 0;        /* only one package, and index is unknow here */

    WIRELESS_INFO("strlen:%d, sublen:%d\r\n", strlen(strSMS->strPDU), ((iBegin -1) - (iEnd + 1 + 1)));

    strSMS->len = ((iBegin -1) - (iEnd + 1 + 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_SMS_ENTRY) ;
    WIRELESS_INFO("Read PDU:%s\r\n", strSMS->strPDU);
    
    return (_VOID*)strSMS;
}


/**
 * @brief Parse SMS content.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */
_VOID* WaveC_WmPs0705_ReadSMS_CDMA(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        iBegin, iEnd;
    _CHAR8        iTemp[10];
    RIL_SMS_ENTRY *strSMS = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strSMS = (RIL_SMS_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_SMS_ENTRY));
    /*clear first*/
    memset(strSMS, 0, sizeof(RIL_SMS_ENTRY));

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }

    iTemp[0] = 0;

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "AT^SMGR", 0);*/
    iBegin = Utl_StrFndL(strResult, "AT+CMGR", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }

    /*iBegin = Utl_StrFnd(strResult, "^SMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7));*/
    iBegin = Utl_StrFndL(strResult, "+CMGR:", (_SHORT16)(iBegin + AK_CMGR_ATCMGRLEN_7), iLength);

    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
    /*parsing the indicators of this message firstly, finding the first ATCMD_RETURN for this message*/
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + AK_CMGR_CMGRLEN_CDMA_6), iLength);

    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
#if 0 /* Kinson Temp */
    iPos1 = iBegin + AK_CMGR_CMGRLEN_7;

    iPos2 = iPos1 + 1;
    if(strResult[iPos2] !=',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
    /*Utl_StrMid(iTemp, strResult, iPos1, iPos1);*/
    if(strResult[iPos1] > '3' || strResult[iPos1] < '0')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
    strSMS->stat = (RIL_SMS_MODE)(strResult[iPos1] - '0');
    /* Getting the stat of the message*/

    if(strResult[iPos2+1] !='\"')
    {
        strSMS->name[0] =0;
        if(strResult[iPos2+1] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strSMS);
            return NULL;
        }
        iPos1 = iPos2 + 1 + 1;
        Utl_StrMid(iTemp, strResult, iPos1, (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strSMS);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
        /*Getting the length of the message and the name of the message default*/

    }
    else
    {
        iPos2 = iPos2 + 1;
        /*the position of the first '\"'*/
        iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iPos2 + 1));
        if(iPos1 > iEnd || iPos1 == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strSMS);
            return NULL;
        }

        Utl_StrMid(strSMS->name, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iPos1 - 1));

    
        if(strResult[ iPos1+1 ] != ',')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strSMS);
            return NULL;
        }
        iPos2 = iPos1 + 1;
        /* the next comma positon*/
        Utl_StrMid(iTemp, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(iTemp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strSMS);
            return NULL;
        }
        strSMS->len = (_USHORT16)(_SHORT16)Utl_Atoi(iTemp);
    }
#endif
    /* finding the the end of the message, also the end of the PDU packet*/
    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iEnd + 1), iLength);

    if(iBegin == iEnd + 1 + 1)
    {

        strcpy(strSMS->strPDU, "");
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }

    Utl_StrMid(strSMS->strPDU, strResult, (_SHORT16)(iEnd + 1 + 1), (_SHORT16)(iBegin -1));
    strSMS->idx = 0;        /* only one package, and index is unknow here */
    /*Getting pDU packet*/
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_SMS_ENTRY) ;

    return (_VOID*)strSMS;
} /* end WaveC_WmPs0705_ReadSMS_CDMA */


/**
 * @brief Read SMS list according to their status from SIM card
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 */

_VOID* WaveC_WmPs0705_ParseSimSms(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        iBegin, iEnd, iPos1, iPos2;
    _CHAR8        iTemp[16];
    _USHORT16        curIndex=0;
    _USHORT16        number=0;
    RIL_SMS_ENTRY *strSMS = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }

    /*check the number of the valid entries*/
    iBegin = Utl_StrFndL(strResult, "+CMGL:", 0, iLength);
    while(iBegin != -1)
    {
        number++;
        iBegin += AK_CMGL_CMGLLEN_7;
        iBegin = Utl_StrFndL(strResult, "+CMGL:", iBegin, iLength);
    }

    if(number > 0)
    {
        strSMS = (RIL_SMS_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_SMS_ENTRY) * number);
        /*clear first*/
        memset(strSMS, 0, (_USHORT16)(sizeof(RIL_SMS_ENTRY) * number));

        iBegin = Utl_StrFndL(strResult, "+CMGL:", 0, iLength);
        while(iBegin != -1)
        {
            iBegin += AK_CMGL_CMGLLEN_7;

            /*get index*/
            iPos1 = Utl_StrFndChr(strResult, ',', iBegin);
            Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iPos1 - 1), iLength);
            strSMS[curIndex].idx = (_SHORT16)Utl_Atoi(iTemp);

            /*get status*/
            if(strResult[iPos1 + 1] > '4' || strResult[iPos1 + 1] < '0')
            {
                iBegin = Utl_StrFndL(strResult, "+CMGL:", iBegin, iLength);
                continue;
            }
            strSMS[curIndex].stat = (RIL_SMS_MODE)(strResult[iPos1 + 1] - '0');

            /*get name and length*/
            iPos2 = iPos1 + 2;
            iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
            if(strResult[iPos2 + 1] != '\"')
            {
                Utl_StrMidL(iTemp, strResult, (_SHORT16)(iPos2 + 2), (_SHORT16)(iEnd - 1), iLength);
                strSMS[curIndex].len = (_USHORT16)Utl_Atoi(iTemp);
            }
            else
            {
                iPos1 = Utl_StrFndL(strResult, "\",", (_SHORT16)(iPos2 + 2), iLength);    
                Utl_StrMidL(iTemp, strResult, (_SHORT16)(iPos1+2), (_SHORT16)(iEnd - 1), iLength);
                strSMS[curIndex].len = (_USHORT16)Utl_Atoi(iTemp);
            }

            /*get PDU*/
            iPos1 = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iEnd + strSMS[curIndex].len), iLength);
            Utl_StrMidL(strSMS[curIndex].strPDU, strResult, (_SHORT16)(iEnd + 2), (_SHORT16)(iPos1 - 1), iLength);
            curIndex++;

            iBegin = Utl_StrFndL(strResult, "+CMGL:", iPos1, iLength);
        }
    }

    *result = RIL_prOK;
    *strNum = curIndex;
    *strSize = sizeof(RIL_SMS_ENTRY)*number;
    return strSMS;
} /*WaveC_WmPs0705_ParseSimSms*/

/**
 * @brief InquireServCentAddr
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 */
_VOID* WaveC_WmPs0705_InquireSmsCenter(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16  iBegin, iEnd, iPos1;
    _CHAR8   strSosca[16];
    RIL_SMS_CENADDRESS *strCenter = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strCenter = (RIL_SMS_CENADDRESS *)WIRELESS_MALLOC(sizeof(RIL_SMS_CENADDRESS));
    /*clear first*/
    memset(strCenter, 0, sizeof(RIL_SMS_CENADDRESS));
    
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCenter);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
      //  WIRELESS_TRACE;
        WIRELESS_FREE(strCenter);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CSCA:", AK_CSCA_ATCSCALEN_7, iLength);
    if (iBegin != -1)
    {
        /*get sca*/
          iEnd = _ril_SearchEnd(strResult, iBegin, iLength);
        if(strResult[iBegin + AK_CSCA_CSCALEN_7] == '"' && strResult[iEnd - 1] == '"')
        {
            Utl_StrMidL(strCenter->sca, strResult, (_SHORT16)(iBegin + AK_CSCA_CSCALEN_7 + 1), (_SHORT16)(iEnd - 2), iLength);

            /*get tosca*/
            iPos1 = Utl_StrFndL(strResult, ATCMD_RETURN, iEnd, iLength);
            if(iPos1 > iEnd)
            {
                Utl_StrMidL(strSosca, strResult, (_SHORT16)(iEnd+1), (_SHORT16)(iPos1-1), iLength);
                if(Utl_StrCmp(strSosca, "145") == 0)
                {
                    strCenter->tosca = RIL_INTER;
                }
                else
                {
                    strCenter->tosca = RIL_OTHERS;
                }
            }
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_SMS_CENADDRESS);
            return strCenter;
        }
    }
    *result = RIL_prGERROR;
    *strNum = 0;
    *strSize = 0;
    WIRELESS_TRACE;
    WIRELESS_FREE(strCenter);
    return NULL;
}

_VOID* WaveC_WmPs0705_SaveSmsPduData(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16  iBegin, iEnd;
    _CHAR8        strID[16];
    RIL_SMS_ENTRY *strSMS = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strSMS = (RIL_SMS_ENTRY *)WIRELESS_MALLOC(sizeof(RIL_SMS_ENTRY));
     /*clear first*/
    memset(strSMS, 0, sizeof(RIL_SMS_ENTRY));

    iBegin = Utl_StrFndL(strResult, "+CMGW:", 0, iLength);
    if (iBegin != -1)
    {
        iBegin += 6;
        iEnd = Utl_StrFndL(strResult, "\r", iBegin, iLength);
        if (iEnd == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strSMS);
            return NULL;
        }
        
        Utl_StrMidL(strID, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);
        strSMS->idx = (_SHORT16)Utl_Atoi(strID);

        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_SMS_ENTRY);
        return (_VOID*)strSMS;
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strSMS);
        return NULL;
    }
}
/**
 * @brief Parse cscb setting
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID* WaveC_WmPs0705_CbsSetting(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd;
    RIL_CBS_SETTING *cbsSetting = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    cbsSetting = (RIL_CBS_SETTING *)WIRELESS_MALLOC(sizeof(RIL_CBS_SETTING));
    /*clear first*/
    memset(cbsSetting, 0, sizeof(RIL_CBS_SETTING));
    
    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CSCB: ", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }

    /*parse mode*/
    iBegin += 7;
    if(strResult[iBegin] == '0')
    {
        cbsSetting->mode = 0;
    }
    else if(strResult[ iBegin ] == '1')
    {
        cbsSetting->mode = 1;
    }
    else
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }

    /*parse ID */
    iBegin += 2;
    if(strResult[ iBegin ] != '\"')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }
    iBegin++;
    iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }
    Utl_StrMidL(cbsSetting->strID, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);

    /*parse dcs*/
    iBegin = iEnd + 2;
    if(strResult[ iBegin ] != '\"')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }
    iBegin++;
    iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(cbsSetting);
        return NULL;
    }
    Utl_StrMidL(cbsSetting->strDCS, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_CBS_SETTING);
    return (_VOID*)cbsSetting;
}


/** * @brief Inquire the IMET
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 * ATD*#06#
 */
_VOID *WaveC_WmPsGsmCd_QueryIMEI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*  strIMEI = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    strIMEI = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strIMEI, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+CGSN", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    Utl_StrMid_N(strIMEI, iLength, strResult, (_SHORT16)(iBegin + 1 + 1), (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    printf("Get strImei:%s\r\n", strIMEI);
    return (_VOID*)strIMEI;
}

_VOID *M10_WmPsGsmCd_QueryCCID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*      strCCID = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;
    _CHAR8      cmdType;

    if(iLength > 128)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Error! M10_WmPsGsmCd_QueryCCID strResult length is too long:%d\r\n", iLength);
        return NULL;
    }
    WIRELESS_INFO("M10_WmPsGsmCd_QueryCCID strResult(%d):%s\r\n", iLength, strResult);

    strCCID = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strCCID, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strCCID);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strCCID);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strCCID);
        return NULL;
    }

    //weikk 20161213
    if((iBegin = Utl_StrFndL(strResult, "AT+CCID", 0, iLength)) >= 0)
    {
        cmdType = 0;
    }
    else if((iBegin = Utl_StrFndL(strResult, "+QCCID: ", 0, iLength)) >= 0)
    {
        cmdType = 1;
    }
    else if((iBegin = Utl_StrFndL(strResult, "AT+QCCID", 0, iLength)) >= 0)
    {
        cmdType = 2;
    }
    
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCCID);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 16), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCCID);
        return NULL;
    }
        
    if(cmdType == 0)
    {
        Utl_StrMid_N(strCCID, iLength, strResult, (_SHORT16)(iBegin + strlen("AT+CCID") + 3), (_SHORT16)(iEnd - 1));
    }
    else if(cmdType == 1)
    {
        Utl_StrMid_N(strCCID, iLength, strResult, (_SHORT16)(iBegin + strlen("+QCCID: ")), (_SHORT16)(iEnd - 1));
    }
    else if(cmdType == 2)
    {
        Utl_StrMid_N(strCCID, iLength, strResult, (_SHORT16)(iBegin + strlen("AT+QCCID") + 3), (_SHORT16)(iEnd - 1));
    }

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strCCID)+1;
    return (_VOID*)strCCID;
}

_VOID *C202_WmPsGsmCd_QueryCCID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*      strCCID = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    if(iLength > 128)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Error! C202_WmPsGsmCd_QueryCCID strResult length is too long:%d\r\n", iLength);
        return NULL;
    }
    return NULL;

    strCCID = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strCCID, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strCCID);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strCCID);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strCCID);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CCID:", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCCID);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 10), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCCID);
        return NULL;
    }

    Utl_StrMid_N(strCCID, iLength, strResult, (_SHORT16)(iBegin + 6), (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strCCID)+1;
    return (_VOID*)strCCID;
}

_VOID *M10_WmPsGsmCd_QueryIMSI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*      strIMSI = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    if(iLength > 128)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Error! M10_WmPsGsmCd_QueryIMSI strResult length is too long:%d\r\n", iLength);
        return NULL;
    }
    WIRELESS_INFO("M10_WmPsGsmCd_QueryIMSI strResult(%d):%s\r\n", iLength, strResult);

    strIMSI = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strIMSI, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+CIMI", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 16), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }

    Utl_StrMid_N(strIMSI, iLength, strResult, (_SHORT16)(iBegin + 11), (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strIMSI)+1;
    return (_VOID*)strIMSI;
}

#if 0
_VOID *M10_WmPsGsmCd_QueryIMSI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*     strIMSI = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    if(iLength > 128)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Error! M10_WmPsGsmCd_QueryIMSI strResult length is too long:%d\r\n", iLength);
        return NULL;
    }
    WIRELESS_INFO("M10_WmPsGsmCd_QueryIMSI strResult(%d):%s\r\n", iLength, strResult);

    strIMSI = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strIMSI, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CIMI:", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "\"", (iBegin+10), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }
    iBegin++;
    
    iEnd = Utl_StrFndL(strResult, "\"", iBegin, iLength);
    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMSI);
        return NULL;
    }

    Utl_StrMid_N(strIMSI, iLength, strResult, (_SHORT16)iBegin, (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strIMSI)+1;
    return (_VOID*)strIMSI;
}
#endif

/** * @brief Inquire the IMET
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 * ATD*#06#
 */
_VOID *M10_WmPsGsmCd_QueryIMEI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*  strIMEI = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    if(iLength > 128)
    {
        WIRELESS_INFO("Error! M10_WmPsGsmCd_QueryIMEI strResult length is too long:%d\r\n", iLength);
        return NULL;
    }

    strIMEI = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strIMEI, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+EGMR:", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    Utl_StrMid_N(strIMEI, iLength, strResult, (_SHORT16)(iBegin + 6 + 1), (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strIMEI)+1;
    return (_VOID*)strIMEI;
}

/** * @brief Inquire the ESN
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 * ATD*#06#
 */
_VOID *C202_WmPsGsmCd_QueryESN(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*  strIMEI = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    strIMEI = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strIMEI, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+GESN:", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

#if 0
    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
#endif
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    Utl_StrMid_N(strIMEI, iLength, strResult, (_SHORT16)(iBegin + 6), (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strIMEI)+1;
    return (_VOID*)strIMEI;
}

/** * @brief Inquire the IMET
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 * ATD*#06#
 */
_VOID *CM300_WmPsGsmCd_QueryESN(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8*  strIMEI = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    strIMEI = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strIMEI, 0, iLength);

    if (strResult == NULL)
    { 
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    
    iPos1 = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iPos1 == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+GSN", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

/*
    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    printf("iBegin = %d\r\n", iBegin);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }
    */
    iBegin = Utl_StrFndL(strResult, "+GSN:", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1 || iEnd > iPos1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strIMEI);
        return NULL;
    }

    Utl_StrMid_N(strIMEI, iLength, strResult, (_SHORT16)(iBegin + 1 + 5), (_SHORT16)(iEnd - 1));
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = strlen(strIMEI)+1;
    return (_VOID*)strIMEI;
}


/**
 * @brief: Parse ccwa URC report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *WaveC_WmPsGsmCd_pCCWAProcess(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd;
    RIL_CCWA_LIST *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strUrc = (RIL_CCWA_LIST *)WIRELESS_MALLOC(sizeof(RIL_CCWA_LIST));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_CCWA_LIST));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "+CCWA:", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ",", iBegin, iLength);
    if(iEnd == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = iBegin + AK_CCWA_LEN_7;

    if(strResult[iBegin] != '\"')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iEnd - 2));
    if (Utl_StrDigital(strUrc->telenumber) == EOS_FALSE)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_CCWA_LIST);
    return (_VOID*)strUrc;
}/* end WaveC_WmPsGsmCd_pCCWAProcess*/


/**
 * @brief the Response of Call Waiting
 * @param _CHAR8* strResult: received AT result
 * @param RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *WaveC_WmPsGsmCd_CallWait(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _CHAR8            Temp[16];
    _USHORT16            iNum = 0;
    RIL_CCWA_LIST *strCallWait = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strCallWait = (RIL_CCWA_LIST *)WIRELESS_MALLOC(sizeof(RIL_CCWA_LIST) * MAX_CLASS_TYPE_NUM);
    /*clear first*/
    memset(strCallWait, 0, (_USHORT16)(sizeof(RIL_CCWA_LIST) * MAX_CLASS_TYPE_NUM));
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCallWait);
        return NULL;
    }
    iBegin = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n",(_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strCallWait);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CCWA:", 0, iLength);
    while(iBegin != -1)
    {
        iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
        iBegin = iBegin + AK_CCWA_LEN_7;
        if(strResult[iBegin] != '0' && strResult[iBegin] != '1')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCallWait);
            return NULL;
        }
        strCallWait[iNum].status = (RIL_QUERY_STATUS)(strResult[iBegin] - '0');

        iPos1 = iBegin + 1;
        if(strResult[iPos1] != ',')
        {
            *result = RIL_prOK;
            *strNum= 0;    
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCallWait);
            return NULL;
        }
        Utl_StrMid(Temp, strResult, (_SHORT16)(iPos1 + 1), (_SHORT16)(iEnd -1));
        if(Utl_StrDigital(Temp) == EOS_FALSE)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(strCallWait);
            return NULL;
        }

        strCallWait[iNum].Class = (RIL_DATA_CLASS)Utl_Atoi(Temp);
        iNum++;

        iBegin = Utl_StrFndL(strResult, "+CCWA:", (_SHORT16)(iEnd + 1), iLength);
    }
    *result = RIL_prOK;
    *strNum = iNum;
    *strSize = sizeof(RIL_CCWA_LIST) * iNum;
    if(0 == iNum)
    {
        WIRELESS_TRACE;
        WIRELESS_FREE(strCallWait);
    }
    return strCallWait;
}

/**
 * @brief the Response of Forward
 * @param _CHAR8* strResult: received AT result
 * @param RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 */
_VOID *WaveC_WmPsGsmCd_CallForward(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1;
    _USHORT16    iNum = 0;
    _INT32    dataClass;
    T_STR_100    strTemp;
    RIL_CCFC_FWDCALLS *strForward = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }

    /*check the number of the valid call forward entries*/
    iBegin = Utl_StrFndL(strResult, "+CCFC:", 0, iLength);
    while(iBegin != -1)
    {
        iNum++;
        iBegin += AK_CCFC_SKIPCCFC_7;
        iBegin = Utl_StrFndL(strResult, "+CCFC:", iBegin, iLength);
    }

    if(0 == iNum)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    strForward = (RIL_CCFC_FWDCALLS*)WIRELESS_MALLOC(sizeof(RIL_CCFC_FWDCALLS)*iNum);
    memset(strForward, 0, sizeof(RIL_CCFC_FWDCALLS)*iNum);

    iNum = 0;
    iBegin = Utl_StrFndL(strResult, "+CCFC:", 0, iLength);
    while(iBegin != -1)
    {
        iBegin += AK_CCFC_SKIPCCFC_7;

        /*check status*/
        if(strResult[iBegin] == '0')
        {
            strForward[iNum].Status = RIL_STATUS_NACTIVE;
        }
        else
        {
            strForward[iNum].Status = RIL_STATUS_ACTIVE;
        }
        
        /*check class*/
        iBegin += 2;
        iEnd = _ril_SearchEnd(strResult, iBegin, iLength);
        Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);
        dataClass = Utl_Atoi(strTemp);
        /*infineon,merge,class may be "16","32","64","128",they are all type "2"*/
        if((dataClass == 16) || (dataClass == 32) || 
                (dataClass == 64) || (dataClass == 128) ||
                (dataClass == 256))
        {
            strForward[iNum].Class = RIL_dcDATA;
        }
        else
        {
            strForward[iNum].Class = (RIL_DATA_CLASS)dataClass;
        }

        iPos1 = Utl_StrFndL(strResult, ATCMD_RETURN, iEnd, iLength);
        if(iPos1 > iEnd)
        {
            /*check number*/
            iBegin = Utl_StrFndL(strResult, "\"", iEnd, iLength);
            iEnd = Utl_StrFndL(strResult, "\"", ++iBegin, iLength);
            Utl_StrMidL(strForward[iNum].Number, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);

            /*check type*/
            iBegin = iEnd + 2;
            iEnd = _ril_SearchEnd(strResult, iBegin, iLength);
            Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);
            if(Utl_StrCmp(strTemp, "145") == 0)
            {
                strForward[iNum].Type = RIL_INTER;
            }
            else
            {
                strForward[iNum].Type = RIL_OTHERS;
            }
            if(iPos1 > iEnd)
            {
                /*check sub-address*/
                iBegin = iEnd + 1;
                iEnd = _ril_SearchEnd(strResult, iBegin, iLength);

                /*check sub-address type*/
                iBegin = iEnd + 1;
                iEnd = _ril_SearchEnd(strResult, iBegin, iLength);
                if(iPos1 > iEnd)
                {
                    /*check time*/
                    iBegin = iEnd + 1;
                    iEnd = _ril_SearchEnd(strResult, iBegin, iLength);
                    Utl_StrMidL(strTemp, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);
                    strForward[iNum].Time = (_SHORT16)Utl_Atoi(strTemp);
                }
            }
        }
        iNum++;
        iBegin = Utl_StrFndL(strResult, "+CCFC:", iBegin, iLength);
    }
    *result = RIL_prOK;
    *strNum = iNum;
    *strSize = sizeof(RIL_CCFC_FWDCALLS)*iNum;
    return strForward;
}

/**
 * @brief Check the status of COLR
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * ATD*#77#
 */
_VOID *WaveC_WmPsGsmCd_CheckCOLR(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin;
    RIL_COL_MODE *strItem = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strItem = (RIL_COL_MODE *)WIRELESS_MALLOC(sizeof(RIL_COL_MODE));
    /*clear first*/
    memset(strItem, 0, sizeof(RIL_COL_MODE));

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+COLR", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+COLR:", (_SHORT16)(iBegin + 1), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = iBegin + AK_COLR_LEN_7;
    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '2')    /*     SIM300*/
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    *strItem = (RIL_COL_MODE)(strResult[ iBegin ] - '0');        
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_COL_MODE);
    return (_VOID*)strItem;
}

/**
 * @brief Check the status of COLP
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 * ATD*#76#
 */
_VOID *WaveC_WmPsGsmCd_CheckCOLP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd;
    RIL_COLP_INQUIRY *strItem = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    
    strItem = (RIL_COLP_INQUIRY *)WIRELESS_MALLOC(sizeof(RIL_COLP_INQUIRY));
    /*clear first*/
    memset(strItem, 0, sizeof(RIL_COLP_INQUIRY));

    if (strResult == NULL) 
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = (_SHORT16)Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

/*    iBegin = Utl_StrFnd(strResult, "ATD*#76#", 0);*/
    iBegin = Utl_StrFndL(strResult, "AT+COLP", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+COLP:", (_SHORT16)(iBegin + 1), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);

    iBegin = iBegin + AK_COLP_LEN_7;
/*    if(strResult[ iBegin ] != '0')                            */    
    if (strResult[ iBegin ] < '0' || strResult[ iBegin ] > '1')    /*  m300*/
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    strItem->colpn = (RIL_COLP_SET)(strResult[ iBegin ] - '0');
    iBegin++;

    if(strResult[ iBegin ] != ',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = iBegin + 1;
/*    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '1')*/
    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '2') /* sim300*/
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    strItem->colpm = (RIL_COL_MODE)(strResult[ iBegin ] - '0');

    if(iEnd != iBegin + 1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    else
    {
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_COLP_INQUIRY);
        return (_VOID*)strItem;
    }
}

/**
 * @brief Check the status of CLIR
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * ATD*#31#
 */
_VOID *WaveC_WmPsGsmCd_CheckCLIR(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    RIL_GSM_CLIR *strItem = NULL;
    strItem = (RIL_GSM_CLIR *)WIRELESS_MALLOC(sizeof(RIL_GSM_CLIR));
    /*clear first*/
    memset(strItem, 0, sizeof(RIL_GSM_CLIR));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin =(_SHORT16) Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

/*    iBegin = Utl_StrFndL(strResult, "ATD*#31#", 0, iLength);    */
    iBegin = Utl_StrFndL(strResult, "AT+CLIR", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CLIR:", (_SHORT16)(iBegin + 1), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);

    iBegin = iBegin + AK_CLIP_LEN_7;
    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '2')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    strItem->n = (RIL_CLIR_MODE)(strResult[ iBegin ] - '0');

    iBegin = iBegin + 1;
    if(strResult[ iBegin ] != ',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = iBegin + 1;

    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '4')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    strItem->m = (RIL_CLIR_STAT)(strResult[ iBegin ] - '0');

    if(iEnd != iBegin + 1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    else
    {    
        *result = RIL_prOK;
        *strNum = 1;
         *strSize = sizeof(RIL_GSM_CLIR);
        return (_VOID*)strItem;
    }
}

/**
 * @brief Check the status of CLIP
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * ATD*#30#
 */
_VOID *WaveC_WmPsGsmCd_CheckCLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin;
    RIL_GSM_CLIP *strItem = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strItem = (RIL_GSM_CLIP *)WIRELESS_MALLOC(sizeof(RIL_GSM_CLIP));
    /*clear first*/
    memset(strItem, 0, sizeof(RIL_GSM_CLIP));

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+CLIP", 0, iLength);
    if(iBegin == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CLIP:", (_SHORT16)(iBegin + 1), iLength); 
    iBegin = iBegin + AK_CLIP_LEN_7;

    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '1')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    if(strResult[iBegin] == '0')
        strItem->n = RIL_CLIP_Disable;
    else
        strItem->n = RIL_CLIP_Enable;

    iBegin = iBegin + 1;
    if(strResult[ iBegin ] != ',')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    iBegin = iBegin + 1;

    if(strResult[ iBegin ] < '0' || strResult[ iBegin ] > '2')
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strItem);
        return NULL;
    }

    strItem->m = (RIL_CLIP_MODE)(strResult[iBegin] -'0');

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_GSM_CLIP);
    return (_VOID*)strItem;
}

_VOID* WaveC_WmPs0705_GetFun(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8     strTemp[16];
    _INT32   *funstatus = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+CFUN:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    start = start + 7;
    end   = Utl_StrFndL(strResult, "OK", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(start));
    funstatus = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));
    *funstatus = Utl_Atoi(strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return funstatus;
}

_VOID* WaveC_WmPs0705_GetSimVol(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8     strTemp[16];
    _INT32   *SimVol = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+QSIMVOL:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }
    
    start = start + 9;
    
    end  = Utl_StrFndL(strResult, "OK", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(start));
    SimVol = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));
    *SimVol = Utl_Atoi(strTemp);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return SimVol;
}

/**
 * @brief Iniquire Call Barring with ATD
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 */
_VOID *WaveC_WmPsGsmCd_CallBarring(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
      _SHORT16 iBegin = 0;
      _SHORT16 iEnd = 0;
      _SHORT16 i = 0;
    _USHORT16    iNum = 0;
    _SHORT16    iOffset = AK_STRING_BEG;
    _CHAR8    iStat[16];
    _CHAR8    iOper[16];
    _CHAR8    iClass[16];
    RIL_CLCK_LOCK *strLock = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    strLock = (RIL_CLCK_LOCK *)WIRELESS_MALLOC(sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM);
    /*clear first*/
    memset(strLock, 0, (_USHORT16)(sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }
    /* Check to see if Module returns OK at end of string. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength)) == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if Module returns AT command. If it does not, function returns error. */
    if ((Utl_StrFndL(strResult, "AT+CLCK", AK_STRING_BEG, iLength)) != AK_STRING_BEG)
    {   
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if OK is the only returned parameter. */
    if ((Utl_StrFndL(strResult, "OK\r\n", AK_STRING_BEG, iLength)) == (AK_STRING_BEG + AK_CLCK_SKIPATCLCK_9))
    {   
        *result = RIL_prOK;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strLock);
        return NULL;
    }

    /* Check to see if the required command string is in result string. */
    iBegin = Utl_StrFndL(strResult, "+CLCK:", AK_STRING_BEG, iLength);
    if (iBegin != -1)
    {
        /* Loop through whole string to get different classes of locks. */
        while ((Utl_StrFndL(strResult, "+CLCK:", iOffset, iLength)) != -1)
        {
            /* Skip to beginning of status parameter. */
            iOffset = Utl_StrFndL(strResult, "+CLCK:", iOffset, iLength);
            iBegin = iOffset + AK_CLCK_SKIPCLCK_7;
            iOffset = iBegin;

            iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
            if (iEnd - iBegin == 2)
            {
                Utl_StrMid(iOper, strResult, iBegin, (_SHORT16)(iBegin+1));
                if (Utl_StrCmp(iOper,"AO") == 0)
                    strLock[iNum].Fac = RIL_AO;
                else if (Utl_StrCmp(iOper, "OI") == 0)
                    strLock[iNum].Fac = RIL_OI;
                else if (Utl_StrCmp(iOper, "OX") == 0)
                    strLock[iNum].Fac = RIL_OX;
                else if (Utl_StrCmp(iOper, "AI") == 0)
                    strLock[iNum].Fac = RIL_AI;
                else if (Utl_StrCmp(iOper, "IR") == 0)
                    strLock[iNum].Fac = RIL_IR;
                else 
                {
                    *result = RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                iBegin=iEnd + 1;
            }

            iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
            if (iEnd == -1)
            {   
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strLock);
                return NULL;
            }

            if ((iEnd - iBegin) < 1)
            {   
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strLock);
                return NULL;
            }

            if ((iEnd - iBegin) == 1)
            {
                /* Verify validity of status parameter, 0 or 1. Else return error. */
                Utl_StrMid(iStat, strResult, iBegin, iBegin);
                if ((iStat[0] != '0') && (iStat[0] != '1'))
                {   
                    *result = RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* If status parameter is 0, set lock off. Else set lock on. */
                if (iStat[0] == '0')
                    strLock[iNum].mode = RIL_CLCK_Unlock;

                else
                    strLock[iNum].mode = RIL_CLCK_Lock;        /* status*/
            }

            else
            {    /*Find comma at end of status parameter. */
                iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
                if (iEnd == -1)
                {   
                    *result = RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* Verify length of status parameter is 1 char. */
                if ((iEnd - iBegin) != 1)
                {   
                    *result = RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* Verify validity of status parameter, 0 or 1. Else return error. */
                Utl_StrMid(iStat, strResult, iBegin, iBegin);
                if ((iStat[0] != '0') && (iStat[0] != '1'))
                {   
                    *result = RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /* If status parameter is 0, set lock off. Else set lock on. */
                if (iStat[0] == '0')
                    strLock[iNum].mode = RIL_CLCK_Unlock;

                else
                    strLock[iNum].mode = RIL_CLCK_Lock;        /* status*/

                /* Skip to beginning of class parameter.
                Find <CR> at end of class parameter. */
                /*iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, ++iEnd, iLength);*/
                iBegin = Utl_StrFndL(strResult, AT_NEW_LINE, ++iEnd, iLength);    /*wavecom,merge*/
                if(iBegin == -1)
                {
                    iBegin = Utl_StrFndL(strResult, ATCMD_RETURN, iEnd, iLength);
                }
                if (iBegin == -1)
                {   
                    *result = RIL_prGERROR;
                    *strNum = 0;
                    *strSize = 0;
                    WIRELESS_TRACE;
                    WIRELESS_FREE(strLock);
                    return NULL;
                }

                /*Utl_StrMid(iClass, strResult, iEnd, --iBegin);*/
                Utl_StrMid(iClass, strResult, iEnd, (_SHORT16)(iBegin - 2)); /*wavecom,merge*/


                /* Verify class parameter are valid digits. */
                i = 0;
                while ((iClass[i] >= '0') && (iClass[i] <= '9'))
                    i++;

                /*
                if ((i-1) != (iEnd - iBegin))
                    return RIL_prGERROR;
                */


                /* Convert class data from string to integer. Store it and keep looping. */
                strLock[iNum].Class = (_UCHAR8)(_SHORT16)Utl_Atoi(iClass);        /* class*/
            }

            /* Increment number of locks. */
            iNum++;
        }
        /* Store number of locks. Return OK. */
        *strNum = iNum;

        for(i=0; i<iNum; i++)
        {
            if (Utl_StrFndL(strResult, "AO", 0, iLength) != -1)
                strLock[i].Fac = RIL_AO;
            else if (Utl_StrFndL(strResult, "OI", 0, iLength) != -1)
                strLock[i].Fac = RIL_OI;
            else if (Utl_StrFndL(strResult, "OX", 0, iLength) != -1)
                strLock[i].Fac = RIL_OX;
            else if (Utl_StrFndL(strResult, "AI", 0, iLength) != -1)
                strLock[i].Fac = RIL_AI;
            else if (Utl_StrFndL(strResult, "IR", 0, iLength) != -1)
                strLock[i].Fac = RIL_IR;
            else 
            {   
                *result = RIL_prGERROR;
                *strNum = 0;
                *strSize = 0;
                WIRELESS_TRACE;
                WIRELESS_FREE(strLock);
                return NULL;
            }
        }

        *result = RIL_prOK;
        *strSize = sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM;
        return (_VOID*)strLock;
    }
    else
    {
        *strNum = iNum;
        *result = RIL_prOK;
        *strSize = sizeof(RIL_CLCK_LOCK) * MAX_CLASS_TYPE_NUM;
        return (_VOID*)strLock;
    }
   
}    /* end WaveC_WmPsGsmCd_CallBarring */

/*
*@ brief: query the cellphone's band used
* @param _CHAR8* strResult: received AT result
* @param  RIL_PARSE_RES* result:AT parse result
* @param _UINT32 * strNum:number of data structure 
* @param  _UINT32 *strSize:szie of data structure 
*@0:stand for 900 (EGSM)
*@1:stand for 1800 (DCS)
*@2:stand for 900/1800,and auto(DUAL)
* @retvalue _VOID*:parse data
*/

_VOID *WaveC_WmPsGsmCd_QueryBand(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[16];
    _INT32   temp, temp1;
    _UCHAR8 *band = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    band = (_UCHAR8 *)WIRELESS_MALLOC(iLength);
    memset(band, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(band);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(band);
        return NULL;
    }
    start = 0;
    if (Utl_StrFndL(strResult, "OK", start, iLength) == -1)            /* no OK */
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(band);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+XBANDSEL: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(band);
        return NULL;
    }

    start = start + 11;
    end = Utl_StrFndL(strResult, ",", start, iLength);
    if(end == -1)
    {
        end = Utl_StrFndL(strResult, "\r", start, iLength);
        if(end == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(band);
            return NULL;
        }
        Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));

        temp = (_SHORT16)Utl_Atoi(strTemp);
        
        switch(temp)
        {
        case 900:
            *band = 0;
            break;
        case 1800:
            *band = 1;
            break;
        default:
            *band = 2;
            break;
        }
    }
    else
    {
        Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
        temp = (_SHORT16)Utl_Atoi(strTemp);
        start = end+1;
        end = Utl_StrFndL(strResult, "\r", start, iLength);
        if(end == -1)
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(band);
            return NULL;
        }
        Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
        temp1 = (_SHORT16)Utl_Atoi(strTemp);
        if(temp == 900 && temp1 == 1800)
        {
            *band = 2;
        }
    }
    
    {
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = iLength;
        return (_VOID *)band;
    }

}/*end of WaveC_WmPsGsmCd_QueryBand();*/

/**
 * @brief Inquire GPRS attach state
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID *WaveC_WmPsGprs_InquireGprsState(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin;
    _BOOL *str = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    str = (_BOOL *)WIRELESS_MALLOC(sizeof(_BOOL));
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str);
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, "+CGATT: ", 0, iLength);
    if(iBegin == -1) 
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str);
        return NULL;
    }
    else
    {
        if(strResult[iBegin+8] == '1')
        {
            *str = EOS_TRUE;
        }
        else
        {
            *str = EOS_FALSE;
        }
        
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(_BOOL);
        return str;
    }
}

/* @brief: SIM storage full report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID *WaveC_WmPsExt_InquSMSFULL(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin;
    RIL_SMS_FULL *str = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    str = (RIL_SMS_FULL *)WIRELESS_MALLOC(sizeof(RIL_SMS_FULL));
    /*clear first*/
    memset(str, 0, sizeof(RIL_SMS_FULL));

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str);
        return NULL;
    }
    iBegin = Utl_StrFndL(strResult, "+CIEV: 8,1", 0, iLength);
    if(iBegin == -1) 
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(str);
        return NULL;
    }
    else
    {
        *str = RIL_SM_FULL_NEWSM;
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_SMS_FULL);
        return (_VOID*)str;
    }

}


/**
 * @brief InquirePINSCounter
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID *WaveC_WmPsExt_InquirePinCount(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _UCHAR8 counter = 0, i = 0;
    _SHORT16 iBegin, iEnd;
    _CHAR8 pinCounter[16];
    _UCHAR8    *nCounts = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    nCounts = (_UCHAR8 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(nCounts, 0, iLength);
    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(nCounts);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(nCounts);
        return NULL;
    }
    
    iBegin = Utl_StrFndL(strResult, "OK\r\n", (_SHORT16)(iLength - AK_OK_POS_5), iLength);
    if(iBegin == -1)
    {
        *nCounts = counter;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(nCounts);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "AT+XPINCNT", 0, iLength);     /*wavecom,merge*/


    if(iBegin == -1)
    {
        *nCounts = counter;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(nCounts);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+XPINCNT:", SPIC_STRING_COMMAND_LEN, iLength);/*wavecom,merge    */

    if(iBegin == -1)
    {
        *nCounts = counter;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(nCounts);
        return NULL;
    }
    
    /*iBegin = iBegin + 11;        */    /*wavecom,merge,wavecom_ulc1*/
    iBegin = iBegin + 10;            /*wavecom,merge,wavecom_ulc2*/

    /*iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);*/

    for (i=0;  i<4;  i++)
    {
        if (i == 3)
        {
            /*the last param*/
            iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
        }
        else
            iEnd = Utl_StrFndL(strResult, AT_COMMA, (_SHORT16)(iBegin + 1), iLength);
        if(iEnd < iBegin)
        {
            *nCounts = counter;
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(nCounts);
            return NULL;
        }
        Utl_StrMid(pinCounter, strResult, iBegin, (_SHORT16)(iEnd - 1));
        if(Utl_StrDigital(pinCounter) == EOS_FALSE)
        {
            *nCounts = counter;
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(nCounts);
            return NULL;

        }
        nCounts[i] = (_UCHAR8)Utl_Atoi(pinCounter);
        iBegin = iEnd + 1;          /*wavecom_ulc2*/
        /*iBegin = iEnd + 2;*/        /*wavecom,merge,wavecom_ulc1*/

    }
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)nCounts;
}

/**
 * @brief Get battery level.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID *WaveC_WmPsExt_InquireBatteryLevel(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end, temp = 0;
    _CHAR8        strTemp[16] = {0};
    RIL_BAT_LEVEL *BatLevel = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    BatLevel = (RIL_BAT_LEVEL *)WIRELESS_MALLOC(sizeof(RIL_BAT_LEVEL));
    /*clear first*/
    memset(BatLevel, 0, sizeof(RIL_BAT_LEVEL));

    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(BatLevel);
        return NULL;
    }

    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(BatLevel);
        return NULL;
    }

    /* Changed for WaveC, because WaveC do not support AT+CBC now*/
    start = Utl_StrFndL(strResult, "+CBC:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(BatLevel);
        return NULL;
    }

    start += 6;    /*length of +CBC:*/
    temp = Utl_StrFndL(strResult, ",", start, iLength);
    if(temp < start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(BatLevel);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(temp - 1));
    BatLevel->battery = (_SHORT16)Utl_Atoi(strTemp);

    start = temp + 1;
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
    temp = Utl_StrFndL(strResult, ",", start, iLength);
    if(temp < start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(BatLevel);
        return NULL;
    }
    start = temp + 1;
#endif
    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        //WIRELESS_TRACE;
        WIRELESS_FREE(BatLevel);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    BatLevel->level = (_SHORT16)Utl_Atoi(strTemp);


    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_BAT_LEVEL);
    return (_VOID*)BatLevel;

}

/**
 * @brief Get battery level.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval battery level
 */
_VOID *WaveC_WmPsExt_InquireBatteryVoltage(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start;
    _CHAR8        strTemp[16];
    _SHORT16 *batteryVoltage = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    /*+XDRV: 10,5,0,4105*/
    batteryVoltage = (_SHORT16 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(batteryVoltage, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batteryVoltage);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batteryVoltage);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+XDRV: 10,5,0,", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batteryVoltage);
        return NULL;
    }
    start += 14;
    
    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(start + 3));
    *batteryVoltage = (_SHORT16)Utl_Atoi(strTemp);

    if((*batteryVoltage < 0) || (*batteryVoltage > 6000))
    {
        *batteryVoltage = 0;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batteryVoltage);
        return NULL;
    }
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)batteryVoltage;

}

/**
 * @brief Get envirement temperature.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval battery level
 */
_VOID *WaveC_WmPsExt_InquireEnvirTemper(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end;
    _CHAR8        strTemp[16];
    _SHORT16 *envirTemper = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    envirTemper = (_SHORT16 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(envirTemper, 0, iLength);
    /*+XDRV: 10,7,0,34*/
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(envirTemper);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(envirTemper);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+XDRV: 10,7,0,", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(envirTemper);
        return NULL;

    }
    start += 14;

    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(envirTemper);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    *envirTemper = (_SHORT16)Utl_Atoi(strTemp);

    if((*envirTemper < -30) || (*envirTemper > 60))
    {
        *envirTemper = 0;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(envirTemper);
        return NULL;
    }
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)envirTemper;

}

/**
 * @brief Get battery temperature.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval battery level
 */
_VOID *WaveC_WmPsExt_InquireBattTemper(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end;
    _CHAR8        strTemp[16];
    _SHORT16 *battTemper = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    battTemper = (_SHORT16 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(battTemper, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(battTemper);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(battTemper);
        return NULL;
    }
    /*+XDRV: 10,6,0,90*/
    start = Utl_StrFndL(strResult, "+XDRV: 10,6,0,", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(battTemper);
        return NULL;
    }
    start += 14;

    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(battTemper);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    *battTemper = (_SHORT16)Utl_Atoi(strTemp);

    if((*battTemper < -30) || (*battTemper > 60))
    {
        *battTemper = 0;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(battTemper);
        return NULL;
    }
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)battTemper;

}

/**
 * @brief Get battery ID ADC value.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @return
 * @retval
 */
_VOID *WaveC_WmPsExt_InquireBattTemperADC(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end;
    _CHAR8        strTemp[16];
    _SHORT16       *batIDValue = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    /*+XDRV: 9,5,0,4095*/
    batIDValue = (_SHORT16 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(batIDValue, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batIDValue);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batIDValue);
        return NULL;
    }
    start = Utl_StrFndL(strResult, "+XDRV: 9,5,0,", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batIDValue);
        return NULL;
    }
    start += 13;

    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batIDValue);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    *batIDValue = (_SHORT16)Utl_Atoi(strTemp);

    if(*batIDValue < 0)
    {
        *batIDValue = -1;
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(batIDValue);
        return NULL;
    }
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)batIDValue;

}


/**
 * @brief Get static EEP value.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @return 
 * @retval
 */
_VOID *WaveC_WmPsExt_InquireStaticEEP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end;
    _CHAR8        strTemp[16];
    _USHORT16       *eepValue = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    eepValue = (_USHORT16 *)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(eepValue, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(eepValue);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(eepValue);
        return NULL;
    }
    /*+XDRV: 9,3,0,0*/
    start = Utl_StrFndL(strResult, "+XDRV: 9,3,0,", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(eepValue);
        return NULL;
    }
    start += 13;

    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(eepValue);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    
    *eepValue = (_USHORT16)Utl_Atoi(strTemp);
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)eepValue;

}

/**
 * @brief Get static EEP value.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @return 
 * @retval
 */
_VOID *WaveC_WmPsExt_InquireChipID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end;
    _CHAR8*      strChipID = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    strChipID = (_CHAR8*)WIRELESS_MALLOC(iLength);
    /*clear first*/
    memset(strChipID, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strChipID);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strChipID);
        return NULL;
    }
    /*+XDRV: 9,4,646a2007b00682400885*/
    start = Utl_StrFndL(strResult, "+XDRV: 9,4,", 0, iLength);

    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strChipID);
        return NULL;
    }
    start += 11;

    end = Utl_StrFndL(strResult, "\r", start, iLength);

    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strChipID);
        return NULL;
    }
    
    if((end-start) != MODEM_CHIP_ID_LEN)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strChipID);
        return NULL;
    }

    Utl_StrMid_N(strChipID, iLength, strResult, start, (_SHORT16)(end - 1));
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = iLength;
    return (_VOID*)strChipID;
}


/**
 * @brief Parse Module State.
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID *WaveC_WmPsExt_ParseModuleState(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{    
    RIL_MODULE_STATE *state = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    state = (RIL_MODULE_STATE *)WIRELESS_MALLOC(sizeof(RIL_MODULE_STATE));
    /*clear first*/
    memset(state, 0, sizeof(RIL_MODULE_STATE));

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(state);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(state);
        return NULL;
    }
    if(Utl_StrFndL(strResult, "+XIER: normal", 0, iLength) != -1)
    {
        *state = RIL_MODULE_NORMAL;
    }
    else if(Utl_StrFndL(strResult, "+XIER: charger", 0, iLength) != -1)
    {
        *state = RIL_MODULE_GHOST;
    }
    else if(Utl_StrFndL(strResult, "+XIER: RTC", 0, iLength) != -1)
    {
        *state = RIL_MODULE_ALARM;
    }
    else
    {
        *state = RIL_MODULE_INVALID;
    }
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_MODULE_STATE);
    return (_VOID*)state;
}

/*_VOID *WaveC_WmPsExt_FieldTestReportNeig(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _INT32    i = 0, j = 0, k = 0, begin;
    _UCHAR8    strTemp[20];
    _UCHAR8    index;
    _UCHAR8    *neigCell[NEIG_ITEM_NUM];
    for(k=0; k<NEIG_ITEM_NUM; k++)
    {
        neigCell[k] =  NULL;
        neigCell[k] =  (_UCHAR8 *)WIRELESS_MALLOC(sizeof(_UCHAR8) * 8);
        memset(neigCell[k], 0, sizeof(_UCHAR8) * 8);
    }
    index = (_UCHAR8)(g_pCellTest->index + 1);
    sprintf(strTemp, "+CENG: %d,\"", index);
    begin = Utl_StrFnd(strResult, strTemp, 0);
    if(begin < 0)
    {
        *result = RIL_prGERROR;
    }

    strResult += (begin + 10);

    while(*strResult != '\"')
    {
        if(*strResult == ',')
        {
            neigCell[ i ][ j ] = 0;
            i++;
            j = 0;
        }
        else
        {
            neigCell[ i ][ j++ ] = *strResult;
        }
        strResult++;
    }
    neigCell[ i ][ j ] = 0;

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_UCHAR8) * 8 * NEIG_ITEM_NUM;
    return (_VOID*)(*neigCell);
}
*/

/**
 * @brief Parse Field Test Report AT+CGED=0
 * all parse data in data struc
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 */
_VOID *WaveC_WmPsExt_FieldTestReport(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16   begin, end, iPos1;    
    RIL_CELL_TEST *CellTest = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    CellTest = WIRELESS_MALLOC(sizeof(RIL_CELL_TEST));
    memset(CellTest, 0, sizeof(RIL_CELL_TEST));

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    /*0*/
    begin = Utl_StrFndL(strResult,"LAC:", 0, iLength);    
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 4;
    end    = Utl_StrFndL(strResult, ",", begin, iLength);    
    Utl_StrMidL(CellTest->servCell[0], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /* 1 */
    begin = Utl_StrFndL(strResult, "CI:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 3;
    end = Utl_StrFndL(strResult, ",", begin, iLength);
    Utl_StrMidL(CellTest->servCell[1], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /* 2 */
    begin = Utl_StrFndL(strResult, "BSIC:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 5;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[2], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /* 3 */
    begin = Utl_StrFndL(strResult, "Arfcn:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 6;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[3], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /* 4 */
    begin = Utl_StrFndL(strResult, "RxLevServ:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 10;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[4], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*17*/
    begin = Utl_StrFndL(strResult, "ms_txpwr:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 9;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[17], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);
    
    /*5*/
    begin = Utl_StrFndL(strResult, "txpwr:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 6;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[5], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*6*/
    begin = Utl_StrFndL(strResult, "RfChannels:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 11;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[6], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*7*/
    begin = Utl_StrFndL(strResult, "Arfcn_ded:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 10;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[7], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*8*/
    begin = Utl_StrFndL(strResult, "RxLevFull:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 10;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[8], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*9*/
    begin = Utl_StrFndL(strResult, "RxQualFull:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 11;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[9], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*10*/
    begin = Utl_StrFndL(strResult, "rx_acc_min:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 11;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[10], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*11*/
    begin = Utl_StrFndL(strResult, "ch_type:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 8;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[11], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*12*/
    begin = Utl_StrFndL(strResult, "ch_mode:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 8;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[12], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*13*/
    begin = Utl_StrFndL(strResult, "RxLevSub:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 9;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[13], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*14*/
    begin = Utl_StrFndL(strResult, "RxQualSub:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 10;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[14], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*15*/
    begin = Utl_StrFndL(strResult, "MCC:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 4;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[15], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    /*16*/
    begin = Utl_StrFndL(strResult, "MNC:", 0, iLength);
    if(begin < 0)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(CellTest);
        return NULL;
    }
    begin += 4;
    end = Utl_StrFndL(strResult, ",", begin, iLength);        
    Utl_StrMidL(CellTest->servCell[16], strResult, (_SHORT16)(begin), (_SHORT16)(end - 1), iLength);

    CellTest->servCellNUmber++;

    /*parse neig cell info*/
    begin = 0;
    iPos1 = Utl_StrFndL(strResult, "Neighbour Cell ", begin, iLength);
    while(iPos1 >= 0)
    {
        /*find parameters */
        /*0*/
        begin = Utl_StrFndL(strResult,"MCC:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 4;
        end = Utl_StrFndL(strResult, ",", begin, iLength);        
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][0], strResult, (_SHORT16)begin, (_SHORT16)(end-1), iLength);

        /* 1 */
        begin = Utl_StrFndL(strResult,"MNC:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 4;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][1], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        /* 2 */
        begin = Utl_StrFndL(strResult,"LAC:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 4;
        end = Utl_StrFndL(strResult,",",begin, iLength);        
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][2], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);    

        /* 3 */
        begin = Utl_StrFndL(strResult,"CI:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 3;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][3], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        /* 4 */
        begin = Utl_StrFndL(strResult,"BSIC:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 5;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][4], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        /* 5 */
        begin = Utl_StrFndL(strResult,"Arfcn:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 6;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][5], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        /* 6 */
        begin = Utl_StrFndL(strResult,"RxLev:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 6;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][6], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        /* 7 */
        begin = Utl_StrFndL(strResult,"C1_nc:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 6;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][7], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        /* 8 */
        begin = Utl_StrFndL(strResult,"C2_nc:", iPos1, iLength);
        if(begin < 0)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(CellTest);
            return NULL;
        }
        begin += 6;
        end = Utl_StrFndL(strResult,",",begin, iLength);
        Utl_StrMidL(CellTest->neigCell[CellTest->neigCellNumber][8], strResult,(_SHORT16)begin,(_SHORT16)(end-1), iLength);

        CellTest->neigCellNumber++;
        iPos1 = Utl_StrFndL(strResult, "Neighbour Cell ", begin, iLength);
    }
    
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_CELL_TEST);
    
    return (_VOID *)CellTest;
}


_VOID *WaveC_WmPsExt_CheckLatestURCEvent(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }    

    if (Utl_StrFndL(strResult, "AT+XIER", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }    

    *result = RIL_prOK;
    *strNum = 0;
       *strSize = 0;
    return NULL;
}

_VOID *WaveC_WmPsExt_ControlURCIndication(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        return NULL;
    }
    if (Utl_StrFndL(strResult, "OK\r\n", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }    

    if (Utl_StrFndL(strResult, "AT+XCDU", 0, iLength) == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }    

    *result = RIL_prOK;
    *strNum = 0;
    *strSize = 0;
    return NULL;
}
 /**
 * @brief parse query URC Indication
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
**/
_VOID *WaveC_WmPsExt_QueryURCIndication(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
    _CHAR8    strTemp[16];
    _SHORT16    i;
    _CHAR8 *urcData = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    urcData = (_CHAR8 *)WIRELESS_MALLOC(sizeof(_CHAR8)*5);/*parse data num*/

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(urcData);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(urcData);
        return NULL;
    }
     start =  Utl_StrFndL(strResult, "+CMER: ", 0, iLength);
    if(start == -1)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(urcData);
        return NULL;

    }

    start += 7;

    for(i=0; i<4; i++)
    {
        end = Utl_StrFndL(strResult, ",", start, iLength);
        if(end <= start)
        {
            *result = RIL_prERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_TRACE;
            WIRELESS_FREE(urcData);
            return NULL;
        }

        Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end-1));
        urcData[i] = (_CHAR8)Utl_Atoi(strTemp);

        start = end+1;
    }

    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if(end <= start)
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(urcData);
        return NULL;
    }

    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end-1));
    urcData[4] = (_CHAR8)Utl_Atoi(strTemp);
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = 5;
    return (_VOID *)urcData;
    
}
 /**
 * @brief parse Product ID
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
**/
_VOID *WaveC_WmPsV25_GetProductID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)

{
    _SHORT16 pos1=0, pos2=0;
    _CHAR8* strPID = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult) + 1;

    strPID = (_CHAR8*)WIRELESS_MALLOC(iLength);
    memset(strPID, 0, iLength);
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPID);
        return NULL;
    }
    if(ParseErrorID(strResult, iLength, result, strNum))
    {
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPID);
        return NULL;
    }
    pos2 = Utl_StrFndL(strResult, "OK\r\n", 0, iLength);
    if (pos2 == -1)    
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strPID);
        return NULL;
    }

    pos2 = pos2 - 4; /*ignore \n*/
    pos1 = pos2;
    while(strResult[pos1] != '\n')
    {
        pos1--;
    }
    pos1++;
    Utl_StrMid_N(strPID, iLength, strResult, pos1, pos2);
    
    {
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = iLength ;
        return (_VOID *)strPID ;
    }
   
}
/*_VOID *WaveC_WmPsV25_InquireIMEI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)

{     
    
    _SHORT16 off1=0, off2=0;
    _CHAR8 *str_imei = NULL;
    str_imei = (_CHAR8*)WIRELESS_MALLOC(strlen(strResult));
    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        str_imei = WIRELESS_FREE(str_imei);
        return NULL;
    }

    if (Utl_StrFnd(strResult,"AT+GSN", 0) != -1)

    {
        off1 = Utl_StrFnd(strResult, "\r\n", 0) + 2;
        off2 = Utl_StrFnd(strResult, "\r\n", off1);
        Utl_StrMid(str_imei, strResult, off1, off2);
        *strNum = 1;
        *strSize = strlen(strResult);
        return (_VOID *)str_imei;
        
    }
    else
    {
        *result = RIL_prERROR;
        *strNum = 0;
        *strSize = 0;
        str_imei = WIRELESS_FREE(str_imei);
        return NULL;
    }
 }
*/
 /**
 * @brief Inquire network register status
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
**/
_VOID *WaveC_WmPsUrc_CREG(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    _SHORT16 iLoc = 0;
    _CHAR8 iTemp[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_NET_STAT *urcStat = NULL;

    WIRELESS_INFO("WaveC_WmPsUrc_CREG %s\r\n", strResult);
    iBegin = Utl_StrFndL(strResult, "+CREG:", 0, iLength);
    iBegin += AK_CREG_CREGLEN_7;
    iLoc = iBegin;
    
    iBegin = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);

    if(iBegin == -1)
    {
        iBegin = iLoc;
    }
    
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
    
    if((iEnd - iBegin >= 1) && (iEnd - iBegin <= 3))
    {
        urcStat = (RIL_NET_STAT *)WIRELESS_MALLOC(sizeof(RIL_NET_STAT));

        Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);
        *urcStat = (RIL_NET_STAT)Utl_Atoi(iTemp);
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_NET_STAT);
        return urcStat;
    }
    
    *result = RIL_prGERROR;
    *strNum = 0;
    *strSize = 0;
    return NULL;
}

_VOID *WaveC_WmPsUrc_L81X_CGREG(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    _SHORT16 iLoc = 0;
    _CHAR8 iTemp[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_NET_STAT *urcStat = NULL;

    WIRELESS_INFO("WaveC_WmPsUrc_CGREG %s\r\n", strResult);
    iBegin = Utl_StrFndL(strResult, "+CGREG:", 0, iLength);
    iBegin += 8;
    iLoc = iBegin;
    
    iBegin = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);

    if(iBegin == -1)
    {
        iBegin = iLoc;
    }
    
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, iBegin, iLength);
    
    if((iEnd - iBegin >= 1) && (iEnd - iBegin <= 3))
    {
        urcStat = (RIL_NET_STAT *)WIRELESS_MALLOC(sizeof(RIL_NET_STAT));

        Utl_StrMidL(iTemp, strResult, iBegin, (_SHORT16)(iEnd-1), iLength);
        *urcStat = (RIL_NET_STAT)Utl_Atoi(iTemp);
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_NET_STAT);
        return urcStat;
    }
    
    *result = RIL_prGERROR;
    *strNum = 0;
    *strSize = 0;
    return NULL;
}


_VOID* WaveC_WmPsUrc_CallProgress(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16        start, end, cin, status;
    _CHAR8        strTemp[16];
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    RIL_CPROGRESS *pstr = NULL;
    /*+XPROGRESS: 1,3*/
    start = Utl_StrFndL(strResult, "+XPROGRESS: ", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    start += 12;

    end = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        return NULL;
    }
    Utl_StrMidL(strTemp, strResult, start, (_SHORT16)(end - 1), iLength);
    cin = (_SHORT16)Utl_Atoi(strTemp);

    start += 2;
    end = Utl_StrFndL(strResult, "\r", start, iLength);    
    if (end <= start)
    {
        *result = RIL_prGERROR;
        return NULL;
    }    
    Utl_StrMidL(strTemp, strResult, start, (_SHORT16)(end - 1), iLength);
    status = (_SHORT16)Utl_Atoi(strTemp);
    
    pstr = (RIL_CPROGRESS*)WIRELESS_MALLOC(sizeof(RIL_CPROGRESS));
    pstr->cin = cin;
    pstr->status = status;

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_CPROGRESS);
    return pstr;
}

/**
 * @brief: Parse clip URC report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *WaveC_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1;
    RIL_URC_CLIP *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strUrc =  (RIL_URC_CLIP *)WIRELESS_MALLOC(sizeof(RIL_URC_CLIP));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_URC_CLIP));

    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CLIP:", 0, iLength);
    if(iBegin == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = iBegin + AK_CLIP_LEN_7;

    if(strResult[iBegin] != '\"')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iBegin + 1));
    if(iPos1 == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    if (((iPos1 - 1) - (iBegin + 1)) + 1 > G_MaxTeleLen)
        Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iBegin + 1 + G_MaxTeleLen - 1));
    else
        Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iPos1 - 1));

    if(Utl_StrDigital(strUrc->telenumber) == EOS_FALSE)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iPos1 = iPos1 + 1;
    if(strResult[iPos1] != ',')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    /* Changed for GPHONE*/
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_URC_CLIP);
 
    return (_VOID*)strUrc;
}

_VOID *MC323_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1;
    RIL_URC_CLIP *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strUrc =  (RIL_URC_CLIP *)WIRELESS_MALLOC(sizeof(RIL_URC_CLIP));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_URC_CLIP));

    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CLIP:", 0, iLength);
    if(iBegin == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = iBegin + 6;

    if(strResult[iBegin] != '\"')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iBegin + 1));
    if(iPos1 == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    if (((iPos1 - 1) - (iBegin + 1)) + 1 > G_MaxTeleLen)
        Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iBegin + 1 + G_MaxTeleLen - 1));
    else
        Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iPos1 - 1));

    if(Utl_StrDigital(strUrc->telenumber) == EOS_FALSE)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iPos1 = iPos1 + 1;
    if(strResult[iPos1] != ',')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    /* Changed for GPHONE*/
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_URC_CLIP);
 
    return (_VOID*)strUrc;
}


/**
 * @brief: Parse clip URC report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *C202_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    RIL_URC_CLIP *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strUrc =  (RIL_URC_CLIP *)WIRELESS_MALLOC(sizeof(RIL_URC_CLIP));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_URC_CLIP));

    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CLIP:", 0, iLength);
    if(iBegin == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    sscanf( (_CHAR8 *)strResult, "+CLIP:%[0-9]", strUrc->telenumber);

    if(Utl_StrDigital(strUrc->telenumber) == EOS_FALSE)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

     *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_URC_CLIP);
 
    return (_VOID*)strUrc;
}

/**
 * @brief: Parse clip URC report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *CM300_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd;
    RIL_URC_CLIP *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strUrc =  (RIL_URC_CLIP *)WIRELESS_MALLOC(sizeof(RIL_URC_CLIP));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_URC_CLIP));

    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "CID:", 0, iLength);
    if(iBegin == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    sscanf( (_CHAR8 *)strResult, "CID: %[0-9]", strUrc->telenumber);

    if(Utl_StrDigital(strUrc->telenumber) == EOS_FALSE)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

     *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_URC_CLIP);
 
    return (_VOID*)strUrc;
}


/**
 * @brief: Parse COLp URC report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *WaveC_WmPsUrc_COLP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin, iEnd, iPos1;
    RIL_URC_COLP *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);

    strUrc =  (RIL_URC_COLP *)WIRELESS_MALLOC(sizeof(RIL_URC_COLP));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_URC_COLP));
    if (strResult == NULL)
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+COLP:", 0, iLength);
    if(iBegin == -1)    
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = iBegin + AK_CLIP_LEN_7;

    if(strResult[iBegin] != '\"')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iPos1 = Utl_StrFndChr(strResult, '\"', (_SHORT16)(iBegin + 1));
    if(iPos1 == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    if (((iPos1 - 1) - (iBegin + 1)) + 1 > G_MaxTeleLen)
        Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iBegin + 1 + G_MaxTeleLen - 1));
    else
        Utl_StrMid(strUrc->telenumber, strResult, (_SHORT16)(iBegin + 1), (_SHORT16)(iPos1 - 1));

    if(Utl_StrDigital(strUrc->telenumber) == EOS_FALSE)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iPos1 = iPos1 + 1;
    if(strResult[iPos1] != ',')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    /* Changed for GPHONE*/
    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_URC_COLP);
 
    return (_VOID*)strUrc;
}
/**
 * @brief: Parse USSD URC report
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *WaveC_WmPsUrc_USSD(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    iBegin, iEnd, iPos1, iPos2;
    RIL_USSD_URC *strUrc = NULL;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _CHAR8  strUSSDValue[256];
    _CHAR8 strTemp[256];
    _INT32 iDcs = -1;
    
    strUrc =  (RIL_USSD_URC*)WIRELESS_MALLOC(sizeof(RIL_USSD_URC));

    memset(strUSSDValue, 0, sizeof(strUSSDValue));
    /*clear first*/
    memset(strUrc, 0, sizeof(RIL_USSD_URC));
    memset(strTemp, 0, sizeof(strTemp));

    WIRELESS_INFO("WaveC_WmPsUrc_USSD, content(%d)(%d):%s\r\n", iLength, *strSize, strResult);
    if (strResult == NULL)   
    {  
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    iBegin = Utl_StrFndL(strResult, "+CUSD:", 0, iLength);
    if(iBegin == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        return NULL;
    }
    #if 0 /* to avoid there is return character in the ussd response    */
    iEnd = Utl_StrFndL(strResult, ATCMD_RETURN, (_SHORT16)(iBegin + 1), iLength);
    if(iEnd == -1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_FREE(strUrc);
        WIRELESS_INFO("WaveC_WmPsUrc_USSD:(%s)(%d)\r\n", __FILE__, __LINE__);
        return NULL;
    }
    #else
    iEnd = iLength;
    #endif

    iPos1 = iBegin + AK_CUSD_LEN_7;

    if(strResult[iPos1] > '2' || strResult[iPos1] < '0')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        WIRELESS_FREE(strUrc);
        return NULL;
    }

    strUrc->type = (_CHAR8)(strResult[iPos1] - '0');

    iPos1 = iPos1 + 1;
    if(iPos1 == iEnd)
    {
        strUrc->dcs = 0;
        strUrc->str = NULL;
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_USSD_URC);
        WIRELESS_TRACE;
        WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        return (_VOID*)strUrc;
    }

    if(strResult[iPos1] != ',')
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_INFO("WaveC_WmPsUrc_USSD:(%s)(%d)\r\n", __FILE__, __LINE__);
        WIRELESS_TRACE;
        WIRELESS_FREE(strUrc);
        return NULL;
    }
    
    if(strResult[ iPos1 + 1] != '\"')
    {  
        if(strResult[ iPos1 + 2] != '\"')
        {
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            WIRELESS_TRACE;
            WIRELESS_FREE(strUrc);
            return NULL;
        }
        else
        {
            iPos1 += 1;
        }
    }

    /* Kinson iPos2 = Utl_StrFndChr(strResult, ',', (_SHORT16)(iPos1 + 1));*/
    iPos2 = Utl_StrNFndChr_Reverse(strResult, iLength, ',');
    WIRELESS_INFO("WaveC_WmPsUrc_USSD:iPos2:%d, iPos1:%d, iLength:%d, iEnd:%d\r\n", iPos2, iPos1, iLength, iEnd);
    if((iPos2 == -1) || ((iPos2 + 1) > iEnd) || (iPos1 >= iPos2))
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
        WIRELESS_FREE(strUrc);
        WIRELESS_TRACE;
        return NULL;
    }
    
    strUrc->str = (_CHAR8 *)WIRELESS_MALLOC((_USHORT16)(iPos2 - iPos1) + 2);

    if(strResult[iPos2 - 1] != '\"')
    {  
        WIRELESS_FREE(strUrc->str);
        
        /*    Find Previous One    */
        iPos2 = Utl_StrNFndChr_Reverse(strResult, (iPos2 - 1), ',');
        WIRELESS_INFO("WaveC_WmPsUrc_USSD:iPos2:%d, iPos1:%d, iLength:%d\r\n", iPos2, iPos1, iLength);
        if((iPos2 == -1) || ((iPos2 + 1) > iEnd) || (iPos1 >= iPos2))
        {  
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_FREE(strUrc);
            WIRELESS_TRACE;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            return NULL;
        }

        strUrc->str = (_CHAR8 *)WIRELESS_MALLOC((_USHORT16)(iPos2 - iPos1) + 2);

        if(strResult[iPos2 - 1] != '\"')
        {  
            *result = RIL_prGERROR;
            *strNum = 0;
            *strSize = 0;
            WIRELESS_ERROR(WIRELESS_ERRORNO, "WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
            WIRELESS_FREE(strUrc->str);
            WIRELESS_TRACE;
            WIRELESS_FREE(strUrc);
            return NULL;
        }
    }

    Utl_StrMid_N(strUrc->str, (_USHORT16)((iPos2 - iPos1) + 2),strResult, (_SHORT16)(iPos1 + 2), (_SHORT16)(iPos2 - 2));

    Utl_StrMid(strTemp, strResult, (_SHORT16)(iPos2 + 1), (_SHORT16)(iEnd - 3));
    
    WIRELESS_INFO("1 strTemp:%s\r\n", strTemp);
    if(sscanf(strTemp, "%d", &iDcs) < 1)
    {  
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        WIRELESS_INFO("WaveC_WmPsUrc_USSD:(%s)(%d)\r\n", __FILE__, __LINE__);
        WIRELESS_FREE(strUrc->str);
        WIRELESS_FREE(strUrc);
        WIRELESS_TRACE;
        return NULL;
    }
    strUrc->dcs = iDcs;
        
    WIRELESS_INFO("2 strTemp:%s, Dcs:%d\r\n", strTemp, strUrc->dcs);

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(RIL_USSD_URC);
    WIRELESS_INFO("WaveC_WmPsUrc_USSD:%s(%s)(%d)\r\n", strResult, __FILE__, __LINE__);
    return (_VOID*)strUrc;
}


static _BOOL WaveC_WmPsStk_Refresh(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;
    
    _CHAR8 string[MAX_STK_STRING] = {0};
    
    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_Refresh;

    begin = Utl_StrFndL(str, AT_COMMA, 0, length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    end = _ril_SearchEnd(str, begin, length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string, str, begin, (_SHORT16)(end - 1), length);
    number = (_SHORT16)Utl_Atoi(string);
    if(number == 1)
        par->para.u_refresh.refMode = RIL_FileChange;
    else
        par->para.u_refresh.refMode = RIL_SIMInitAndFullFileChange;
    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SetupEventList(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16
    begin = -1,
    end = -1,
    number = -1,
    eventnum = 0;
    _CHAR8 string[10] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_EventList;

    /*get event list,currently not used*/
    begin = strlen(STGC_STRING_SETUPEVENTLIST);
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if ((-1 == end) || (begin == end))
    {
        return EOS_FALSE;
    }
    Utl_StrMidL(string , str, begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);
    if (number >= 0)
    {
        par->para.u_setup_event_list.eventNum++;
        eventnum = par->para.u_setup_event_list.eventNum;
        par->para.u_setup_event_list.eventList[eventnum] = (RIL_STK_EVENTLIST)number;
    }
    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SetupCall(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16    begin = -1;
    _SHORT16    end = -1;
    _SHORT16    number = -1;

    _CHAR8     string[MAX_STK_STRING] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SetupCall;

/*number(infineon): MMI may not use : the call party number*/
    begin = Utl_StrFndL(str, AT_COMMA, 0, length);
    if(-1 == begin)
        return EOS_TRUE;
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;
/*subaddr(infineon): MMI may not use: the call party subaddr*/
    begin = end + 1;
    end = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;
/*type(infineon): MMI may not use*/
    begin = end + 1;
    end = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;
/*alphaId*/
    begin = end + 1;
    end = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin+1) , (_SHORT16)(end -2) , length);
    ConvUCS2StrToUnicode(string , par->para.u_setup_call.alphaId);
/*may have dispMode*/
    par->para.u_setup_call.dispMode = RIL_DisplayIconOnly;

/*may have iconID*/
/*icon_id: may have "iconId" and "dispMode"*/
    begin = end;
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;/*end.*/

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number <= 0xFF) && (number >= 0))
        par->para.u_setup_call.iconId = (_UCHAR8)number;

    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SendSS(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;
    _CHAR8 string[MAX_STK_STRING] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SendSS;
    par->para.u_send_ss.iconId = 1;    /*init the var*/

/*ss_data: may no use*/
    begin = Utl_StrFndL(str , STK_STRING_SENDSS , 0 , length);
    
    begin += strlen(STK_STRING_SENDSS);

#if 0    
    /*begin = Utl_StrFndL(str , COMMA , begin , length);*/
    /*if(-1 == begin)*/
    /*    return EOS_FALSE;*/

    /*begin++;*/
    /*end = _ril_SearchEnd(str , begin , length);*/
    /*if((-1 == end) || (begin == end))*/
    /*    return EOS_FALSE;*/

    /*Utl_StrMidL(string , str , (_SHORT16)(begin + 1) , (_SHORT16)(end - 2) , length);*/
    /*ConvUCS2ToSTK(string , par->para.u_send_ss.alphaId);*/
#endif
/*alphaId*/
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        /*return EOS_FALSE;*/
        return EOS_TRUE;

    Utl_StrMidL(string , (_CHAR8*)str , (_SHORT16)(begin + 1) , (_SHORT16)(end - 2) , length);
    ConvUCS2StrToUnicode(string, par->para.u_send_ss.alphaId);
/*icon_id: may have ref_number*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if(-1 == end)
    {
        if(length > begin)
        {
            Utl_StrMidL(string , str , begin , (_SHORT16)(length - 1) , length);
            number = (_SHORT16)Utl_Atoi(string);

            if((number < 255) && (number >= 0))
                par->para.u_send_ss.iconId = (_UCHAR8)number;
        }
        return EOS_TRUE;/*end.*/
    }

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_send_ss.iconId = (_UCHAR8)number;
/*ref_number*/

    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if(-1 == end)
    {
        if(length > begin)
        {
            Utl_StrMidL(string , str , begin , (_SHORT16)(length - 1) , length);
            number = (_SHORT16)Utl_Atoi(string);

            if((number < 255) && (number >= 0))
                par->para.u_send_ss.iconId = (_UCHAR8)number;
        }
        return EOS_TRUE;/*end.*/
    }

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_send_ss.iconId = (_UCHAR8)number;

/*dispMode: no use*/
    par->para.u_send_ss.dispMode = RIL_DisplayIconOnly;

    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SendUSSD(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;
    _CHAR8 string[MAX_STK_STRING] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SendUSSD;

/*dcs*/
    begin = Utl_StrFndL(str, STK_STRING_SENDUSSD, 0, length);
    
    begin += strlen(STK_STRING_SENDUSSD);
    
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    
    end = Utl_StrFndL(str , AT_COMMA , begin , length);
    
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string, str, begin, (_SHORT16)(end - 1), length);
    number = (_SHORT16)Utl_Atoi(string);
    if ((number == 72) || (number == 15))
    {
        par->para.u_send_ussd.dcs = (_UCHAR8)number;
    }

/*hex string*/
    begin = end + 1;
    begin = Utl_StrFndL(str, "\"", begin, length);
    if (-1 == begin)
    {
        return EOS_FALSE;
    }
    
    begin++;
    end = Utl_StrFndL(str, "\"", begin, length);
    if (-1 == end) 
    {
        return EOS_FALSE;
    }
    
    Utl_StrMidL(string, (_CHAR8*)str, begin, (_SHORT16)(end - 1), length);

/*alphaId, may be null*/
    begin = end + 1;
    begin = Utl_StrFndL(str, "\"", begin, length);
    if (-1 == begin)
    {
        return EOS_FALSE;
    }
    
    begin++;
    end = Utl_StrFndL(str, "\"", begin, length);
    if (-1 == end)
    {
        return EOS_FALSE;
    }
    
    if(begin == end || begin == end - 1)
    {
        string[0]= '\0';   
    }
    else
    {
        Utl_StrMidL(string, str, begin, (_SHORT16)(end - 1), length);
        
    }
    
    ConvUCS2StrToUnicode((_CHAR8*)string, par->para.u_send_ussd.alphaId);

/*icon_id*/
    begin = end + 1;
    begin = Utl_StrFndL(str, AT_COMMA, begin, length);
    if (-1 == begin)
    {
        return EOS_FALSE;
    }

    begin++;
    end = Utl_StrFndL(str, AT_COMMA, begin, length);
    if (-1 == end)
    {
        return EOS_FALSE;
        
    }
    
    Utl_StrMidL(string, str, begin, (_SHORT16)(end - 1), length);
    number = (_SHORT16) Utl_Atoi(string);
    par->para.u_send_ussd.iconId = (_UCHAR8)number;

/*reference number*/
    begin = end + 1;
    end = _ril_SearchEnd(str, begin, length);
    if ((-1 == end) || (begin == end))
    {
        return EOS_FALSE;
    }
    
    Utl_StrMidL(string, str, begin, (_SHORT16)(end - 1), length);
    number = (_SHORT16) Utl_Atoi(string);
    par->para.u_send_ussd.refNum = (_UCHAR8)number;

/*dispMode: no use*/
    par->para.u_send_ussd.dispMode = RIL_DisplayIconOnly;

    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SendShortMsg(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;
    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SendSM;
    par->para.u_send_sm.iconId = 1;

/*alphaId*/
    begin = Utl_StrFndL(str , STK_STRING_SENDSM , 0 , length);
    
    begin += strlen(STK_STRING_SENDSM);
    
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;/*end.*/

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
    {
        string[0]= '\0';
    }   
    else
    {
        Utl_StrMidL(string , (_CHAR8*)str , (_SHORT16)(begin +1) , (_SHORT16)(end - 2) , length); 
    }
    ConvUCS2StrToUnicode(string, par->para.u_send_sm.alphaId);

    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;/*not found iconId end*/

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_send_sm.iconId = (_UCHAR8)number;
    
/*ref_number: not use may be the simcom's display mode*/
    par->para.u_send_sm.dispMode = RIL_DisplayIconOnly;
    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SendDTMF(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16    begin = -1;
    _SHORT16    end = -1;
    _SHORT16    number = -1;
/*    _SHORT16    temp = -1;*/

    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SendDTMF;

/*alphaId*/
    begin = Utl_StrFndL(str , STGC_STRING_SENDDTMF , 0 , length);
    begin += strlen(STK_STRING_COMMAND);
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;/*end.*/

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin + 1) , (_SHORT16)(end - 2) , length);
    ConvUCS2StrToUnicode(string , par->para.u_send_dtmf.alphaId);
/*may have "iconId" and "dispMode"*/
    begin = end;
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;/*end.*/

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;/*end.*/

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_send_dtmf.iconId = (_UCHAR8)number;

/*ref_number: not use may be the simcom's display mode*/
    par->para.u_send_dtmf.dispMode = RIL_DisplayIconOnly;
    
    return EOS_TRUE;
}

#if 0
static _BOOL WaveC_WmPsStk_LaunchBrowser(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16    begin = -1;
    _SHORT16    end = -1;
    _SHORT16    number = -1;

    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_LaunchBrowser;
/*url*/
    begin = Utl_StrFndL(str , STGC_STRING_LAUNCHBROWSER , 0 , length);
    begin = Utl_StrFndL(str , AT_COMMA, begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL((_CHAR8*)(par->para.u_launch_browser.url) , str , begin , (_SHORT16)(end -1) , length);
/*alphaId*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin + 1) , (_SHORT16)(end - 2) , length);
    ConvUCS2StrToUnicode(string , par->para.u_launch_browser.alphaId);
/*may have "iconId" and "dispMode"*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_launch_browser.iconId = (_UCHAR8)number;

    return EOS_TRUE;
}
#endif

static _BOOL WaveC_WmPsStk_PlayTone(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _INT32 number = -1;

    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_PlayTone;

/*tone: may have*/
    begin = Utl_StrFndL(str , STK_STRING_PLAYTONE , 0 , length);
    
    begin += strlen(STK_STRING_PLAYTONE);
    
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = Utl_Atoi(string);

    if (RIL_Dial                    == number    
        || RIL_CalledSubscriberBusy    == number    
        || RIL_Congestion            == number
        || RIL_RadioPathAcknowledge    == number    
        || RIL_CallDropped            == number    
        || RIL_Error                == number    
        || RIL_CallWaiting            == number    
        || RIL_RingingTone            == number    
        || RIL_GeneralBeep            == number    
        || RIL_PositiveAck            == number    
        || RIL_NegativeAck            == number)
    {
        par->para.u_play_tone.tone = (RIL_STK_TONE)number;
    }
    else /*If no tone is specified,set it to default value "general beep"*/
    {
        par->para.u_play_tone.tone = RIL_GeneralBeep;
    }

/*duration: infineon module not this return,but the value=unit * interval*/
/*unit*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);
    par->para.u_play_tone.duration = (_UINT32)(number);
/*interval*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = Utl_Atoi(string);
    par->para.u_play_tone.duration *= (_UINT32)number;
/*alphaId*/
    begin = Utl_StrFndL(str, "\"", begin, length);
    if(-1 == begin)
        return EOS_FALSE;
    begin++;
    end = Utl_StrFndL(str, "\"", begin, length);
    if(-1 == end)
        return EOS_FALSE;
    if(begin == end || begin == end - 1)   /*alphaId is null*/
    {
        string[0]= '\0';
    }   
    else
    {
        Utl_StrMidL(string , str , (_SHORT16)begin, (_SHORT16)(end - 1) , length);   
    }
    
    ConvUCS2StrToUnicode(string, par->para.u_play_tone.alphaId);
    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_DisplayText(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;    
    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_DisplayText;

/*type: not use infineon_lyb*/
    begin = Utl_StrFndL(str , STK_STRING_DISPLAYTEXT , 0 , length);
    begin = Utl_StrFndL(str , AT_COMMA, begin , length);
    if(-1 == begin)
        return EOS_FALSE;
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

/*dcs*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if(RIL_GSM7bit == number || RIL_Data8bit == number || RIL_UCS2 == number)
    {
        par->para.u_display_text.dcs = (RIL_STK_ALPHABET)number;
    }
    else 
    {
        par->para.u_display_text.dcs = RIL_Data8bit;
    }

/*text*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin+1) , (_SHORT16)(end - 2) , length);

    switch(par->para.u_display_text.dcs)
    {
        case RIL_GSM7bit:/*wait for later*/
            break;
        case RIL_Data8bit:
            Conv8BitDataToUnicode(string, (_USHORT16*)(par->para.u_display_text.text));/*wait for later*/
            break;
        case RIL_UCS2:
            ConvUCS2StrToUnicode(string, (_USHORT16*)(par->para.u_display_text.text));
            break;
        default:
            return EOS_FALSE;
    }

/*icon_id: may have "iconId"*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;    /*its end,the may have isn't be must,so it return AK_TURE*/

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_display_text.iconId = (_UCHAR8)number;
    else return EOS_TRUE;    /*its end,the may have isn't be must,so it return AK_TURE*/

/*imm_resp: may have*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;    /*its end,the may have isn't be must,so it return AK_TURE*/

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if(RIL_ImmediateResponse == number)
        par->para.u_display_text.response = RIL_ImmediateResponse;
    else
        par->para.u_display_text.response = RIL_NormalReponse;
    /*its end,the may have isn't be must,so it return AK_TURE*/

    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_GetInkey(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;
/*    _SHORT16 temp = -1;*/
    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_GetInkey;

/*type: infineon_lyb means the response and helpInformation and echo*/
    begin = Utl_StrFndL(str , STK_STRING_GETINKEY , 0 , length);
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    /*parse the bit function
    bit 1: 0 = digits only, 1 = alphabet set
    bit 2: 0 = sms default alphabet , 1 = UCS2 alphabet if supported
    bit 3: 0 = MI may echo user input,1 = hide user input(display ****)
    bit 4: 0 = user input to be in unpacked format, 1 = user input to be in sms packed format
    bit 5---7: not used
    bit 8: 0 = no help information available, 1 = help information available
    type: response*/
    if(0x01 == (number & 0x01))
    {/*bit 1: not digits only ; node : the ikYesNo not use*/
        if(0x02 == (number & 0x02))
        {
            number = RIL_ikUCS2;
        }
        else
        {
            number = RIL_ikSMS;
        }
    }
    else
    {
        number = RIL_ikDigits;
    }

    if(
        RIL_ikDigits == number    ||
        RIL_ikSMS == number        ||
        RIL_ikUCS2 == number    ||
        RIL_ikYesNo == number)
        par->para.u_get_inkey.dispMode = (RIL_STK_DISPLAY)number;
    else 
        return EOS_FALSE;

    /*type: helpinfo*/
    if(0x80 == (number & 0x80))
    {/*bit 8: 1 = help information available*/
        par->para.u_get_inkey.helpInfo = RIL_HelpInfo;
    }
    else
    {/*bit 8: 0 = no help information available*/
        par->para.u_get_inkey.helpInfo = RIL_NoHelpInfo;
    }

/*dcs*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if(
        RIL_GSM7bit == number ||
        RIL_Data8bit == number ||
        RIL_UCS2 == number)
        par->para.u_get_inkey.dcs = (RIL_STK_ALPHABET)number;
    else
        par->para.u_get_inkey.dcs = RIL_Data8bit;
    /*else return EOS_FALSE;*/

/*hex_string*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin +1), (_SHORT16)(end - 2) , length);

    switch(par->para.u_get_inkey.dcs)
    {
    case RIL_GSM7bit:/*wait for later*/
        break;
    case RIL_Data8bit:
        Conv8BitDataToUnicode(string, (_USHORT16*)par->para.u_get_inkey.text);/*wait for later*/
        break;
    case RIL_UCS2:    
        ConvUCS2StrToUnicode(string, par->para.u_get_inkey.text);
        break;
    default:
        return EOS_FALSE;
    }
    
/*icon_id: may have "iconId"*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;    /*its end,the may have isn't be must,so it return AK_TURE*/

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_get_inkey.iconId = (_UCHAR8)number;
/*dispMode mode may not use*/
    par->para.u_get_inkey.dispMode = RIL_DisplayIconOnly;

    return EOS_TRUE;    /*its end,the may have isn't be must,so it return AK_TURE*/
}

static _BOOL WaveC_WmPsStk_GetInput(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;

    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_GetInput;

/*type: infineon_lyb means the response and helpInformation and echo*/
    begin = Utl_StrFndL(str , STK_STRING_GETINPUT , 0 , length);
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;
    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    /*parse the bit function
    bit 1: 0 = digits only, 1 = alphabet set
    bit 2: 0 = sms default alphabet , 1 = UCS2 alphabet if supported
    bit 3: 0 = MI may echo user input,1 = hide user input(display ****)
    bit 4: 0 = user input to be in unpacked format, 1 = user input to be in sms packed format
    bit 5---7: not used
    bit 8: 0 = no help information available, 1 = help information available
    type: response    
    */
    if(0x01 == (number & 0x01))
    {/*bit 1: not digits only*/
        par->para.u_get_input.response = RIL_SMSUnpacked;
        if(0x02 == (number & 0x02))
        {/*bit 2: UCS2*/
            if(0x08 == (number & 0x08))
            {/*bit 4: sms packed*/
                par->para.u_get_input.response = RIL_SMSPackedUCS2;
            }
            else
            {/*bit 4: unpacked*/
                par->para.u_get_input.response = RIL_SMSUnpackedUCS2;
            }    
        }
        else
        {/*bit 2: alphabet*/
            if(0x08 == (number & 0x08))
            {/*bit 4: sms packed*/
                par->para.u_get_input.response = RIL_SMSPacked;
            }
            else
            {/*bit 4: unpacked*/
                par->para.u_get_input.response = RIL_SMSUnpacked;
            }
        }
    }
    else
    {
        par->para.u_get_input.response = RIL_DigitsFromSMSUnpacked;
        if(0x02 == (number & 0x02))
        {/*bit 2: UCS2*/
            if(0x08 == (number & 0x08))
            {/*bit 4: sms packed*/
                par->para.u_get_input.response = RIL_DigitsFromUCS2;
            }
            else
            {/*bit 4: unpacked*/
                par->para.u_get_input.response = RIL_DigitsFromUCS2;
            }    
        }
        else
        {/*bit 2: alphabet*/
            if(0x08 == (number & 0x08))
            {/*bit 4: sms packed*/
                par->para.u_get_input.response = RIL_DigitsFromSMSpacked;
            }
            else
            {/*bit 4: unpacked*/
                par->para.u_get_input.response = RIL_DigitsFromSMSUnpacked;
            }
        }
    }
    /*type: echo*/
    if(0x04 == (number & 0x04))
    {/*bit 3: 1 = hide user input(display ****)*/
        par->para.u_get_input.echo = RIL_NoECHO;
    }
    else
    {/*bit 3: 0 = MI may echo user input*/
        par->para.u_get_input.echo = RIL_ECHO;
    }
    /*type: helpinfo*/
    if(0x80 == (number & 0x80))
    {/*bit 8: 1 = help information available*/
        par->para.u_get_input.helpInfo = RIL_HelpInfo;
    }
    else
    {/*bit 8: 0 = no help information available*/
        par->para.u_get_input.helpInfo = RIL_NoHelpInfo;
    }

    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if(
        RIL_GSM7bit == number ||
        RIL_Data8bit == number ||
        RIL_UCS2 == number)
        par->para.u_get_input.dcs = (RIL_STK_ALPHABET)number;
    else
        par->para.u_get_input.dcs = RIL_Data8bit;
    /*else return EOS_FALSE;*/

/*hex_string*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin +1), (_SHORT16)(end - 2) , length);

    switch(par->para.u_get_input.dcs)
    {
    case RIL_GSM7bit:/*wait for later*/
        break;
    case RIL_Data8bit:
        Conv8BitDataToUnicode(string, (_USHORT16*)par->para.u_get_input.text);/*wait for later*/
        break;
    case RIL_UCS2:    
        ConvUCS2StrToUnicode(string, par->para.u_get_input.text);
        break;
    default:
        return EOS_FALSE;
    }

    /*maxLength*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number >=0) && (number <= 255))
        par->para.u_get_input.maxLength = (_UCHAR8)number;
    else return EOS_FALSE;
    
/*minLength*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number >=0) && (number <= 255))
        par->para.u_get_input.minLength = (_UCHAR8)number;
    else return EOS_FALSE;

/*default dcs*/
    par->para.u_get_input.defaultDcs = par->para.u_get_input.dcs;
/*default text*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin + 1) , (_SHORT16)(end - 2) , length);

    switch(par->para.u_get_input.defaultDcs)
    {
    case RIL_GSM7bit:/*wait for later*/
        break;
    case RIL_Data8bit:
        Conv8BitDataToUnicode(string, (_USHORT16*)par->para.u_get_input.defaultText);/*wait for later*/
        break;
    case RIL_UCS2:    
        ConvUCS2StrToUnicode(string, par->para.u_get_input.defaultText);
        break;
    default:
        return EOS_FALSE;
    }

/*icon_id: may have "iconId" and "dispMode"*/
    begin = end;
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;/*end.*/

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_get_input.iconId = (_UCHAR8)number;

    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SelectItem(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16    i = 0;
    _SHORT16    begin = -1;
    _SHORT16    end = -1;
    _SHORT16    number = -1;

    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SelectItem;

/*alphaId*/
    begin = Utl_StrFndL(str , STK_STRING_SELECTITEM , 0 , length);
    begin = begin + strlen(STK_STRING_SELECTITEM);

    begin += 2;
    begin = Utl_StrFndL(str , "\"" , begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    end = Utl_StrFndL(str , "\"" , begin , length);
    /*infineon_lyb : temp for if the alphaid is null,it must point to MAIN MENU item text*/
    /*if((-1 == end) || (begin == end))*/
    /*    return EOS_FALSE;*/
    if(-1 == end)
        return EOS_FALSE;
    if(begin == end || begin == end - 1)
    {
        par->para.u_select_item.alphaId[0] = 0;
        par->para.u_select_item.alphaId[1] = 0;
    }
    else
    {
        /*infineon has got a bug at here,an unwanted byte is in the front of alphaId    */
        Utl_StrMidL(string , str , begin , (_SHORT16)(end -1) , length);
        ConvUCS2StrToUnicode(string, par->para.u_select_item.alphaId);
    }
    /*end temp*/

    /*item number*/
    begin = end + 2;
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    end = Utl_StrFndL(str , AT_COMMA , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < MAX_MENU_ITEM) && (number >= 0))
        par->para.u_select_item.numItems = (_UCHAR8)number;
    else
        return EOS_FALSE;

    /*selection*/
    par->para.u_select_item.selection = RIL_SoftKeySelection;

    /*helpinfo*/
    par->para.u_select_item.helpInfo = RIL_NoHelpInfo;

    /*items*/
    end = 0;
    for(i = 0; i < par->para.u_select_item.numItems; i++)
    {
        /*position begin to the begin of next line*/
        begin = end;
        begin = Utl_StrFndL(str , STK_STRING_SELECTITEM , begin , length);        

        /*item id*/
        begin += strlen(STK_STRING_SELECTITEM);
        /*begin++;*/
        begin += 3;
        begin = Utl_StrFndL(str , AT_COMMA , begin , length);
        if(-1 == begin)
            return EOS_FALSE;

        begin++;
        end = Utl_StrFndL(str , AT_COMMA , begin , length);
        if((-1 == end) || (begin == end))
            return EOS_FALSE;

        Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
        number = (_SHORT16)Utl_Atoi(string);
        if((number >= 0)&&(number <= 0xFF))
            par->para.u_select_item.items[i].itemId = (_UCHAR8)number;
        else return EOS_FALSE;

        /*itemText*/
        begin = end + 1;
        begin = Utl_StrFndL(str , "\"" , begin , length);
        if(-1 == begin)
            return EOS_FALSE;

        begin++;
        end = Utl_StrFndL(str , "\"" , begin , length);
        if(-1 == end)
            return EOS_FALSE;

        Utl_StrMidL(string , (_CHAR8*)str , begin , (_SHORT16)(end-1) , length);
        ConvUCS2StrToUnicode(string , par->para.u_select_item.items[i].itemText);
        end = _ril_SearchEnd(str , end , length);
    }
    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SetupMenu(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16 i = 0;
    _SHORT16 begin = -1;
    _SHORT16 end = -1;
    _SHORT16 number = -1;
    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SetupMenu;

/*alphaId*/
    begin = Utl_StrFndL(str , STK_STRING_SETUPMENU , 0 , length);
    begin = begin + strlen(STK_STRING_SETUPMENU);
    begin = Utl_StrFndL(str , "\"" , begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin ++;
    end = Utl_StrFndL(str , "\"" , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    /*infineon has got a bug at here,an unwanted byte is in the front of alphaId*/    
    Utl_StrMidL(string , str , begin , (_SHORT16)(end -1) , length);
    ConvUCS2StrToUnicode(string , par->para.u_setup_menu.alphaId);    

/*item number*/
    begin = end + 2;
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    end = Utl_StrFndL(str , AT_COMMA , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < MAX_MENU_ITEM) && (number >= 0))
        par->para.u_setup_menu.numItems = (_UCHAR8)number;
    else
        return EOS_FALSE;

/*items*/
    end = 0;
    for(i = 0; i < par->para.u_setup_menu.numItems; i++)
    {
        /*position begin to the begin of next line*/
        begin = end;
        begin = Utl_StrFndL(str , STK_STRING_SETUPMENU , begin , length);        

        
        begin += strlen(STK_STRING_SETUPMENU);
        /*type*/
        begin = Utl_StrFndL(str , AT_COMMA , begin , length);
        if(-1 == begin)
            return EOS_FALSE;
        
        begin++;
        end = Utl_StrFndL(str , AT_COMMA , begin , length);
        if((-1 == end) || (begin == end))
            return EOS_FALSE;

        /*item id*/
        begin = end + 1;
        begin = Utl_StrFndL(str , AT_COMMA , begin , length);
        if(-1 == begin)
            return EOS_FALSE;

        begin++;
        end = Utl_StrFndL(str , AT_COMMA , begin , length);
        if((-1 == end) || (begin == end))
            return EOS_FALSE;

        Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
        number = (_SHORT16)Utl_Atoi(string);
        /*infineon_lyb : for deal with the number may has 209~221 and 226*/
        /*if((number >= 0)&&(number <= par->para.u_setup_menu.numItems))*/
        /*    par->para.u_setup_menu.items[i].itemId = (_UCHAR8)number;*/
        if((number >= 0)&&(number <= 0xFF))
            par->para.u_setup_menu.items[i].itemId = (_UCHAR8)number;
        /*end*/
        else return EOS_FALSE;

/*itemText*/
        begin = end + 1;
        begin = Utl_StrFndL(str , "\"" , begin , length);
        if(-1 == begin)
            return EOS_FALSE;

        begin++;
        end = Utl_StrFndL(str , "\"" , begin , length);
        if((-1 == end) || (begin == end))
            return EOS_FALSE;

        Utl_StrMidL(string , str , begin , (_SHORT16)(end-1) , length);
        ConvUCS2StrToUnicode(string , par->para.u_setup_menu.items[i].itemText);
        end = _ril_SearchEnd(str , end , length);
    }
    return EOS_TRUE;
}

static _BOOL WaveC_WmPsStk_SetupIdleModeText(const _CHAR8* str, _USHORT16 length, RIL_STK_PAR* par)
{
    _SHORT16    begin = -1;
    _SHORT16    end = -1;
    _SHORT16    number = -1;

    _CHAR8 string[1024] = {0};

    /*commandId*/
    par->cmdId = RIL_NOTIFY_STK_SetupIdleModeText;

    begin = Utl_StrFndL(str , STK_STRING_SETUPIDLEMODETEXT , 0 , length);
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_FALSE;

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if(
        RIL_GSM7bit == number ||
        RIL_Data8bit == number ||
        RIL_UCS2 == number)
        par->para.u_setup_idle_text.dcs = (RIL_STK_ALPHABET)number;
    else
        par->para.u_setup_idle_text.dcs = RIL_Data8bit;
    /*else return EOS_FALSE;*/

/*hex_string*/
    begin = end + 1;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_FALSE;

    Utl_StrMidL(string , str , (_SHORT16)(begin + 1) , (_SHORT16)(end - 2) , length);

    switch(par->para.u_setup_idle_text.dcs)
    {
    case RIL_GSM7bit:/*wait for later*/
        break;
    case RIL_Data8bit:
        Conv8BitDataToUnicode(string, (_USHORT16*)par->para.u_setup_idle_text.text);/*wait for later*/break;
    case RIL_UCS2:    
        ConvUCS2StrToUnicode(string , par->para.u_setup_idle_text.text);
        break;
    default:
        return EOS_FALSE;
    }

/*may have "iconId"*/
    begin = end;
    begin = Utl_StrFndL(str , AT_COMMA , begin , length);
    if(-1 == begin)
        return EOS_TRUE;/*end.*/

    begin++;
    end = _ril_SearchEnd(str , begin , length);
    if((-1 == end) || (begin == end))
        return EOS_TRUE;/*end.*/

    Utl_StrMidL(string , str , begin , (_SHORT16)(end - 1) , length);
    number = (_SHORT16)Utl_Atoi(string);

    if((number < 255) && (number >= 0))
        par->para.u_setup_idle_text.iconId = (_UCHAR8)number;

/*may have "iconId"*/
    par->para.u_setup_idle_text.dispMode = RIL_DisplayIconOnly;

    return EOS_TRUE;
}

/**
 * @brief: Parse STK URC
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*:parse data
 * @retval
 *
 */
_VOID *WaveC_WmPsUrc_STK(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16 iBegin;
    RIL_STK_PAR *stk = (RIL_STK_PAR *)WIRELESS_MALLOC(sizeof(RIL_STK_PAR));
    _BOOL rtn = EOS_FALSE;
    _USHORT16   length = (_USHORT16)strlen(strResult);

    if((iBegin = Utl_StrFndL(strResult, STK_STRING_STKCNF, 0, length)) != -1)
    {
        if(Utl_StrFndL(strResult, "255,144\r", iBegin, length) != -1)
        {
            stk->cmdId = RIL_NOTIFY_STK_EndProactive;
            *result = RIL_prOK;
            *strNum = 1;
            *strSize = sizeof(RIL_STK_PAR);
            return stk;
        }
        WIRELESS_TRACE;
        WIRELESS_FREE(stk);
        *result = RIL_prNULL;
        return NULL;
    }
    
    if(Utl_StrFndL(strResult, STK_PROACTIVE_END, 0, length) != -1)
    {
        stk->cmdId = RIL_NOTIFY_STK_EndProactive;
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_STK_PAR);
        return stk;
    }
    
    if(Utl_StrFndL(strResult,STK_STRING_REFRESH, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_Refresh(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STGC_STRING_SETUPEVENTLIST, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SetupEventList(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SETUPCALL, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SetupCall(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SENDSS, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SendSS(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SENDUSSD, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SendUSSD(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SENDSM, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SendShortMsg(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STGC_STRING_SENDDTMF, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SendDTMF(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STGC_STRING_LAUNCHBROWSER, 0, length) != -1)
    {
    /*Kinson
        rtn = WaveC_WmPsStk_LaunchBrowser(strResult, length, stk);
        */
    }
    else if(Utl_StrFndL(strResult,STK_STRING_PLAYTONE, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_PlayTone(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_DISPLAYTEXT, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_DisplayText(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_GETINKEY, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_GetInkey(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_GETINPUT, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_GetInput(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SELECTITEM, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SelectItem(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SETUPMENU, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SetupMenu(strResult, length, stk);
    }
    else if(Utl_StrFndL(strResult,STK_STRING_SETUPIDLEMODETEXT, 0, length) != -1)
    {
        rtn = WaveC_WmPsStk_SetupIdleModeText(strResult, length, stk);
    }

    if(rtn)
    {
        *result = RIL_prOK;
        *strNum = 1;
        *strSize = sizeof(RIL_STK_PAR);
        return stk;
    }
    else
    {
        WIRELESS_TRACE;
        WIRELESS_FREE(stk);
        *result = RIL_prNULL;
        return NULL;
    }
}


/**
 * @brief: parse signal strength URC
 * @param _CHAR8* strResult: received AT result
 * @param  RIL_PARSE_RES* result:AT parse result
 * @param  _UINT32 * strNum:number of data structure 
 * @param  _UINT32 *strSize:szie of data structure 
 * @retvalue _VOID*: parse data
 **/
_VOID* WaveC_WmPsExt_SignalStrengthURC(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize)
{
    _SHORT16    start, end;
      _CHAR8    strTemp[16];
    _INT32   *signal = NULL;
    _INT32    value;
    _USHORT16   iLength = (_USHORT16)strlen(strResult);
    _INT32 tabRxLevToSignalDsp[3][4] = { 
        {3,5,9,12},        /*good signal strength display for customer*/
        {4,8,13,18},    /*normal signel strength display for customer*/
        {9,15,20,25} }; /*weak signal strength display for customer*/

    if (strResult == NULL)
    {
        *result = RIL_prNULL;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start = Utl_StrFndL(strResult, "+XCIEV:", 0, iLength);
    if (start == -1)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start += 8;
    end   = Utl_StrFndL(strResult, ",", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }

    start += 2;
    end = Utl_StrFndL(strResult, "\r", start, iLength);
    if (end <= start)
    {
        *result = RIL_prGERROR;
        *strNum = 0;
        *strSize = 0;
        return NULL;
    }
    Utl_StrMid(strTemp, strResult, start, (_SHORT16)(end - 1));
    signal = (_INT32*)WIRELESS_MALLOC(sizeof(_INT32));

    value = Utl_Atoi(strTemp);
    if (value == 99)
        *signal = 0;
    else if (value <= tabRxLevToSignalDsp[1][0])
        *signal = 0;
    else if (value <= tabRxLevToSignalDsp[1][1])
        *signal = 1;
    else if (value <= tabRxLevToSignalDsp[1][2])
        *signal = 2;
    else if (value <= tabRxLevToSignalDsp[1][3])    
        *signal = 3;
    else
        *signal = 4;    

    *result = RIL_prOK;
    *strNum = 1;
    *strSize = sizeof(_INT32);
    return signal;
}


/**
 * @brief: Parse time zone URC report
 * @param[out]  RIL_PARSE_RES* result:AT parse result
 * @param[in] const _CHAR8* strResult: received AT result
 * @retvalue _VOID*: point to parse strTime
 *
 */
_VOID* WaveC_WmPsUrc_TimeZoneReporting(RIL_PARSE_RES* result, const _CHAR8* strResult)
{
    _SHORT16     iBegin, iEnd, iLength;
    _CHAR8    iyear[8], imon[8], iday[8];
    _CHAR8    ihour[8], imin[8], isec[8];
    _SHORT16    year, mon, day, hour, min, sec;
    _CHAR8    iTemp[16];
    _SHORT16    timeZone;
    RIL_SYSTIME        *strTime;
    
    *result = RIL_prNULL;

    if (strResult == NULL)
        return NULL;

    *result = RIL_prERROR;

    iLength =(_SHORT16) strlen((_CHAR8*)strResult);
    iBegin = Utl_StrFndL(strResult, "+CTZV:", 0, iLength);
    if(iBegin == -1)
    {
        return NULL;
    }

    iBegin = iBegin + AK_CTZV_LEN_7;
    iEnd = Utl_StrFndL(strResult, AT_COMMA, iBegin, iLength);
    if (iEnd == -1)
    {
        return NULL;
    }

    Utl_StrMid(iTemp, strResult, iBegin, (_SHORT16)(iEnd-1));
    timeZone = (_SHORT16)(Utl_Atoi(iTemp) / 4);

    iBegin = iEnd + 1;
    /* Start checking on date format.*/
    /* Find first quotation mark in beginning of date. Else, return error. */
    if(strResult[ iBegin ] != '\"')
    {
        return NULL;
    }
    
    iBegin++;
    Utl_StrMidL(iyear, strResult, iBegin, (_SHORT16)(iBegin+1), iLength);

    /* Verify year data are valid digits. */
    if ((iyear[0] < '0') || (iyear[0] > '9') || (iyear[1] < '0') || (iyear[1] > '9'))
        return NULL;

    /* Convert year data from string to integer. */
    year = (_SHORT16)Utl_Atoi(iyear);
    year += INITIALYEAR;

    iBegin += 2;
    if(strResult[ iBegin++ ] != '/')
    {
        return NULL;
    }

    /* Check validity of month.*/
    Utl_StrMidL(imon, strResult, iBegin, (_SHORT16)(iBegin+1), iLength);

    /* Verify month data are valid digits. */
    if ((imon[0] < '0') || (imon[0] > '9') || (imon[1] < '0') || (imon[1] > '9'))
        return NULL;

    /* Convert month data from string to integer. Verify valid month. */
    mon = (_SHORT16)Utl_Atoi(imon);
    if (mon<1 || mon>12)
        return NULL;

    iBegin += 2;
    if(strResult[ iBegin++ ] != '/')
    {
        return NULL;
    }

    /* Check validity of day.*/
    Utl_StrMidL(iday, strResult, iBegin, (_SHORT16)(iBegin+1), iLength);
    /* Verify day data are valid digits. */
    if ((iday[0] < '0') || (iday[0] > '9') || (iday[1] < '0') || (iday[1] > '9'))
        return NULL;

    /* Convert day data from string to integer. Verify valid day of month. */
    day = (_SHORT16)Utl_Atoi(iday);
    if (day<1 || day> 31)
        return NULL;

    iBegin += 2;
    if(strResult[ iBegin++ ] != ',')
    {
        return NULL;
    }

    /* Check validity of hour.*/
    Utl_StrMidL(ihour, strResult, iBegin, (_SHORT16)(iBegin+1), iLength);
    /* Verify hour data are valid digits. */
    if ((ihour[0] < '0') || (ihour[0] > '9') || (ihour[1] < '0') || (ihour[1] > '9'))
        return NULL;
    /* Convert hour data from string to integer. Verify valid hour. */
    hour = (_SHORT16)Utl_Atoi(ihour);
    if (hour < 0 || hour > 23)
        return NULL;

    iBegin += 2;
    if(strResult[ iBegin++ ] != ':')
    {
        return NULL;
    }

    /* Check validity of minute.*/
    Utl_StrMidL(imin, strResult, iBegin, (_SHORT16)(iBegin+1), iLength);
    /* Verify minute data are valid digits. */
    if ((imin[0] < '0') || (imin[0] > '9') || (imin[1] < '0') || (imin[1] > '9'))
        return NULL;
    /* Convert minute data from string to integer. Verify valid minute. */
    min = (_SHORT16)Utl_Atoi(imin);
    if (min < 0 || min > 59)
        return NULL;

    iBegin += 2;
    if(strResult[ iBegin++ ] != ':')
    {
        return NULL;
    }

    /* Check validity of second.*/
    Utl_StrMidL(isec, strResult, iBegin, (_SHORT16)(iBegin+1), iLength);

    /* Verify second data are valid digits. */
    if ((isec[0] < '0') || (isec[0] > '9') || (isec[1] < '0') || (isec[1] > '9'))
        return NULL;

    /* Convert second data from string to integer. Verify valid second. */
    sec = (_SHORT16)Utl_Atoi(isec);
    if (sec < 0 || sec > 59)
        return NULL;

    strTime =  (RIL_SYSTIME*)WIRELESS_MALLOC(sizeof(RIL_SYSTIME));
    if(NULL == strTime)
        return NULL;
    /* If all formats are valid, store into data. Return OK. */
    strTime->year = (_USHORT16)year;
    strTime->month = (_UCHAR8)mon;
    strTime->day = (_UCHAR8)day;
    strTime->hour = (_UCHAR8)hour;
    strTime->minute = (_UCHAR8)min;
    strTime->second = (_UCHAR8)sec;

    Time_AddHours(strTime, timeZone);
    *result = RIL_prOK;

    return strTime;
}

