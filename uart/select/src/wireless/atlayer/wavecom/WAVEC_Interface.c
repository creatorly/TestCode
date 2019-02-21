
#include "eos.h"
#include "WAVEC_Interface.h"
#include "wm_channel.h"
#include "WAVEC_at_pack.h"
#include "WAVEC_at_parse.h"

#include "../wireless_common.h"
#include "../../wireless_debug.h"
#include "../wireless_interface_inc.h"
#include "wireless_string.h"
#include "../wireless_app.h"

extern WIRELESSROOTST    stWirelessRootSt;
extern _BOOL FilterShortLine;

/** 短信的发送,存储. 需要两条AT命令来完成,这两条AT命令需要在同一个通道上发送,用该全局变量保存前一条AT发送的通道.*/
static _USHORT16 s_MsgChID = CHID_PPP_WAVEC;
/** 在哪条通道上执行了STK的download profile操作, 后续的STK操作都需要在该通道上完成.*/
static _USHORT16 s_STKChID = RIL_CHANNEL_ANY;
static _USHORT16 s_CallChID = CHID_AT_WAVEC;
static _USHORT16 s_DataConnChID = CHID_PPP_WAVEC;
static _USHORT16 s_LongTimeCmdChID = CHID_PPP_WAVEC;

/**
  *@brief: start to call a number
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom
  *@param    _CHAR8* strNumber    [in] call number
  *@param    RIL_DATA_CLASS eClass    [in] the data class

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_DialCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                        _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    _UCHAR8 cmd[DAIL_CALL_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];

    pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pURC, 0, (sizeof(T_URCRESULT)));
    
    pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pResult, 0, (sizeof(T_URCRESULT)));


    pURC->count = 1;
    strcpy(pURC->info[0], "NO CARRIER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO CARRIER");

    WaveC_WmAtV25_DialCall((_CHAR8*)cmd, &size, strNumber, eClass);

    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseDataConnect,
                                ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1,
                                EXE_NORMAL, 2, EOS_FALSE, pURC, pResult);
}

/**
  *@brief: start to call a number
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom
  *@param    _CHAR8* strNumber    [in] call number
  *@param    RIL_DATA_CLASS eClass    [in] the data class

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 U10_DialCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                        _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    _UCHAR8 cmd[DAIL_CALL_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];

    pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pURC, 0, (sizeof(T_URCRESULT)));
    
    pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pResult, 0, (sizeof(T_URCRESULT)));


    pURC->count = 1;
    strcpy(pURC->info[0], "NO ANSWER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO ANSWER");

    WaveC_WmAtV25_DialCall((_CHAR8*)cmd, &size, strNumber, eClass);

    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseDataConnect,
                                ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1,
                                EXE_NORMAL, 10, EOS_FALSE, pURC, pResult);
}


/**
  *@brief: start to call a number
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom
  *@param    _CHAR8* strNumber    [in] call number
  *@param    RIL_DATA_CLASS eClass    [in] the data class

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_DialCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                        _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    _UCHAR8 cmd[DAIL_CALL_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];

    pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pURC, 0, (sizeof(T_URCRESULT)));
    
    pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pResult, 0, (sizeof(T_URCRESULT)));


    pURC->count = 1;
    strcpy(pURC->info[0], "NO CARRIER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO CARRIER");

    C202_WmAtV25_DialCall((_CHAR8*)cmd, &size, strNumber, eClass);

    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseDataConnect,
                                ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1,
                                EXE_NORMAL, 10, EOS_FALSE, pURC, pResult);
}

/**
  *@brief: start to call a number
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom
  *@param    _CHAR8* strNumber    [in] call number
  *@param    RIL_DATA_CLASS eClass    [in] the data class

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_DialCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                        _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    _UCHAR8 cmd[DAIL_CALL_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];

    pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pURC, 0, (sizeof(T_URCRESULT)));
    
    pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));        
    memset(pResult, 0, (sizeof(T_URCRESULT)));


    pURC->count = 1;
    strcpy(pURC->info[0], "NO CARRIER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO CARRIER");

    CM300_WmAtV25_DialCall((_CHAR8*)cmd, &size, strNumber, eClass);

    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseDataConnect,
                                ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1,
                                EXE_NORMAL, 10, EOS_FALSE, pURC, pResult);
}



/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_DeleteSMS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strIndex)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_DeleteSMS((_CHAR8*)cmd, &size, strIndex);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetSmsFormat(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strMode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetSmsFormat((_CHAR8*)cmd, &size, strMode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetTxtSmsShow(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strMode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetTxtSmsShow((_CHAR8*)cmd, &size, strMode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ListSMS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                    RIL_SMS_MODE stat)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    WaveC_WmAtExt_XDRVControlNewSMSStatus((_CHAR8*)cmd, &size, EOS_TRUE);
    cmdID = SendCmd(pWM_Handle, cmd, size, &s_MsgChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR);

    WaveC_WmAt0705_ListSMS((_CHAR8*)cmd, &size, stat);
    SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_WmPs0705_ParseSimSms, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 20);

    WaveC_WmAtExt_XDRVControlNewSMSStatus((_CHAR8*)cmd, &size, EOS_FALSE);
    return SendCmd(pWM_Handle, cmd, size, &s_MsgChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ReadSMS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                    _USHORT16 index)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_ReadSMS((_CHAR8*)cmd, &size, index);
    return SendCmd(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_WmPs0705_ReadSMS, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
} 

_UINT32 M35_ReadSMS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                    _USHORT16 index)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_ReadSMS((_CHAR8*)cmd, &size, index);
    return SendCmd(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, M35_WmPs0705_ReadSMS, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
} 


_UINT32 MC323_ReadSMS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                    _USHORT16 index)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    MC323_WmAt0705_ReadSMS((_CHAR8*)cmd, &size, index);
    return SendCmd(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, MC323_WmPs0705_ReadSMS, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ReadSMS_CDMA(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                    _USHORT16 index)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_ReadSMS((_CHAR8*)cmd, &size, index);
    return SendCmd(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_WmPs0705_ReadSMS_CDMA, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendSmsByPduMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strLength)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_SendSmsByPduMode((_CHAR8*)cmd, &size, strLength);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseCMGSOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 3, EXE_NORMAL, 3);
} 

_UINT32 MC323_SendSmsByPduMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strLength)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    MC323_WmAt0705_SendSmsByPduMode((_CHAR8*)cmd, &size, strLength);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, MC323_ParseHCMGSOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 5, EXE_NORMAL, 40);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendSmsByPduMode_CDMA(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strPDU)
{
    _UCHAR8 cmd[MAX_PDU_LEN_CDMA];
    _UINT32 size;

    WaveC_WmAt0705_SendSmsByPduMode_CDMA((_CHAR8*)cmd, &size, strPDU);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 5, EXE_NORMAL, 40);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendSmsByTxtMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strTXT)
{
    _UCHAR8 cmd[512];
    _UINT32 size;

    WaveC_WmAt0705_SendSmsByTxtMode((_CHAR8*)cmd, &size, strTXT);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseCMGSOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 3, EXE_NORMAL, 40);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SaveSmsByPduMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strLength, _UCHAR8 read_flag)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_SaveSmsByPduMode((_CHAR8*)cmd, &size, strLength, read_flag);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseCMGWOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendSmsPduData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strPDU)
{
    _UCHAR8 cmd[MAX_PDU_LEN];
    _UINT32 size;

    WaveC_WmAt0705_SendSmsPduData((_CHAR8*)cmd, &size, strPDU);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);
} 

_UINT32 WAVEC_SendQiRawData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strData, _UINT32 len)
{
    _UCHAR8 cmd[MAX_PDU_LEN];
    _UINT32 size;

    WaveC_WmAt0705_SendQiRawData((_CHAR8*)cmd, &size, strData, len);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseQiRawDataOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
} 

_UINT32 WAVEC_SendSmsTxtData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strPDU, _UINT32 uiLen)
{
    _UCHAR8 cmd[MAX_PDU_LEN];
    _UINT32 size;

    WaveC_WmAt0705_SendSmsTxtData((_CHAR8*)cmd, &size, strPDU, uiLen);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SaveSmsPduData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strPDU)
{
    _UCHAR8 cmd[MAX_PDU_LEN];
    _UINT32 size;

    WaveC_WmAt0705_SendSmsPduData((_CHAR8*)cmd, &size, strPDU);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_WmPs0705_SaveSmsPduData, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);
} 


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_StoreNewMsgMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                RIL_STORENEWMSG_MODE mode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_NewSMSAck(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_NewSMSAck((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireSmsStorage(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                RIL_MSG_STORAGE storage)
{
    _UCHAR8 cmd[INPUIRE_SMSSTORAGE_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireSmsStorage((_CHAR8*)cmd, &size, storage);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireSmsStorage, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireSmsCenter(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireSmsCenter((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireSmsCenter, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetSmsCenter(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strSca)
{
    _UCHAR8 cmd[SET_SMSCENTER_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetSmsCenter((_CHAR8*)cmd, &size, strSca);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 25);
} 

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireBroadcast(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireBroadcast((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SelectBroadcast(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,  
                            _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss)

{
    _UCHAR8 cmd[SELECT_BROADCAST_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SelectBroadcast((_CHAR8*)cmd, &size, strMode, strMids, strDcss);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SelectMsgService(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strService)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SelectMsgService((_CHAR8*)cmd, &size, strService);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SwitchDataToAtMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SWITCH_DATATOATMODE_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));
    T_pURCRESULT pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));

    pURC->count = 1;
    strcpy(pURC->info[0], "NO CARRIER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO CARRIER");

    WaveC_WmAtV25_SwitchDataToAtMode((_CHAR8*)cmd, &size);
    return SendCmdToHeadWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataResult, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10, EOS_TRUE, pURC, pResult);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_Attention(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_Attention((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_AnswerCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));
    T_pURCRESULT pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));

    pURC->count = 1;
    strcpy(pURC->info[0], "NO CARRIER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO CARRIER");

    WaveC_WmAtV25_AnswerCall((_CHAR8*)cmd, &size);
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10, EOS_FALSE, pURC, pResult);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_AnswerCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    T_pURCRESULT pURC = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));
    T_pURCRESULT pResult = (T_pURCRESULT)WIRELESS_MALLOC(sizeof(T_URCRESULT));

    pURC->count = 1;
    strcpy(pURC->info[0], "NO CARRIER");
    pResult->count = 1;
    strcpy(pResult->info[0], "NO CARRIER");

    CM300_WmAtV25_AnswerCall((_CHAR8*)cmd, &size);
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10, EOS_FALSE, pURC, pResult);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetCommandEcho(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _BOOL bOnOff)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetCommandEcho((_CHAR8*)cmd, &size, bOnOff);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_DisconnectCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAtV25_DisconnectCall((_CHAR8*)cmd, &size);
    
    return SendCmd(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_DisconnectCall(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    C202_WmAtV25_DisconnectCall((_CHAR8*)cmd, &size);
    
    return SendCmd(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_GetProductID(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_GetProductID((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsV25_GetProductID, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SwitchAtToDataMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SwitchAtToDataMode((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetResultPresentation(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetResultPresentation((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetResultFormat(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetResultFormat((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetManufDefault(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetManufDefault((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 M10_InquireCCID(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    //diff sim800 and m26
    #if (ACOM_BOARD_SUBTYPE == RL02)
    if(pWM_Handle->uiChnlId == 0)
        SIM800_WmAtV25_InquireCCID((_CHAR8*)cmd, &size);
    else
    #endif
        M10_WmAtV25_InquireCCID((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, M10_WmPsGsmCd_QueryCCID, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 C202_InquireCCID(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAtV25_InquireCCID((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, C202_WmPsGsmCd_QueryCCID, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 M10_InquireIMSI(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAtV25_InquireIMSI((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, M10_WmPsGsmCd_QueryIMSI, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireIMEI(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_InquireIMEI((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGsmCd_QueryIMEI, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 M10_InquireIMEI(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAtV25_InquireIMEI((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, M10_WmPsGsmCd_QueryIMEI, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 C202_InquireESN(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAtV25_InquireESN((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, C202_WmPsGsmCd_QueryESN, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 CM300_InquireESN(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAtV25_InquireESN((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, CM300_WmPsGsmCd_QueryESN, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetIMEI(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strIMEI)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetIMEI((_CHAR8*)cmd, &size, strIMEI);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_LockBS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strBS)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_LockBS((_CHAR8*)cmd, &size, strBS);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetBaudrate(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strRate)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetBaudrate((_CHAR8*)cmd, &size, strRate);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireBand(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_InquireBand((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGsmCd_QueryBand, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetNetBand(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strMode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtV25_SetNetBand((_CHAR8*)cmd, &size, strMode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_GetCurrentAcm(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_GetCurrentAcm((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_GetCurrentAcm, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ResetAcm(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strPasswd)
{
    _UCHAR8 cmd[RESET_ACM_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_ResetAcm((_CHAR8*)cmd, &size, strPasswd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_GetAcmMax(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_GetAcmMax((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_GetACMMax, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetAcmMax(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strAcmax, _CHAR8* strPasswd)
{
    _UCHAR8 cmd[SET_ACMMAX_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetAcmMax((_CHAR8*)cmd, &size, strAcmax, strPasswd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_GetCurCallMeter(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_GetCurCallMeter((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_GetCurCallMeter, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CallForward(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        RIL_CCFC_FWDCALLS *cfData)
{
    _UCHAR8 cmd[SET_CALLFORWARD_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_CallForward((_CHAR8*)cmd, &size, cfData);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 40);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireCallFwd(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_CCFC_REAS reason, RIL_DATA_CLASS Class)
{
    _UCHAR8 cmd[SET_CALLFORWARD_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_InquireCallFwd((_CHAR8*)cmd, &size, reason, Class);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_WmPsGsmCd_CallForward, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 40);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireClock(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireClock((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireClock, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetClock(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_SYSTIME *SysTime)
{
    _UCHAR8 cmd[SET_CLOCK_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetClock((_CHAR8*)cmd, &size, SysTime);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetCallWait(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass)
{
    _UCHAR8 cmd[SET_CALLWAIT_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_SetCallWait((_CHAR8*)cmd, &size, bURCEnable, bEnable, eClass);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 60);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireCallWait(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_DATA_CLASS eClass)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_InquireCallWait((_CHAR8*)cmd, &size, eClass);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_WmPsGsmCd_CallWait, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 60);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_GetExtendedError(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_GetExtendedError((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_GetExtendedError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetFunctionality(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strFun)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetFunctionality((_CHAR8*)cmd, &size, strFun);
    //return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetSimVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strMod)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQSIMVOL((_CHAR8*)cmd, &size, strMod);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 M35_SetRingBack(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strEn)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M35_WmAt0707_SetRingBack((_CHAR8*)cmd, &size, strEn);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 UC15_SetVBatt(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strEn)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    UC15_WmAt0707_SetVBatt((_CHAR8*)cmd, &size, strEn);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 EC20_SetNWScanMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    EC20_WmAt0707_SetNWScanMode((_CHAR8*)cmd, &size, strStr);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 WAVEC_SetOpenTCPUDP(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetOpenTCPUDP((_CHAR8*)cmd, &size, strStr);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 WAVEC_SetQindi(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetQindi((_CHAR8*)cmd, &size, strParam);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetQiDnsip(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
    _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetQiDnsip((_CHAR8*)cmd, &size, strParam);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SendQiData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strLength)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0705_SendQiData((_CHAR8*)cmd, &size, strLength);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, WaveC_ParseQISendOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 3, EXE_NORMAL, 3);
} 

_UINT32 WAVEC_InquireQiStat(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireQiStat((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireQiStat, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 WAVEC_InquireQiSack(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireQiSack((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireQiSack, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 WAVEC_InquireQiLocIP(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireQiLocIP((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireQiLocIP, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 WAVEC_InquireQiRcvData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                                   _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_InquireQiRcvData((_CHAR8*)cmd, &size, strParam);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireQiRcvData, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 2);
}

_UINT32 WAVEC_SetQiClose(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
      _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
      _USHORT16 chID=RIL_CHANNEL_ANY;
      _UINT32 size;

      WaveC_WmAt0705_SetQiClose((_CHAR8*)cmd, &size);
      WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
      return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 WAVEC_InquireQIRegApp(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireQIRegApp((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetQIRegApp(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQIRegApp((_CHAR8*)cmd, &size, strStr);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 SIM800_SetAPN(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAt0707_SetAPN((_CHAR8*)cmd, &size, strStr);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 WAVEC_SetQICsgp(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQICsgp((_CHAR8*)cmd, &size, strStr);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 WAVEC_SetQIAct(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQIAct((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 SIM800_SetCIICR(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAt0707_SetCIICR((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetQIDeact(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQIDeact((_CHAR8*)cmd, &size);
    
    WIRELESS_INFO("WAVEC_SetQIDeact:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 SIM800_SetCipStart(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_SetCipStart((_CHAR8*)cmd, &size, strStr);
    WIRELESS_INFO("SIM800_SetCipStart:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 SIM800_SetCipRxget(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_SetCipRxget((_CHAR8*)cmd, &size, strParam);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 SIM800_SendCipSend(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strLength)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    SIM800_WmAtV25_SendCipData((_CHAR8*)cmd, &size, strLength);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_MsgChID, EOS_TRUE, SIM800_WmPsV25_SendCipData, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 3, EXE_NORMAL, 3);
} 

_UINT32 SIM800_InquireCipStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_InquireCipStatus((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireQiStat, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 SIM800_InquireCipAck(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_InquireCipAck((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, SIM800_WmPsV25_InquireCipAck, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 SIM800_InquireCipRxget(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                                   _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_InquireCipRxget((_CHAR8*)cmd, &size, strParam);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, SIM800_WmPsV25_InquireCipRxget, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 2);
}

_UINT32 SIM800_SetCipClose(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
      _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
      _USHORT16 chID=RIL_CHANNEL_ANY;
      _UINT32 size;

      SIM800_WmAtV25_SetCipClose((_CHAR8*)cmd, &size);
      WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
      return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 SIM800_SetCipShut(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_SetCipShut((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 SIM800_InquireQiLocIP(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_InquireQiLocIP((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_InquireQiLocIP, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 SIM800_InquireCCID(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_InquireCCID((_CHAR8*)cmd, &size);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, M10_WmPsGsmCd_QueryCCID, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 SIM800_SetCnetScan(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_SetCnetScan((_CHAR8*)cmd, &size, strParam);
    WIRELESS_INFO("AT Cmd:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 SIM800_InquireCnetScan(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    SIM800_WmAtV25_InquireCnetScan((_CHAR8*)cmd, &size);  
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 WAVEC_InquireCellLoc(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireCellLoc((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireCellLoc, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

_UINT32 UC15_SetQFun(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strFun)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    UC15_WmAt0707_SetQFun((_CHAR8*)cmd, &size, strFun);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 WAVEC_GetFunctionality(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_GetFunctionality((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_GetFun, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_GetSimVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_GetQSIMVOL((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_GetSimVol, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetQMoStat(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strEn)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQMoStat((_CHAR8*)cmd, &size, strEn);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 UC15_SetQCCINFOEn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strEn)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQCCINFOEn((_CHAR8*)cmd, &size, strEn);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 FIBOCOM_SendSIMData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strSimData)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SendSIMData((_CHAR8*)cmd, &size, strSimData);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 UC15_SetQURCCFG(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQURCCFG((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetQJamDet(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strEn)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetQJamDet((_CHAR8*)cmd, &size, strEn);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_HoldAndMultiparty(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                T_ECALL_OPERATION operation, _USHORT16 index)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    _CHAR8 param[20], strIndex[4];

    switch(operation) {
    case eCALL_ENDHOLD:
        strcpy(param, "0");
        break;
    case eCALL_ENDACTIVE:
        strcpy(param, "1");
        break;
    case eCALL_ENDSINGLE:
        strcpy(param, "1");
        strcat(param, Utl_Itoa(index, strIndex, 10));
        break;
    case eCALL_HOLDACTIVE:
        strcpy(param, "2");
        break;
    case eCALL_HOLDACTIVEEXP:
        strcpy(param, "2");
        strcat(param, Utl_Itoa(index, strIndex, 10));
        break;
    case eCALL_CONFERENCE:
        strcpy(param, "3");
        break;
    default:
        strcpy(param, "0");
        break;
    }
    WaveC_WmAt0707_HoldAndMultiparty((_CHAR8*)cmd, &size, param);
    return SendCmd(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireSubscriber(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireSubscriber((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireSubscriber, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ListCurCalls(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    
    WaveC_WmAt0707_ListCurCalls((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, WaveC_WmPs0707_ListCurCalls,
           ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL, 2);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_ListCurCalls(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    
    WIRELESS_INFO("C202_ListCurCalls(%s)(%d)\r\n", __FILE__, __LINE__);
    C202_WmAt0707_ListCurCalls((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, C202_WmPs0707_ListCurCalls,
           ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 MC323_ListCurCalls(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WIRELESS_INFO("MC323_ListCurCalls(%s)(%d)\r\n", __FILE__, __LINE__);
    MC323_WmAt0707_ListCurCalls((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, MC323_WmPs0707_ListCurCalls,
           ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_ListCurCalls(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WIRELESS_INFO("CM300_ListCurCalls(%s)(%d)\r\n", __FILE__, __LINE__);
    CM300_WmAt0707_ListCurCalls((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &s_CallChID, EOS_TRUE, CM300_WmPs0707_ListCurCalls,
           ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL);
}



/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_FacilityLock(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass)
{
    _UCHAR8 cmd[SET_FACILITYLOCK_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    if(bLock)
    {
        WaveC_WmAt0707_FacilityLock((_CHAR8*)cmd, &size, fac, RIL_CLCK_Lock, strPasswd, eClass);
    }
    else
    {
        WaveC_WmAt0707_FacilityLock((_CHAR8*)cmd, &size, fac, RIL_CLCK_Unlock, strPasswd, eClass);
    }
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 40);
}

_UINT32 WAVEC_InqFacilityLock(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_FACILITY fac, RIL_DATA_CLASS eClass)
{
    _UCHAR8 cmd[SET_FACILITYLOCK_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_FacilityLock((_CHAR8*)cmd, &size, fac, RIL_CLCK_Query, NULL, eClass);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireLock, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 40);
}
/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CallingLineId(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_CallingLineId((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CheckCLIP(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAT0707_CheckCLIP((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGsmCd_CheckCLIP, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CheckCLIR(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAT0707_CheckCLIR((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGsmCd_CheckCLIR, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetCLIRActive(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAT0707_SetCLIRActive((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetCLIRDisable(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAT0707_SetCLIRDisable((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_DailCLIR(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strValue)
{
    _UCHAR8 cmd[SET_DAILCLIR_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAT0707_DailCLIR((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CheckCOLR(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAT0707_CheckCOLR((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGsmCd_CheckCOLR, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetSpeakerVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetSpeakerVol((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 M10_SetSIMSlot(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 simslot)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAt0707_SetDualSIM((_CHAR8*)cmd, &size, simslot);
    printf("M10_SetSIMSlot:%s\r\n", cmd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 M10_SetSpeakerVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAt0707_SetSpeakerVol((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 UC15_SetSpeakerVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    UC15_WmAt0707_SetSpeakerVol((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 M10_SetFreqBand(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 bandindex)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAt0707_SetFreqBand((_CHAR8*)cmd, &size, bandindex);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetQSFRParam(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 sfrindex)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetQSFRParam((_CHAR8*)cmd, &size, sfrindex);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_SetSpeakerVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAt0707_SetSpeakerVol((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 MC323_SetSpeakerVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC323_WmAt0707_SetSpeakerVol((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


_UINT32 C202_SetSpeakerVol(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_SPEAKERVOL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAt0707_SetSpeakerVol((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ReportMobileError(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_ReportMobileError((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_EnterMultiplex(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_EnterMultiplex((_CHAR8*)cmd, &size, "0,0,,1400,254,5,255");
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ReadOperatorNames(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_ReadOperatorNames((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_ReadOperatorNames, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);



}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireCurOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireCurOperator((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireCurOperator, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 UC15_InquireCurOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireCurOperator((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, UC15_WmPs0707_InquireCurOperator, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireBSInfo(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_InquireBSInfo((_CHAR8*)cmd, &size);
    
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_WmPs0707_InquireBSInfo, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_STOPWHENERROR, 50);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_InquireCurOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAt0707_InquireCurOperator((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, CM300_WmPs0707_InquireCurOperator, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 C202_InquireCurOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAt0707_InquireCurOperator((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, C202_WmPs0707_InquireCurOperator, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 MC323_InquireCurOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC323_WmAt0707_InquireCurOperator((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, MC323_WmPs0707_InquireCurOperator, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireAvaiOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_InquireAvaiOperator((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_WmPs0707_InquireAvaiOperator, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 60);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SelectOperator(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_NET_MODE mode, _CHAR8* strOper)
{
    _UCHAR8 cmd[SELECT_OPERATOR_SIZE];
    _UINT32 size;

    if(RIL_AUTO == mode)
    {
        WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "0", "2", NULL);
    }
    else if(RIL_MANU == mode)
    {
        WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "1", "2", strOper);
    }
    else if(RIL_COMBIN == mode)
    {
        WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "4", "2", strOper);
    }
    else if(RIL_SETFORMAT == mode) /* Just set the long string format */
    {
        WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "3", "0", strOper);
    }
    else if(RIL_SETDIGITAL == mode) /* Just set the digital format */
    {
        WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "3", "2", strOper);
    }
    else
    {
        WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "2", NULL, NULL);
    }
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 80);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetEngMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _UINT32 mode, _UINT32 dump)
{
    _UCHAR8 cmd[SELECT_OPERATOR_SIZE];
    _UINT32 size;

    WaveC_WmAt0707_SetEngMode((_CHAR8*)cmd, &size, mode, dump);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 200);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ReadPbEntries(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end)
{
    _UCHAR8 cmd[READ_PBENTRY_SIZE];
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, 0, NEWCMDID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_ReadPbEntries((_CHAR8*)cmd, &size, begin, end);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_WmPs0707_ReadPbEntries, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_STOPWHENERROR, 100);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, 0, NEWCMDID, 1, EXE_STOPWHENERROR, 50);
    }
    else
    {
        
        PWIRELESSROOTST pWirelessRootSt;
        PWIRELESSCHANNELST  pWirelessChannelCtl;
        T_HCHNL* pChnl = NULL;
        
        /* clear buffer first */
        pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
        pChnl = pWirelessChannelCtl->pChnl;
        
        WaveC_WmAt0707_ReadPbEntries((_CHAR8*)cmd, &size, begin, end);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, WaveC_WmPs0707_ReadPbEntries, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 100);
    }
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_ReadPbEntries(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end)
{
    _UCHAR8 cmd[READ_PBENTRY_SIZE];
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_ReadPbEntries((_CHAR8*)cmd, &size, begin, end);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, C202_WmPs0707_ReadPbEntries, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 100);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);
    }
    else
    {
        
        PWIRELESSROOTST pWirelessRootSt;
        PWIRELESSCHANNELST  pWirelessChannelCtl;
        T_HCHNL* pChnl = NULL;
        
        /* clear buffer first */
        pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
        pChnl = pWirelessChannelCtl->pChnl;
        
        WaveC_WmAt0707_ReadPbEntries((_CHAR8*)cmd, &size, begin, end);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, C202_WmPs0707_ReadPbEntries, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 100);
    }
}

_UINT32 MC323_ReadPbEntries(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end)
{
    _UCHAR8 cmd[READ_PBENTRY_SIZE];
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_ReadPbEntries((_CHAR8*)cmd, &size, begin, end);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, C202_WmPs0707_ReadPbEntries, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 100);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);
    }
    else
    {
        PWIRELESSROOTST pWirelessRootSt;
        PWIRELESSCHANNELST  pWirelessChannelCtl;
        T_HCHNL* pChnl = NULL;
        
        /* clear buffer first */
        pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
        pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
        pChnl = pWirelessChannelCtl->pChnl;
        
        MC323_WmAt0707_ReadPbEntries((_CHAR8*)cmd, &size, begin, end);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &s_LongTimeCmdChID, EOS_TRUE, MC323_WmPs0707_ReadPbEntries, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 100);
    }
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquirePbSpace(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_PB_STORAGE storage)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_InquirePbSpace((_CHAR8*)cmd, &size);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquirePbSpace, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);
    }
    else
    {
        WaveC_WmAt0707_InquirePbSpace((_CHAR8*)cmd, &size);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquirePbSpace, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 50);
    }
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_WritePbEntry(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                            RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText)
{
    _UCHAR8 cmd[WRITE_PBENTRY_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, 0, NEWCMDID, 1, EXE_STOPWHENERROR, 10);

        WaveC_WmAt0707_WritePbEntry((_CHAR8*)cmd, &size, index, strNumber, strText);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_STOPWHENERROR, 10);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, 0, NEWCMDID, 1, EXE_STOPWHENERROR, 10);
    }
    else
    {
        WaveC_WmAt0707_WritePbEntry((_CHAR8*)cmd, &size, index, strNumber, strText);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
    }
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_WritePbEntry(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                            RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText)
{
    _UCHAR8 cmd[WRITE_PBENTRY_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        C202_WmAt0707_WritePbEntry((_CHAR8*)cmd, &size, index, strNumber, strText);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);
    }
    else
    {
        C202_WmAt0707_WritePbEntry((_CHAR8*)cmd, &size, index, strNumber, strText);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 50);
    }
}

_UINT32 MC323_WritePbEntry(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                            RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText)
{
    _UCHAR8 cmd[WRITE_PBENTRY_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    if(storage != RIL_sSIM)
    {
        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, storage);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        MC323_WmAt0707_WritePbEntry((_CHAR8*)cmd, &size, index, strNumber, strText);
        cmdID = SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);

        WaveC_WmAt0707_SetPbStorage((_CHAR8*)cmd, &size, RIL_sSIM);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50);
    }
    else
    {
        MC323_WmAt0707_WritePbEntry((_CHAR8*)cmd, &size, index, strNumber, strText);
        return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 50);
    }
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquirePinStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;


    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST    pWirelessChannelCtl;
    T_HCHNL* pChnl = NULL;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
    pChnl = pWirelessChannelCtl->pChnl;

    WaveC_WmAt0707_InquirePinStatus((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquirePinStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL, 1);
    
}

_UINT32 WAVEC_InquireInitStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST    pWirelessChannelCtl;
    T_HCHNL* pChnl = NULL;
    
    /* clear buffer first */
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
    pChnl = pWirelessChannelCtl->pChnl;
    //WIRELESS_ReadCompleteInfo(pWM_Handle->uiChnlId, (_CHAR8 *)(pChnl->ATLineBuffer), ATLineBuffSize);
    //WIRELESS_ReadCompleteInfo(pWM_Handle->uiChnlId, (_CHAR8 *)(pChnl->ATLineBuffer), ATLineBuffSize);
    //WIRELESS_ReadCompleteInfo(pWM_Handle->uiChnlId, (_CHAR8 *)(pChnl->ATLineBuffer), ATLineBuffSize);

    WaveC_WmAt0707_InquireInitStatus((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireInitStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL, 1);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_InquirePinStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAt0707_InquirePinStatus((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, C202_WmPs0707_InquirePinStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 MC323_InquirePinStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC323_WmAt0707_InquirePinStatus((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, MC323_WmPs0707_InquirePinStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL, 1);

}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_InquirePinStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAt0707_InquirePinStatus((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, CM300_WmPs0707_InquirePinStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL);
}

_UINT32 MC600_InquirePinStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC600_WmAt0707_InquirePinStatus((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, MC600_WmPs0707_InquirePinStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 2, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_EnterPIN(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strPIN, _CHAR8* strNewPIN)

{
    _UCHAR8 cmd[ENTER_PIN_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_EnterPIN((_CHAR8*)cmd, &size, strPIN, strNewPIN);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_SetPin, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ChangePassword(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd)
{
    _UCHAR8 cmd[CHANGE_PWD_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_ChangePassword((_CHAR8*)cmd, &size, fac, strOldPwd, strNewPwd);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_SetPin, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_NetworkRegistration(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_NetworkRegistration((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireNetworkRegStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    T_URCRESULT tempURC = { 1, {"+CGREG:"} };
    T_pURCRESULT pURC = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
    memcpy(pURC, &tempURC, struSize);
    WaveC_WmAt0707_InquireNetworkRegStatus((_CHAR8*)cmd, &size, strValue);
    
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireRegisterStatus, ManagerHandle, ManagerType, CustomHandle, 
                                                NEWCMDID, 1, EXE_NORMAL, 1, EOS_FALSE, pURC, NULL);
}

_UINT32 FIBOCOM_InquireNetworkRegStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    T_URCRESULT tempURC = { 1, {"+CGREG:"} };
    T_pURCRESULT pURC = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
    memcpy(pURC, &tempURC, struSize);
    FIBOCOM_WmAt0707_InquireNetworkRegStatus((_CHAR8*)cmd, &size, strValue);
    
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_TRUE, FIBOCOM_WmPs0707_InquireRegisterStatus_With_Cid, ManagerHandle, ManagerType, CustomHandle, 
                                                NEWCMDID, 1, EXE_NORMAL, 3, EOS_FALSE, pURC, NULL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_InquireNetworkRegStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAt0707_InquireNetworkRegStatus((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, CM300_WmPs0707_InquireRegisterStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 MC600_InquireNetworkRegStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC600_WmAt0707_InquireNetworkRegStatus((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, MC600_WmPs0707_InquireRegisterStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetCharacterSet(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strChset)
{
    _UCHAR8 cmd[SET_CHARACTERSET_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetCharacterSet((_CHAR8*)cmd, &size, strChset);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireSignalQuality(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireSignalQuality((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireSignalQuality, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}
//weikk 2016.3.31 add XREG
_UINT32 WAVEC_InquireNetType(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireNetType((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireNetType, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

//weikk 20161026 add CADC
_UINT32 WAVEC_InquireADC(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireADC((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireADC, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

//weikk 2016.7.13 add MMAD
_UINT32 WAVEC_InquireMMAD(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireMMAD((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireADC, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}



_UINT32 Quectel_InquireCallStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    Quectel_WmAt0707_InquireCallStatus((_CHAR8*)cmd, &size);
    
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, Quectel_WmPs0707_InquireCallStatus, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 2);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 MC323_InquireSignalQuality(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_InquireSignalQuality((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, MC323_WmPs0707_InquireSignalQuality, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 1);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_InquireSignalQuality(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAt0707_InquireSignalQuality((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0707_InquireSignalQuality, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetSSNote(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strCSSI, _CHAR8* strCSSU)
{
    _UCHAR8 cmd[SET_SSNOTE_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SetSSNote((_CHAR8*)cmd, &size, strCSSI, strCSSU);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendUSSD(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs)
{
    _UCHAR8 cmd[SEND_USSD_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_SendUSSD((_CHAR8*)cmd, &size, strN, strStr, strDcs);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 20);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendUssdData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strUssdString)
{
    _UCHAR8 cmd[MAX_CUSD_LEN];
    _USHORT16 chID = RIL_CHANNEL_ANY;
    _UINT32 size;
    T_URCRESULT tempURC = {1, {"+CUSD:"}};
    T_URCRESULT tempResult = {1, {"+CUSD:"}};
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);
    
    WaveC_WmAt0707_SendUssdData((_CHAR8*)cmd, &size, strUssdString);
    
    pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
    memcpy(pURC, &tempURC, struSize);
    pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
    memcpy(pResult, &tempResult, struSize);
    
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsUrc_USSD, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 20, EOS_FALSE, pURC, pResult);
}

_UINT32 WAVEC_SendATOrigin(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strATString)
{
    _UCHAR8 cmd[MAX_CUSD_LEN];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    T_URCRESULT tempURC = { 1, {"+CREG:"} };
    T_pURCRESULT pURC = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    WaveC_WmAt0707_SendATOrigin((_CHAR8*)cmd, &size, strATString);
    
    pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
    memcpy(pURC, &tempURC, struSize);
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_OriginATResponse, ManagerHandle, ManagerType, CustomHandle, 
                                                    NEWCMDID, 1, EXE_NORMAL, 30, EOS_FALSE, pURC, NULL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CancelUssdData(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[MAX_CUSD_LEN];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_CancelUssdData((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 10, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_DialDTMF(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strDtmfStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0707_DialDTMF((_CHAR8*)cmd, &size, strDtmfStr);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 MC323_DialDTMF(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strDtmfStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];

    MC323_WmAt0707_DialDTMF((_CHAR8*)cmd, &size, strDtmfStr, pWirelessChannelCtl->iCallIndex);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 CM300_DialDTMF(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strDtmfStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAt0707_DialDTMF((_CHAR8*)cmd, &size, strDtmfStr);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 C202_SetDTMFPrd(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        _CHAR8* strDtmfStr)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAt0707_SetDTMFPrd((_CHAR8*)cmd, &size, strDtmfStr);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetCancel(_USHORT16 chID)
{
    _CHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAtExt_SetCancel(cmd, &size);
    
    WIRELESS_ComStrWrite(chID, cmd, strlen(cmd));
    //WIRELESS_INFO("Channel %d SetCancel %s\r\n", chID, cmd);
    /* Kinson 
    return (_USHORT16)AK_vatcWrite((_SHORT16)chID, (const _CHAR8*)cmd, (_SHORT16)size);
    */
    return 0;
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CancelDial(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _CHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_WmAtExt_SetCancel(cmd, &size);
    /*Kinson 
    return (_USHORT16)AK_vatcWrite((_SHORT16)s_CallChID, (const _CHAR8*)cmd, (_SHORT16)size);
    */
    return 0;
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetAlarm(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                        RIL_CALA_ALARM *alarmData)
{
    _UCHAR8 cmd[SET_ALARM_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetAlarm((_CHAR8*)cmd, &size, alarmData);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_FieldTestReport(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strMode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_FieldTestReport((_CHAR8*)cmd, &size, strMode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_FieldTestReport, ManagerHandle,
                    ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ReadSMSUnchgStat(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _USHORT16 index)
{
    _UCHAR8 cmd[READ_SMSUNCHGSTAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    WaveC_WmAtExt_XDRVControlNewSMSStatus((_CHAR8*)cmd, &size, EOS_TRUE);
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR);

    WaveC_WmAt0705_ReadSMS((_CHAR8*)cmd, &size, index);
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPs0705_ReadSMS, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR);

    WaveC_WmAtExt_XDRVControlNewSMSStatus((_CHAR8*)cmd, &size, EOS_FALSE);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR);
    
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SwitchOffModule(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=CHID_AT_WAVEC;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    SuspendChannel(pWM_Handle, CHID_PPP_WAVEC);
    SetChannelMode(pWM_Handle, CHID_AT_WAVEC, CHANNEL_MODE_PPP);

    WaveC_WmAtV25_DisconnectCall((_CHAR8*)cmd, &size);
    cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

    WaveC_WmAtExt_SwitchOffModule((_CHAR8*)cmd, &size);
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 C202_SwitchOffModule(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=CHID_AT_WAVEC;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    SuspendChannel(pWM_Handle, CHID_PPP_WAVEC);
    SetChannelMode(pWM_Handle, CHID_AT_WAVEC, CHANNEL_MODE_PPP);

    C202_WmAtV25_DisconnectCall((_CHAR8*)cmd, &size);
    cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

    WaveC_WmAtExt_SwitchOffModule((_CHAR8*)cmd, &size);
    return SendCmdWithAddiInfo(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetMicGainLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_MICGAINLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetMicGainLevel((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 M10_SetMicGainLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_MICGAINLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAtExt_SetMicGainLevel((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


_UINT32 UC15_SetMicGainLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_MICGAINLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    UC15_WmAtExt_SetMicGainLevel((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 CM300_SetMicGainLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_MICGAINLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    CM300_WmAtExt_SetMicGainLevel((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 MC323_SetMicGainLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_MICGAINLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC323_WmAtExt_SetMicGainLevel((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 C202_SetMicGainLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _SHORT16 volume)
{
    _UCHAR8 cmd[SET_MICGAINLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAtExt_SetMicGainLevel((_CHAR8*)cmd, &size, volume);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}


/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetAudioPath(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_AUDIO_PATH_TYPE Mode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetAudioPath((_CHAR8*)cmd, &size, Mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 M10_SetAudioPath(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_AUDIO_PATH_TYPE Mode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAtExt_SetAudioPath((_CHAR8*)cmd, &size, Mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 UC15_SetAudioMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_AUDIO_MOD_TYPE Mode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    UC15_WmAtExt_SetAudioMod((_CHAR8*)cmd, &size, Mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 S5320_SetVoiceHangupCtrl(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bEnable)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    S5320_WmAtExt_SetVoiceHangupCtrl((_CHAR8*)cmd, &size, bEnable);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 MC323_SetAudioPath(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_AUDIO_PATH_TYPE Mode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC323_WmAtExt_SetAudioPath((_CHAR8*)cmd, &size, Mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetAudioMute(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMute)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    if(bMute)
    {
        WaveC_WmAtExt_ControlModuleDriver((_CHAR8*)cmd, &size, 3);
    }
    else
    {
        WaveC_WmAtExt_ControlModuleDriver((_CHAR8*)cmd, &size, 2);
    }
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_PlayTone(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _USHORT16 type, _USHORT16 volume)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    if(volume == 10)
    {
        WaveC_WmAtExt_ControlModuleDriver((_CHAR8*)cmd, &size, 10);
    }
    else
    {
        WaveC_WmAtExt_ControlModuleDriver((_CHAR8*)cmd, &size, 11);
    }
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireHeadsetStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_InquireHeadsetStatus((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireHeadsetBtnStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_InquireHeadsetBtnStatus((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireBatteryLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_InquireBatteryLevel((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquireBatteryLevel, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquirePinCount(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_UCHAR8 type)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_InquirePinCount((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquirePinCount, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquirePukCount(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_InquirePukCount((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquirePinCount, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_DumpBroadCast(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_DumpBroadCast((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetRingerSoundLevel(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _SHORT16 level)
{
    _UCHAR8 cmd[SET_RINGERSOUNDLEVEL_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetRingerSoundLevel((_CHAR8*)cmd, &size, level);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetAudioOutput(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strAudioParam)
{
    
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetAudioOutput((_CHAR8*)cmd, &size, strAudioParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetSmsFullUrc(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetSmsFullUrc((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_QueryModuleState(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_QueryModuleState((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_ParseModuleState, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_InquireCOLP(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_InquireCOLP((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGsmCd_CheckCOLP, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetColpStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetColpStatus((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_CheckLatestURCEvent(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strIndex)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_CheckLatestURCEvent((_CHAR8*)cmd, &size, strIndex);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_CheckLatestURCEvent, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ControlURCIndication(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_ControlURCIndication((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_ControlURCIndication, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ControlModuleDriver(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                                _UCHAR8 mode)
{
    _UCHAR8 cmd[CONTROL_MODULEDRI_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_ControlModuleDriver((_CHAR8*)cmd, &size, mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_QueryURCIndication(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_QueryURCIndication((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_QueryURCIndication, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_MobileEquipmentEvent(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_MobileEquipmentEvent((_CHAR8*)cmd, &size, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SendAT(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SendAT((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 3);
}

_UINT32 WAVEC_PwrCtrl(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_PwrCtrl((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 3);
}

_UINT32 UC15_PwrCtrl(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    UC15_WmAtExt_PwrCtrl((_CHAR8*)cmd, &size);
    return SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 3);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetMSStorageType(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strService)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetMSStorageType((_CHAR8*)cmd, &size, strService);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetExtSMSStorageStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _BOOL bFull)
{
    
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;
    _CHAR8* strService;

    if (bFull)
    {
        strService = "1";
    }
    else
    {
        strService = "0";
    }

    WaveC_WmAtExt_SetExtSMSStorageStatus((_CHAR8*)cmd, &size, strService);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetModuleSleep(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_UCHAR8 enable)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetModuleSleep((_CHAR8*)cmd, &size,enable);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_GetModemVersionNum(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_GetModemVersionNum((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsV25_GetProductID, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetChargingCurrent(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                _CHAR8* strStep)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID = RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetChargingCurrent((_CHAR8*)cmd, &size, strStep);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}
/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_ConfigURCMode(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            RIL_URC_MODE mode)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=CHID_AT_WAVEC;
    _UINT32 size;

    if(RIL_URC_SLEEP == mode)
    {
        WaveC_WmAtExt_SetSignalStrengthReport((_CHAR8*)cmd, &size, EOS_FALSE);
        SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    else if(RIL_URC_NORMAL == mode)
    {
        WaveC_WmAtExt_SetSignalStrengthReport((_CHAR8*)cmd, &size, EOS_TRUE);
        SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    
    WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, mode);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}
/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
 _UINT32 WAVEC_XDRVURCControl(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                           RIL_sXDRV_TYPE *xdrvType)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID = RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, xdrvType);
    if((xdrvType->devName == RIL_NAME_STATICEEP)
        &&(xdrvType->ctrType == RIL_TYPE_INQUIRE)
        &&(xdrvType->ctrEnable == EOS_FALSE)
        &&(Utl_StrCmp(xdrvType->strValue1, "1556") == 0))
    {
        return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquireStaticEEP, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    else if((xdrvType->devName == RIL_NAME_CHIPID)
            &&(xdrvType->ctrType == RIL_TYPE_INQUIRE)
            &&(xdrvType->ctrEnable == EOS_FALSE))
    {
        return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquireChipID, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    else if((xdrvType->devName == RIL_NAME_BATTERYIDVOL)
            &&(xdrvType->ctrType ==RIL_TYPE_INQUIRE)
            &&(xdrvType->ctrEnable == EOS_FALSE))
    {
        return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquireBattTemperADC, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    else if ((xdrvType->devName == RIL_NAME_BETTERYVOLTAGE)
            &&(xdrvType->ctrType == RIL_TYPE_INQUIRE)
            &&(xdrvType->ctrEnable == EOS_FALSE))
    {
        return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquireBatteryVoltage, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    else if((xdrvType->devName == RIL_NAME_ENVIRTEMPER)
            &&(xdrvType->ctrType == RIL_TYPE_INQUIRE)
            &&(xdrvType->ctrEnable == EOS_FALSE))
    {
        return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsExt_InquireEnvirTemper, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
   else
   {
        return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
   }
}
                           
 /**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
 _UINT32 WAVEC_XDRVAudioControl(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                             RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_XDRVAudioControl((_CHAR8*)cmd, &size, audCtrlType, strValue);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}
 
 
     /**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
 _UINT32 WAVEC_SetTraceOutput(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                             _BOOL bOnOff)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_SetTraceOutput((_CHAR8*)cmd, &size, bOnOff);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    
}
 
 /**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
 _UINT32 WAVEC_GetModemExcLog(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
 {
     _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_GetModemExcLog((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
 }
 
     /**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/        
 _UINT32 WAVEC_XDRVControlNewSMSStatus(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                                          _BOOL enable)
 {
     _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_XDRVControlNewSMSStatus((_CHAR8*)cmd, &size, enable);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
 }
 
 /**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/        
 _UINT32 WAVEC_XDRVControlAudioLoopTest(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,
                                          _BOOL enable)
 {
     _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtExt_XDRVControlAudioLoopTest((_CHAR8*)cmd, &size, enable);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
 }

/**
*@brief: If the new incoming message need acknowledge
*@return    _BOOL         true: need acknowledge
*/        
_BOOL WAVEC_NewMsgNeedAck(_VOID)
{
    return EOS_TRUE;
}

/**
*@brief: If the ATD command return directly before call establishing.
*@return    _BOOL         true: need acknowledge
*/        
_BOOL WAVEC_ATDReturnDirectly(_VOID)
{
    return EOS_TRUE;
}

/**
*@brief: If received the AT command end flag
*@return    _BOOL         true: need acknowledge
*/        
_BOOL WAVEC_CheckATResult(const _CHAR8* ATResponse, _USHORT16 size)
{
    if (strstr(ATResponse, "OK\r\n") != NULL ||
         strstr(ATResponse, "ERROR") != NULL ||
         strstr(ATResponse, ">") != NULL ||
         strstr(ATResponse, "+CPIN:") != NULL) 
    {
        return EOS_TRUE;
    }
    
    return EOS_FALSE;
}

/**
  *@brief: Init wireless module before PIN
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return
*/        
_UINT32 WAVEC_InitBeforePIN(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=1;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;
    RIL_sXDRV_TYPE xdrvType;
    
    WaveC_WmAtV25_SetCommandEcho((_CHAR8*)cmd, &size, EOS_TRUE);
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_SetTraceOutput((_CHAR8*)cmd, &size, EOS_TRUE);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
    
    xdrvType.devName = RIL_NAME_HEADSET;
    xdrvType.ctrType = RIL_TYPE_SETURCRPT;
    xdrvType.ctrEnable = EOS_TRUE;
    WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    xdrvType.devName = RIL_NAME_BATTERYLOW;
    xdrvType.ctrType = RIL_TYPE_SETURCRPT;
    xdrvType.ctrEnable = EOS_TRUE;
    WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_InquireHeadsetStatus((_CHAR8*)cmd, &size);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

/*
    xdrvType.devName = RIL_NAME_BATTERYIDVOL;
    xdrvType.ctrType = RIL_TYPE_SETURCRPT;
    xdrvType.ctrEnable = EOS_TRUE;
    xdrvType.strValue1 = "2000";
    xdrvType.strValue2 = "7000";
    WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
*/    
    WaveC_WmAtExt_XDRVAudioControl((_CHAR8*)cmd, &size, RIL_AUDIO_MASTER_VOLUME, "100");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, RIL_URC_BUSY);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_ReportMobileError((_CHAR8*)cmd, &size, "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    chID = 2;
    WaveC_WmAt0707_ReportMobileError((_CHAR8*)cmd, &size, "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, RIL_URC_DATACONN);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
    return 0;
}


/**
  *@brief: Init wireless module after PIN
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return
*/        
_UINT32 WAVEC_InitAfterPIN(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=1;
    _UINT32 size;
    _UINT32 cmdID = NEWCMDID;

    FilterShortLine = EOS_TRUE; /* after send ATE1, 如果再有仅仅OK的返回，可以过滤掉*/

    WaveC_WmAtExt_MobileEquipmentEvent((_CHAR8*)cmd, &size, "1,1,0,2,1");
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtV25_SetResultFormat((_CHAR8*)cmd, &size, "1");
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
    
    WaveC_WmAtV25_SetResultPresentation((_CHAR8*)cmd, &size, "0");
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_SetCharacterSet((_CHAR8*)cmd, &size, "UCS2");
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0705_SetSmsFormat((_CHAR8*)cmd, &size, "0");
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_NetworkRegistration((_CHAR8*)cmd, &size, "1");
    cmdID = SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
    
    WaveC_WmAt0705_SelectMsgService((_CHAR8*)cmd, &size, "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_CallingLineId((_CHAR8*)cmd, &size, "0");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_SetColpStatus((_CHAR8*)cmd, &size, "0");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_SetSSNote((_CHAR8*)cmd, &size, "1", "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_MEFIRST);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_SendUSSD((_CHAR8*)cmd, &size, "1", NULL, NULL);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_ReportCallStat((_CHAR8*)cmd, &size);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    chID = 2;
    WaveC_WmAtV25_SetResultFormat((_CHAR8*)cmd, &size, "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
    
    WaveC_WmAtV25_SetResultPresentation((_CHAR8*)cmd, &size, "0");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_SetCharacterSet((_CHAR8*)cmd, &size, "UCS2");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0705_SetSmsFormat((_CHAR8*)cmd, &size, "0");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

/*
    WaveC_WmAt0707_NetworkRegistration((_CHAR8*)cmd, &size, "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
*/    
    WaveC_WmAt0705_SelectMsgService((_CHAR8*)cmd, &size, "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_CallingLineId((_CHAR8*)cmd, &size, "0");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAtExt_SetColpStatus((_CHAR8*)cmd, &size, "0");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

/*
    WaveC_WmAt0707_SetSSNote((_CHAR8*)cmd, &size, "1", "1");
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
*/
    WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_MEFIRST);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_SendUSSD((_CHAR8*)cmd, &size, "1", NULL, NULL);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);
    
    WaveC_WmAt0707_SetTimeZoneReporting((_CHAR8*)cmd, &size, EOS_TRUE);
    SendCmd(pWM_Handle, cmd, size, &chID, EOS_FALSE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL);

    WaveC_WmAt0707_SelectOperator((_CHAR8*)cmd, &size, "0", "2", NULL);
    SendCmdWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 200);

    return 0;
}

_UINT32 WAVEC_STKSetProfile(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_STK_SetProfile((_CHAR8*)cmd, &size);
    SendCmd(pWM_Handle, cmd, size, &s_STKChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);

    return 0;
}

_UINT32 WAVEC_STKSetAutoRsp(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    M35_STK_SetAutoRsp((_CHAR8*)cmd, &size);
    SendCmd(pWM_Handle, cmd, size, &s_STKChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);

    return 0;
}

_UINT32 WAVEC_STKSetTR(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    M35_STK_SetTR((_CHAR8*)cmd, &size);
    SendCmd(pWM_Handle, cmd, size, &s_STKChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);

    return 0;
}


_UINT32 WAVEC_STKSelectMainMenu(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 index)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    WaveC_STK_SelectMainMenu((_CHAR8*)cmd, &size, index);
    SendCmd(pWM_Handle, cmd, size, &s_STKChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);

    return 0;
}

_UINT32 WAVEC_STKResponse(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_RES_PAR *par)
{
    _UCHAR8 cmd[STK_RESPONSE_SIZE];
    _UINT32 size;

    WaveC_STK_Response((_CHAR8*)cmd, &size, par);
    SendCmdWithTimeout(pWM_Handle, cmd, size, &s_STKChID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 30);

    return 0;
}

/**
*@brief: If the STK detail info within URC.
*@return    _BOOL         
*/        
_BOOL WAVEC_STKDetailInfoInURC(_VOID)
{
    return EOS_TRUE;
}

_UINT32 WAVEC_InquireGprsState(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAtGprs_InquireGprsState((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_WmPsGprs_InquireGprsState, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_EstablishDataConnWithGPRS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _CHAR8* strAPN, _BOOL bMUX, _USHORT16 step)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    RIL_sXDRV_TYPE xdrvType;
    _UINT32 cmdID = NEWCMDID;
    T_URCRESULT tempURC = { 3, {"NO CARRIER", "NO ANSWER", "CONNECT"} };
    T_URCRESULT tempResult = { 3, {"NO CARRIER", "NO ANSWER", "CONNECT"} };
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    if(bMUX)
    {
/*        attach to GPRS*/
/*        WaveC_WmAtGprs_GprsAttachDetach((_CHAR8*)cmd, &size, "1");
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
*/        
        /*define PDF context*/
        WaveC_WmAtGprs_DefinePDPContext((_CHAR8*)cmd, &size, "1", "IP", strAPN, NULL);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        
        /*define quality of service*/
        WaveC_WmAtGprs_QualityOfService((_CHAR8*)cmd, &size, "1,,,1");
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        
        /*Dial to GPRS network*/
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        WaveC_WmAtGprs_RequestGprsService((_CHAR8*)cmd, &size);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
    }
    else
    {
        /*change new Msg store mode, New SMS will store in the SIM, 
            then inform the ME with new SMS index*/
        
        WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_SIMFIRST);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);

        /*disable signal URC report*/
        WaveC_WmAtExt_SetSignalStrengthReport((_CHAR8*)cmd, &size, EOS_FALSE);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);

        /*disable new SMS URC*/
        WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, RIL_URC_DATACONN);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        /*disable urc of headset and hookkey*/
        xdrvType.devName = RIL_NAME_HEADSET;
        xdrvType.ctrType = RIL_TYPE_SETURCRPT;
        xdrvType.ctrEnable = EOS_FALSE;
        WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
/*        attach to GPRS*/
/*        WaveC_WmAtGprs_GprsAttachDetach((_CHAR8*)cmd, &size, "1");
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
*/
        /*enable ATO*/
        strcpy((_CHAR8*)cmd, "AT&D2\r");
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        /*define PDF context*/
        WaveC_WmAtGprs_DefinePDPContext((_CHAR8*)cmd, &size, "1", "IP", strAPN, NULL);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        /*define quality of service*/
        WaveC_WmAtGprs_QualityOfService((_CHAR8*)cmd, &size, "1,,,1");
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        /*Dial to GPRS network*/
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        WaveC_WmAtGprs_RequestGprsService((_CHAR8*)cmd, &size);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
    }
    return cmdID;
}

_UINT32 WAVEC_EstablishDataConnWithCSD(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _USHORT16 step)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    RIL_sXDRV_TYPE xdrvType;
    _UINT32 cmdID = NEWCMDID;
    T_URCRESULT tempURC = { 3, {"NO CARRIER", "NO ANSWER", "CONNECT"} };
    T_URCRESULT tempResult = { 5, {"NO CARRIER", "NO ANSWER", "username:", "password:", "PPP mode."} };
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    if(bMUX)
    {
        /*should parse "username:"*/
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        WaveC_WmAtV25_DialCall((_CHAR8*)cmd, &size, dial, RIL_dcDATA);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50, EOS_TRUE, pURC, pResult);
        
        /*write user name*/
        /*should parse "password:"*/
        strcpy((_CHAR8*)cmd, user);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
        
        /*write password*/
        /*should parse "PPP mode."*/
        strcpy((_CHAR8*)cmd, pwd);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);        
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
    }
    else
    {
        /*change new Msg store mode, New SMS will store in the SIM,
            then inform the ME with new SMS index*/
        WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_SIMFIRST);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);

        /*disable signal URC report*/
        WaveC_WmAtExt_SetSignalStrengthReport((_CHAR8*)cmd, &size, EOS_FALSE);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);

        /*disable new SMS URC*/
        WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, RIL_URC_DATACONN);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        /*disable urc of headset and hookkey*/
        xdrvType.devName = RIL_NAME_HEADSET;
        xdrvType.ctrType = RIL_TYPE_SETURCRPT;
        xdrvType.ctrEnable = EOS_FALSE;
        WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        
        /*should parse "username:"*/
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        WaveC_WmAtV25_DialCall((_CHAR8*)cmd, &size, dial, RIL_dcDATA);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
        
        /*write user name*/
        /*should parse "password:"*/
        strcpy((_CHAR8*)cmd, user);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
        
        /*write password*/
        /*should parse "PPP mode."*/
        strcpy((_CHAR8*)cmd, pwd);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
    }
    return cmdID;
}

_UINT32 C202_EstablishDataConnWithCSD(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                                    _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _USHORT16 step)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    RIL_sXDRV_TYPE xdrvType;
    _UINT32 cmdID = NEWCMDID;
    T_URCRESULT tempURC = { 3, {"NO CARRIER", "NO ANSWER", "CONNECT"} };
    T_URCRESULT tempResult = { 5, {"NO CARRIER", "NO ANSWER", "username:", "password:", "PPP mode."} };
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    if(bMUX)
    {
        /*should parse "username:"*/
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        C202_WmAtV25_DialCall((_CHAR8*)cmd, &size, dial, RIL_dcDATA);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 50, EOS_TRUE, pURC, pResult);
        
        /*write user name*/
        /*should parse "password:"*/
        strcpy((_CHAR8*)cmd, user);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
        
        /*write password*/
        /*should parse "PPP mode."*/
        strcpy((_CHAR8*)cmd, pwd);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);        
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
    }
    else
    {
        /*change new Msg store mode, New SMS will store in the SIM,
            then inform the ME with new SMS index*/
        WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_SIMFIRST);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);

        /*disable signal URC report*/
        WaveC_WmAtExt_SetSignalStrengthReport((_CHAR8*)cmd, &size, EOS_FALSE);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);

        /*disable new SMS URC*/
        WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, RIL_URC_DATACONN);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        /*disable urc of headset and hookkey*/
        xdrvType.devName = RIL_NAME_HEADSET;
        xdrvType.ctrType = RIL_TYPE_SETURCRPT;
        xdrvType.ctrEnable = EOS_FALSE;
        WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, NULL, NULL);
        
        /*should parse "username:"*/
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        C202_WmAtV25_DialCall((_CHAR8*)cmd, &size, dial, RIL_dcDATA);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
        
        /*write user name*/
        /*should parse "password:"*/
        strcpy((_CHAR8*)cmd, user);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
        
        /*write password*/
        /*should parse "PPP mode."*/
        strcpy((_CHAR8*)cmd, pwd);
        strcat((_CHAR8*)cmd, ATCMD_RETURN);
        size = (_USHORT16)strlen((const _CHAR8*)cmd);
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnectCSD, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_STOPWHENERROR, 10, EOS_TRUE, pURC, pResult);
    }
    return cmdID;
}


_UINT32 WAVEC_ReleaseDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;
    RIL_sXDRV_TYPE xdrvType;
    _UINT32 cmdID = NEWCMDID;
    T_URCRESULT tempURC = { 1, {"NO CARRIER"} };
    T_URCRESULT tempResult = { 1, {"NO CARRIER"} };
    T_pURCRESULT pURC = NULL;
    T_pURCRESULT pResult = NULL;
    _UINT32 struSize = sizeof(T_URCRESULT);

    if(bMUX)
    {
        if(step < 1)
        {
            pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
            memcpy(pURC, &tempURC, struSize);
            pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
            memcpy(pResult, &tempResult, struSize);
            WaveC_WmAtV25_SwitchDataToAtMode((_CHAR8*)cmd, &size);
            cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataResult, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, pURC, pResult);
        }

        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        WaveC_WmAtV25_Attention((_CHAR8*)cmd, &size);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataResult, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 20, EOS_TRUE, pURC, pResult);
    }
    else
    {
        pURC = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pURC, &tempURC, struSize);
        pResult = (T_pURCRESULT)WIRELESS_MALLOC(struSize);
        memcpy(pResult, &tempResult, struSize);
        WaveC_WmAtV25_SwitchDataToAtMode((_CHAR8*)cmd, &size);
        cmdID = SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataResult, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, pURC, pResult);

        /*T500代码中，如果当前电话不为空，什么也不做。
        否则，发ATH挂断GPRS电话，不知道这里是否要跟Call manager联系起来只挂GPRS call*/
        WaveC_WmAtV25_Attention((_CHAR8*)cmd, &size);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

        /*enable ATO*/
        strcpy((_CHAR8*)cmd, "AT&D1\r");
        size = (_USHORT16)strlen((_CHAR8*)cmd);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

        WaveC_WmAtV25_SwitchAtToDataMode((_CHAR8*)cmd, &size);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

        WaveC_WmAtExt_SetSignalStrengthReport((_CHAR8*)cmd, &size, EOS_TRUE);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

        WaveC_WmAtExt_ConfigURCMode((_CHAR8*)cmd, &size, RIL_URC_NORMAL);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

        /*enable urc of headset and hookkey*/
        xdrvType.devName = RIL_NAME_HEADSET;
        xdrvType.ctrType = RIL_TYPE_SETURCRPT;
        xdrvType.ctrEnable = EOS_TRUE;
        WaveC_WmAtExt_XDRVURCControl((_CHAR8*)cmd, &size, &xdrvType);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_FALSE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);

        WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_MEFIRST);
        SendCmdWithAddiInfo(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, cmdID, 1, EXE_NORMAL, 10, EOS_TRUE, NULL, NULL);
    }
    return cmdID;
}

_UINT32 WAVEC_SuspendDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    if(bMUX)
    {
    }
    else
    {
        WaveC_WmAtV25_SwitchDataToAtMode((_CHAR8*)cmd, &size);
        /*receive "NO CARRIER" or "OK" or "ERROR" all means OK*/
        SendCmd(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataResult, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);

        WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_MEFIRST);
        SendCmd(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    return 0;
}

_UINT32 WAVEC_ResumeDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _UINT32 size;

    if(bMUX)
    {
    }
    else
    {
        /*change new Msg store mode, New SMS will store in the SIM, 
        then inform the ME with new SMS index*/
        WaveC_WmAt0705_NewSMSIndication((_CHAR8*)cmd, &size, RIL_STORENEWMSG_SIMFIRST);
        SendCmd(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);

        WaveC_WmAtV25_SwitchAtToDataMode((_CHAR8*)cmd, &size);
        SendCmd(pWM_Handle, cmd, size, &s_DataConnChID, EOS_TRUE, WaveC_ParseDataConnect, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
    }
    return 0;
}

_VOID WAVEC_SetDataConnMode(HWM pWM_Handle, T_DATACALL_TYPE DataMode)
{
    if(DATACALL_CSD == DataMode || DATACALL_GPRS == DataMode)
    {
        SetChannelMode(pWM_Handle, s_DataConnChID, CHANNEL_MODE_PPP);
    }
    else
    {
        SetChannelMode(pWM_Handle, s_DataConnChID, CHANNEL_MODE_AT);
    }
}

_UINT32 WAVEC_ResetWM(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _USHORT16 chID=1;

    return SendHardwareCmd(pWM_Handle, CMDTYPE_RESETMODULE, &chID, EOS_TRUE, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 C202_ResetByAT(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    C202_WmAt0707_Reset((_CHAR8*)cmd, &size);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 MC323_ResetByAT(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    MC323_WmAt0707_Reset((_CHAR8*)cmd, &size);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 M10_ResetByAT(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAt0707_Reset((_CHAR8*)cmd, &size);
    return SendCmd_ToHeadWithTimeout(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL, 10);
}

_UINT32 WAVEC_IgniteWM(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _USHORT16 chID=1;

    return SendHardwareCmd(pWM_Handle, CMDTYPE_IGNITE, &chID, EOS_TRUE, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_Reset(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _USHORT16 chID=1;

    return SendHardwareCmd(pWM_Handle, CMDTYPE_RESETWM, &chID, EOS_TRUE, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetBatteryParam(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetBatteryParam((_CHAR8*)cmd, &size, strParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetToneDetParam(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetToneDetParam((_CHAR8*)cmd, &size, strParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 WAVEC_SetDAIParam(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetDAIParam((_CHAR8*)cmd, &size, strParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SelectVoiceGain(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SelectVoiceGain((_CHAR8*)cmd, &size, strParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SetSideTone(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SetSideTone((_CHAR8*)cmd, &size, strParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 M10_SetSideTone(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, 
                            _CHAR8* strParam)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAt0705_SetSideTone((_CHAR8*)cmd, &size, strParam);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

_UINT32 M10_InquireSideTone(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    M10_WmAt0705_InquireSideTone((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, M10_WmPsGsmCd_QuerySideTone, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: 
  *@param    HWM pWM_Handle        [in] the handle of wireless module
  *@param    _UINT32 ManagerHandle    [in] the handle of server manager module
  *@param    T_EDEVICEETYPE ManagerType    [in] the type of server manager module
  *@param    _UINT32 CustomHandle    [in] the handle of custom

  *@return    _BOOL         true is successful and otherwise is failed
*/
_UINT32 WAVEC_SaveConfiguration(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UCHAR8 cmd[SEND_STANDARDAT_SIZE];
    _USHORT16 chID=RIL_CHANNEL_ANY;
    _UINT32 size;

    WaveC_WmAt0705_SaveConfiguration((_CHAR8*)cmd, &size);
    return SendCmd(pWM_Handle, cmd, size, &chID, EOS_TRUE, WaveC_ParseOkError, ManagerHandle, ManagerType, CustomHandle, NEWCMDID, 1, EXE_NORMAL);
}

/**
  *@brief: initialize the wireless module as infineon module
  *@param    HWM pWM_Handle    [in] the handle of wireless module
  *@return    _VOID
*/
_VOID WAVEC_Create(HWM pWM_handle)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_handle->uiChnlId];

    /*GSM0705 AT command    */ 
    pWM_handle->pATFunction->P_Delete_SMS = WAVEC_DeleteSMS;
    pWM_handle->pATFunction->P_Set_SmsFormat = WAVEC_SetSmsFormat;
    pWM_handle->pATFunction->P_Set_TxtSmsShow = WAVEC_SetTxtSmsShow;
    pWM_handle->pATFunction->P_List_SMS = WAVEC_ListSMS;
    if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Read_SMS = MC323_ReadSMS;
    }
    else
    {
        pWM_handle->pATFunction->P_Read_SMS = WAVEC_ReadSMS;
    }

    pWM_handle->pATFunction->P_Read_SMS_ByTxt = M35_ReadSMS;
    
    pWM_handle->pATFunction->P_Read_SMS_CDMA = WAVEC_ReadSMS_CDMA;
    if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_SendSms_ByPduMode = MC323_SendSmsByPduMode;
    }
    else
    {
        pWM_handle->pATFunction->P_SendSms_ByPduMode = WAVEC_SendSmsByPduMode;
    }
    
    pWM_handle->pATFunction->P_SendSms_ByPduMode_CDMA = WAVEC_SendSmsByPduMode_CDMA;
    pWM_handle->pATFunction->P_SendSms_ByTxtMode = WAVEC_SendSmsByTxtMode;
    pWM_handle->pATFunction->P_SaveSms_ByPduMode = WAVEC_SaveSmsByPduMode;
    pWM_handle->pATFunction->P_SendSms_PduData = WAVEC_SendSmsPduData;
    pWM_handle->pATFunction->P_SendQiRawData = WAVEC_SendQiRawData;
    
    pWM_handle->pATFunction->P_SendSms_TxtData = WAVEC_SendSmsTxtData;
    pWM_handle->pATFunction->P_SaveSms_PduData = WAVEC_SaveSmsPduData;
    pWM_handle->pATFunction->P_StoreNewMsgMode = WAVEC_StoreNewMsgMode;
    pWM_handle->pATFunction->P_NewSMS_Ack = WAVEC_NewSMSAck;
    pWM_handle->pATFunction->P_Inquire_SmsStorage = WAVEC_InquireSmsStorage;
    pWM_handle->pATFunction->P_Inquire_SmsCenter = WAVEC_InquireSmsCenter;
    pWM_handle->pATFunction->P_Set_SmsCenter = WAVEC_SetSmsCenter;
    pWM_handle->pATFunction->P_Inquire_Broadcast = WAVEC_InquireBroadcast;
    pWM_handle->pATFunction->P_Select_Broadcast = WAVEC_SelectBroadcast;
    pWM_handle->pATFunction->P_Select_MsgService = WAVEC_SelectMsgService;
    
    
    /*v25ter AT command */
    pWM_handle->pATFunction->P_SwitchData_ToAtMode = WAVEC_SwitchDataToAtMode;
    pWM_handle->pATFunction->P_Attention = WAVEC_Attention;
    if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Answer_Call = CM300_AnswerCall;
    }
    else
    {
        pWM_handle->pATFunction->P_Answer_Call = WAVEC_AnswerCall;
    }
    
    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Dial_Call = C202_DialCall;
    }
    else if((NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Dial_Call = CM300_DialCall;
    }
    else if((NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType) 
        || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) 
        || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Dial_Call = U10_DialCall;;
    }
    else
    {
        pWM_handle->pATFunction->P_Dial_Call = WAVEC_DialCall;
    }
    
    pWM_handle->pATFunction->P_Set_CommandEcho = WAVEC_SetCommandEcho;
    
    if((NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Disconnect_Call = C202_DisconnectCall;
    }
    else
    {
        pWM_handle->pATFunction->P_Disconnect_Call = WAVEC_DisconnectCall;
    }
    
    pWM_handle->pATFunction->P_Get_ProductID = WAVEC_GetProductID;
    pWM_handle->pATFunction->P_SwitchAt_ToDataMode = WAVEC_SwitchAtToDataMode;
    pWM_handle->pATFunction->P_Set_ResultPresentation = WAVEC_SetResultPresentation;
    pWM_handle->pATFunction->P_Set_ResultFormat = WAVEC_SetResultFormat;
    pWM_handle->pATFunction->P_Set_ManufDefault = WAVEC_SetManufDefault;
    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_M26 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_EC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Inquire_CCID = M10_InquireCCID;
    }
	else if(NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
	{
	    pWM_handle->pATFunction->P_Inquire_CCID = SIM800_InquireCCID;
	}
    else if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_CCID = CM300_InquireESN;
    }
    else if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_CCID = C202_InquireCCID;
    }
    else
    {
        pWM_handle->pATFunction->P_Inquire_CCID = WAVEC_InquireIMEI;
    }

    pWM_handle->pATFunction->P_Inquire_IMSI = M10_InquireIMSI;

#if 0
    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Inquire_IMEI = M10_InquireIMEI;
    }
    else if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_IMEI = C202_InquireESN;
    }
    else if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_IMEI = CM300_InquireESN;
    }
    else
#endif        
    {
        pWM_handle->pATFunction->P_Inquire_IMEI = WAVEC_InquireIMEI;
    }
    pWM_handle->pATFunction->P_Set_IMEI = WAVEC_SetIMEI;
    pWM_handle->pATFunction->P_Lock_BS = WAVEC_LockBS;
    pWM_handle->pATFunction->P_Set_Baudrate = WAVEC_SetBaudrate;
    pWM_handle->pATFunction->P_Inquire_Band = WAVEC_InquireBand;
    pWM_handle->pATFunction->P_Set_NetBand = WAVEC_SetNetBand;
    
    /*GSM0707 AT command*/
    pWM_handle->pATFunction->P_Get_CurrentAcm = WAVEC_GetCurrentAcm;
    pWM_handle->pATFunction->P_Reset_Acm = WAVEC_ResetAcm;
    pWM_handle->pATFunction->P_Get_AcmMax = WAVEC_GetAcmMax;
    pWM_handle->pATFunction->P_Set_AcmMax = WAVEC_SetAcmMax;
    pWM_handle->pATFunction->P_Get_CurCallMeter = WAVEC_GetCurCallMeter;
    pWM_handle->pATFunction->P_Call_Forward = WAVEC_CallForward;
    pWM_handle->pATFunction->P_Inquire_CallFwd = WAVEC_InquireCallFwd;
    pWM_handle->pATFunction->P_Inquire_Clock = WAVEC_InquireClock;
    pWM_handle->pATFunction->P_Set_Clock = WAVEC_SetClock;
    pWM_handle->pATFunction->P_SetCall_Wait = WAVEC_SetCallWait;
    pWM_handle->pATFunction->P_Inquire_CallWait = WAVEC_InquireCallWait;
    pWM_handle->pATFunction->P_Get_ExtendedError = WAVEC_GetExtendedError;
    pWM_handle->pATFunction->P_Get_Functionality = WAVEC_GetFunctionality;
    pWM_handle->pATFunction->P_Get_SimVol = WAVEC_GetSimVol;
    pWM_handle->pATFunction->P_Set_QFun = UC15_SetQFun;
    pWM_handle->pATFunction->P_Set_Functionality = WAVEC_SetFunctionality;
    pWM_handle->pATFunction->P_Set_SimVol = WAVEC_SetSimVol;
    
    pWM_handle->pATFunction->P_Set_RingBackCfg = M35_SetRingBack;    
    pWM_handle->pATFunction->P_Set_VBatt = UC15_SetVBatt;
    pWM_handle->pATFunction->P_Set_NWScanMode = EC20_SetNWScanMode;
    if (NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_OpenTCPUDP = SIM800_SetCipStart;
        pWM_handle->pATFunction->P_Set_Qindi = SIM800_SetCipRxget;
        pWM_handle->pATFunction->P_SendQidata = SIM800_SendCipSend;
        pWM_handle->pATFunction->P_InquireQiStat = SIM800_InquireCipStatus;
        pWM_handle->pATFunction->P_InquireQiSack = SIM800_InquireCipAck;
        pWM_handle->pATFunction->P_InquireQiRcvData = SIM800_InquireCipRxget;
        pWM_handle->pATFunction->P_SendQiClose = SIM800_SetCipClose;
        pWM_handle->pATFunction->P_Set_QIDeact = SIM800_SetCipShut;
        pWM_handle->pATFunction->P_InquireQiLocIP = SIM800_InquireQiLocIP;
        pWM_handle->pATFunction->P_Set_CnetScan = SIM800_SetCnetScan;
        pWM_handle->pATFunction->P_Inquire_CellLoc = SIM800_InquireCnetScan;
    }
    else
    {
        pWM_handle->pATFunction->P_Set_OpenTCPUDP = WAVEC_SetOpenTCPUDP;
        pWM_handle->pATFunction->P_Set_Qindi = WAVEC_SetQindi;
        pWM_handle->pATFunction->P_SendQidata = WAVEC_SendQiData;
        pWM_handle->pATFunction->P_InquireQiStat = WAVEC_InquireQiStat;
        pWM_handle->pATFunction->P_InquireQiSack = WAVEC_InquireQiSack;
        pWM_handle->pATFunction->P_InquireQiRcvData = WAVEC_InquireQiRcvData;
        pWM_handle->pATFunction->P_SendQiClose = WAVEC_SetQiClose;
        pWM_handle->pATFunction->P_Set_QIDeact = WAVEC_SetQIDeact;
        pWM_handle->pATFunction->P_InquireQiLocIP = WAVEC_InquireQiLocIP;
        pWM_handle->pATFunction->P_Inquire_CellLoc = WAVEC_InquireCellLoc;
    }
    pWM_handle->pATFunction->P_Set_Qidnsip = WAVEC_SetQiDnsip;
    pWM_handle->pATFunction->P_InquireQIRegApp = WAVEC_InquireQIRegApp;

    if (NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_QIRegApp = SIM800_SetAPN;
        pWM_handle->pATFunction->P_Set_QIAct = SIM800_SetCIICR;
    }
    else
    {
        pWM_handle->pATFunction->P_Set_QIRegApp = WAVEC_SetQIRegApp;
        pWM_handle->pATFunction->P_Set_QIAct = WAVEC_SetQIAct;
    }
    
    pWM_handle->pATFunction->P_Set_QICsgp = WAVEC_SetQICsgp;

    pWM_handle->pATFunction->P_Set_QMoStat = WAVEC_SetQMoStat;
    pWM_handle->pATFunction->P_Set_QJamDet = WAVEC_SetQJamDet;

    if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_QCCINFOEn = UC15_SetQCCINFOEn;
        pWM_handle->pATFunction->P_Set_QURCCFG = UC15_SetQURCCFG;
    }

    pWM_handle->pATFunction->P_Set_FSendSimData = FIBOCOM_SendSIMData;
    
    pWM_handle->pATFunction->P_HoldAnd_Multiparty = WAVEC_HoldAndMultiparty;
    pWM_handle->pATFunction->P_Inquire_Subscriber = WAVEC_InquireSubscriber;
    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_List_CurCalls = C202_ListCurCalls;    
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_List_CurCalls = MC323_ListCurCalls;    
    }
    else if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_List_CurCalls = CM300_ListCurCalls;    
    }
    else
    {
        pWM_handle->pATFunction->P_List_CurCalls = WAVEC_ListCurCalls;    
    }

    pWM_handle->pATFunction->P_Facility_Lock = WAVEC_FacilityLock;
    pWM_handle->pATFunction->P_InqFacility_Lock = WAVEC_InqFacilityLock;
    pWM_handle->pATFunction->P_Calling_LineId = WAVEC_CallingLineId;
    pWM_handle->pATFunction->P_Check_CLIP = WAVEC_CheckCLIP;
    pWM_handle->pATFunction->P_Check_CLIR = WAVEC_CheckCLIR;
    pWM_handle->pATFunction->P_Set_CLIRActive = WAVEC_SetCLIRActive;
    pWM_handle->pATFunction->P_Set_CLIRDisable = WAVEC_SetCLIRDisable;
    pWM_handle->pATFunction->P_Dail_CLIR = WAVEC_DailCLIR;
    pWM_handle->pATFunction->P_Check_COLR = WAVEC_CheckCOLR;

    if(NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_SetSIMSlot = M10_SetSIMSlot;
    }
    
    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_SpeakerVol = M10_SetSpeakerVol;
    }
    else if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_SpeakerVol = CM300_SetSpeakerVol;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_SpeakerVol = MC323_SetSpeakerVol;
    }
    else if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_SpeakerVol = C202_SetSpeakerVol;
    }
    else if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_SpeakerVol = UC15_SetSpeakerVol;
    }
    else
    {
        pWM_handle->pATFunction->P_Set_SpeakerVol = WAVEC_SetSpeakerVol;
    }

    pWM_handle->pATFunction->P_Set_FreqBand = M10_SetFreqBand;
    pWM_handle->pATFunction->P_Set_QSFRParam = WAVEC_SetQSFRParam;

    
    pWM_handle->pATFunction->P_Report_MobileError = WAVEC_ReportMobileError;
    pWM_handle->pATFunction->P_Enter_Multiplex = WAVEC_EnterMultiplex;
    pWM_handle->pATFunction->P_Read_OperatorNames = WAVEC_ReadOperatorNames;
    
    if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_CurOperator = CM300_InquireCurOperator;
    }
    else if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_CurOperator = C202_InquireCurOperator;
    }    
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_CurOperator = MC323_InquireCurOperator;
    }        
    else if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Inquire_CurOperator = UC15_InquireCurOperator;
    }  
    else
    {
        pWM_handle->pATFunction->P_Inquire_CurOperator = WAVEC_InquireCurOperator;
    }
    
    pWM_handle->pATFunction->P_Inquire_BSInfo = WAVEC_InquireBSInfo;
    
    pWM_handle->pATFunction->P_Inquire_AvaiOperator = WAVEC_InquireAvaiOperator;
    pWM_handle->pATFunction->P_Select_Operator = WAVEC_SelectOperator;
    pWM_handle->pATFunction->P_Set_EngMode = WAVEC_SetEngMode;
    
    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Read_PbEntries = C202_ReadPbEntries;
    }    
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Read_PbEntries = MC323_ReadPbEntries;
    }
    else
    {
        pWM_handle->pATFunction->P_Read_PbEntries = WAVEC_ReadPbEntries;        
    }
    
    pWM_handle->pATFunction->P_Inquire_PbSpace = WAVEC_InquirePbSpace;
    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Write_PbEntry = C202_WritePbEntry;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Write_PbEntry = MC323_WritePbEntry;
    }
    else
    {
        pWM_handle->pATFunction->P_Write_PbEntry = WAVEC_WritePbEntry;
    }
    
    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_PinStatus = C202_InquirePinStatus;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_PinStatus = MC323_InquirePinStatus;
    }
    else if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_PinStatus = CM300_InquirePinStatus;
    }
    else if(NVM_GOIP_MODTYPE_MC600 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_PinStatus = MC600_InquirePinStatus;
    }    
    else
    {
        pWM_handle->pATFunction->P_Inquire_PinStatus = WAVEC_InquirePinStatus;
    }

    pWM_handle->pATFunction->P_Inquire_InitStatus = WAVEC_InquireInitStatus;
    
    pWM_handle->pATFunction->P_Enter_PIN = WAVEC_EnterPIN;
    pWM_handle->pATFunction->P_Change_Password = WAVEC_ChangePassword;
    pWM_handle->pATFunction->P_Network_Registration = WAVEC_NetworkRegistration;
    if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_NetworkRegStatus = CM300_InquireNetworkRegStatus;
    }
    else if(NVM_GOIP_MODTYPE_MC600 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_NetworkRegStatus = MC600_InquireNetworkRegStatus;
    }
    else if((NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_M26 == pWirelessChannelCtl->ModuleType) 
        || (NVM_GOIP_MODTYPE_EC20 == pWirelessChannelCtl->ModuleType) 
        || (NVM_GOIP_MODTYPE_L810 == pWirelessChannelCtl->ModuleType))
    {
        //pWM_handle->pATFunction->P_Inquire_NetworkRegStatus = FIBOCOM_InquireNetworkRegStatus;        
        pWM_handle->pATFunction->P_Inquire_NetworkRegStatus = WAVEC_InquireNetworkRegStatus;        
    }
    else
    {
        pWM_handle->pATFunction->P_Inquire_NetworkRegStatus = WAVEC_InquireNetworkRegStatus;
    }

    pWM_handle->pATFunction->P_Inquire_GNetworkRegStatus = FIBOCOM_InquireNetworkRegStatus;        

    pWM_handle->pATFunction->P_Set_CharacterSet = WAVEC_SetCharacterSet;

    if((NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Inquire_SignalQuality = C202_InquireSignalQuality;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Inquire_SignalQuality = MC323_InquireSignalQuality;
    }
    else
    {
        pWM_handle->pATFunction->P_Inquire_SignalQuality = WAVEC_InquireSignalQuality;
    }
    //weikk 2016.3.31 add XREG
    //if(NVM_GOIP_MODTYPE_L810 == pWirelessChannelCtl->ModuleType)
    {
         pWM_handle->pATFunction->P_Inquire_NetType = WAVEC_InquireNetType;
    }
    pWM_handle->pATFunction->P_Inquire_ADC = WAVEC_InquireADC;
    pWM_handle->pATFunction->P_Inquire_MMAD = WAVEC_InquireMMAD;
    
    pWM_handle->pATFunction->P_Inquire_SideTone = M10_InquireSideTone;

    pWM_handle->pATFunction->P_Inquire_CallStatus = Quectel_InquireCallStatus;
    
    pWM_handle->pATFunction->P_Set_SSNote = WAVEC_SetSSNote;
    pWM_handle->pATFunction->P_Send_USSD = WAVEC_SendUSSD;
    pWM_handle->pATFunction->P_Send_UssdData = WAVEC_SendUssdData;
    pWM_handle->pATFunction->P_Cancel_UssdData = WAVEC_CancelUssdData;
    
    if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Dial_DTMF = CM300_DialDTMF;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Dial_DTMF = MC323_DialDTMF;
    }
    else
    {
        pWM_handle->pATFunction->P_Dial_DTMF = WAVEC_DialDTMF;
    }

    pWM_handle->pATFunction->P_Set_DTMFPrd = C202_SetDTMFPrd;
    
    /*extended AT command*/
    pWM_handle->pATFunction->P_Set_Cancel = WAVEC_SetCancel;
    pWM_handle->pATFunction->P_Cancel_Dial = WAVEC_CancelDial;
    pWM_handle->pATFunction->P_Set_Alarm = WAVEC_SetAlarm;
    pWM_handle->pATFunction->P_Field_TestReport = WAVEC_FieldTestReport;
    pWM_handle->pATFunction->P_Read_SMSUnchgStat = WAVEC_ReadSMSUnchgStat;
    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_SwitchOff_Module = C202_SwitchOffModule;
    }
    else
    {
        pWM_handle->pATFunction->P_SwitchOff_Module = WAVEC_SwitchOffModule;
    }

    
    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_MicGainLevel = M10_SetMicGainLevel;
    }
    else if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_MicGainLevel = UC15_SetMicGainLevel;
    }    
    else if(NVM_GOIP_MODTYPE_CM300 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_MicGainLevel = CM300_SetMicGainLevel;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_MicGainLevel = MC323_SetMicGainLevel;
    }
    else if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_MicGainLevel = C202_SetMicGainLevel;
    }
    else
    {
        pWM_handle->pATFunction->P_Set_MicGainLevel = WAVEC_SetMicGainLevel;
    }
    
    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_SetAudioPath = M10_SetAudioPath;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_SetAudioPath = MC323_SetAudioPath;
    }
    else
    {
        pWM_handle->pATFunction->P_SetAudioPath = WAVEC_SetAudioPath;
    }

    if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_SetAudioMode = UC15_SetAudioMode;
    }
    
    if(NVM_GOIP_MODTYPE_S5320 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_SetVoiceHangupCtrl = S5320_SetVoiceHangupCtrl;
    }
    
    pWM_handle->pATFunction->P_SetAudioMute = WAVEC_SetAudioMute;
    pWM_handle->pATFunction->P_PlayTone = WAVEC_PlayTone;
    pWM_handle->pATFunction->P_Inquire_HeadsetStatus = WAVEC_InquireHeadsetStatus;
    pWM_handle->pATFunction->P_Inquire_HeadsetBtnStatus = WAVEC_InquireHeadsetBtnStatus;
    pWM_handle->pATFunction->P_Inquire_BatteryLevel = WAVEC_InquireBatteryLevel;
    pWM_handle->pATFunction->P_Inquire_PinCount = WAVEC_InquirePinCount;
    pWM_handle->pATFunction->P_Inquire_PukCount = WAVEC_InquirePukCount;
    pWM_handle->pATFunction->P_Dump_BroadCast = WAVEC_DumpBroadCast ;
    pWM_handle->pATFunction->P_Set_RingerSoundLevel = WAVEC_SetRingerSoundLevel;
    pWM_handle->pATFunction->P_Set_AudioOutput = WAVEC_SetAudioOutput;
    pWM_handle->pATFunction->P_Set_SmsFullUrc = WAVEC_SetSmsFullUrc;
    pWM_handle->pATFunction->P_Query_ModuleState = WAVEC_QueryModuleState;
    pWM_handle->pATFunction->P_Inquire_COLP = WAVEC_InquireCOLP;
    pWM_handle->pATFunction->P_Set_ColpStatus = WAVEC_SetColpStatus;
    pWM_handle->pATFunction->P_Check_LatestURCEvent = WAVEC_CheckLatestURCEvent;
    pWM_handle->pATFunction->P_Control_URCIndication = WAVEC_ControlURCIndication;
    pWM_handle->pATFunction->P_Control_ModuleDriver = WAVEC_ControlModuleDriver;
    pWM_handle->pATFunction->P_Query_URCIndication = WAVEC_QueryURCIndication;
    pWM_handle->pATFunction->P_Mobile_EquipmentEvent = WAVEC_MobileEquipmentEvent;
    pWM_handle->pATFunction->P_Send_AT = WAVEC_SendAT;

    if((NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType) || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_PwrCtrl = UC15_PwrCtrl;
    }
    else
    {
        pWM_handle->pATFunction->P_PwrCtrl = WAVEC_PwrCtrl;
    }
    
    pWM_handle->pATFunction->P_Send_ATOrigin = WAVEC_SendATOrigin;
    
    pWM_handle->pATFunction->P_Set_MSStorageType = WAVEC_SetMSStorageType;
    pWM_handle->pATFunction->P_Set_ExtSMSStorageStatus = WAVEC_SetExtSMSStorageStatus;
    pWM_handle->pATFunction->P_Set_ModuleSleep = WAVEC_SetModuleSleep;
    pWM_handle->pATFunction->P_Get_ModemVersionNum = WAVEC_GetModemVersionNum;
    pWM_handle->pATFunction->P_Set_ChargingCurrent = WAVEC_SetChargingCurrent;
    
    pWM_handle->pATFunction->P_ConfigURCMode = WAVEC_ConfigURCMode;
    pWM_handle->pATFunction->P_XDRV_URCControl = WAVEC_XDRVURCControl;
    pWM_handle->pATFunction->P_XDRV_AudioControl = WAVEC_XDRVAudioControl;
    pWM_handle->pATFunction->P_Set_TraceOutput = WAVEC_SetTraceOutput;
    pWM_handle->pATFunction->P_Get_ModemExcLog = WAVEC_GetModemExcLog;
    pWM_handle->pATFunction->P_XDRV_ControlNewSMSStatus = WAVEC_XDRVControlNewSMSStatus;
    pWM_handle->pATFunction->P_XDRV_ControlAudioLoopTest = WAVEC_XDRVControlAudioLoopTest;
    pWM_handle->pATFunction->P_NewMsgNeedAck = WAVEC_NewMsgNeedAck;
    pWM_handle->pATFunction->P_ATDReturnDirectly = WAVEC_ATDReturnDirectly;
    pWM_handle->pATFunction->P_CheckATResult = WAVEC_CheckATResult;
    pWM_handle->pATFunction->P_CheckUrcData = WaveC_CheckUrcData;
    pWM_handle->pATFunction->P_InitBeforePIN = WAVEC_InitBeforePIN;
    pWM_handle->pATFunction->P_InitAfterPIN = WAVEC_InitAfterPIN;
    
    pWM_handle->pATFunction->P_STKSetProfile = WAVEC_STKSetProfile;
    pWM_handle->pATFunction->P_STKSetAutoRsp = WAVEC_STKSetAutoRsp;
    pWM_handle->pATFunction->P_STKSetTR = WAVEC_STKSetTR;
    
    pWM_handle->pATFunction->P_STKSelectMainMenu = WAVEC_STKSelectMainMenu;
    pWM_handle->pATFunction->P_STKResponse = WAVEC_STKResponse;
    pWM_handle->pATFunction->P_STKDetailInfoInURC = WAVEC_STKDetailInfoInURC;
    pWM_handle->pATFunction->P_STKGetDetailInfo = NULL;
    
    pWM_handle->pATFunction->P_InquireGprsState = WAVEC_InquireGprsState;
    pWM_handle->pATFunction->P_EstablishDataConnWithGPRS = WAVEC_EstablishDataConnWithGPRS;
    pWM_handle->pATFunction->P_EstablishDataConnWithCSD = WAVEC_EstablishDataConnWithCSD;

    if(NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_EstablishDataConnWithCSD = C202_EstablishDataConnWithCSD;
    }
    else
    {
        pWM_handle->pATFunction->P_EstablishDataConnWithCSD = WAVEC_EstablishDataConnWithCSD;
    }


    pWM_handle->pATFunction->P_ReleaseDataConn = WAVEC_ReleaseDataConn;
    pWM_handle->pATFunction->P_SuspendDataConn = WAVEC_SuspendDataConn;
    pWM_handle->pATFunction->P_ResumeDataConn = WAVEC_ResumeDataConn;
    pWM_handle->pATFunction->P_SetDataConnMode = WAVEC_SetDataConnMode;

    if((NVM_GOIP_MODTYPE_C202 == pWirelessChannelCtl->ModuleType)
        || (NVM_GOIP_MODTYPE_MC600 == pWirelessChannelCtl->ModuleType))
    {
           pWM_handle->pATFunction->P_ResetWM = C202_ResetByAT;
    }
    else if(NVM_GOIP_MODTYPE_MC323 == pWirelessChannelCtl->ModuleType)
    {
           pWM_handle->pATFunction->P_ResetWM = MC323_ResetByAT;
    }
    else if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
            ||(NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_L810 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_EC20 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_H330 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_SIM800 == pWirelessChannelCtl->ModuleType)
            || (NVM_GOIP_MODTYPE_M26 == pWirelessChannelCtl->ModuleType))
    {
           pWM_handle->pATFunction->P_ResetWM = M10_ResetByAT;
    }    
    else
    {
           pWM_handle->pATFunction->P_ResetWM = WAVEC_ResetWM;
    }
    
    pWM_handle->pATFunction->P_IgniteWM = WAVEC_IgniteWM;
    pWM_handle->pATFunction->P_Reset = WAVEC_Reset;

    if(NVM_GOIP_MODTYPE_2406 == pWirelessChannelCtl->ModuleType)
    {
        pWM_handle->pATFunction->P_Set_BatteryParam = WAVEC_SetBatteryParam;
    }

    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_ToneDetParam = WAVEC_SetToneDetParam;
    }    

    pWM_handle->pATFunction->P_Set_DAIParam = WAVEC_SetDAIParam;
    
    pWM_handle->pATFunction->P_Set_VoiceGain = WAVEC_SelectVoiceGain;
    
    if((NVM_GOIP_MODTYPE_M10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_U10 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_UC15 == pWirelessChannelCtl->ModuleType)
       || (NVM_GOIP_MODTYPE_UC20 == pWirelessChannelCtl->ModuleType))
    {
        pWM_handle->pATFunction->P_Set_SideTone = M10_SetSideTone;
    }
    else
    {
        pWM_handle->pATFunction->P_Set_SideTone = WAVEC_SetSideTone;
    }
    
    pWM_handle->pATFunction->P_Save_Configuration = WAVEC_SaveConfiguration;

    /* Kinson temporary
    pWM_handle->pATFunction->P_IgniteModule = WAVEC_IgniteModule;
    pWM_handle->pATFunction->P_IgniteResult = WAVEC_IgniteResult;
    pWM_handle->pATFunction->P_PowerOffModule = WAVEC_PowerOffModule;
    pWM_handle->pATFunction->P_PowerOffResult = WAVEC_PowerOffResult;
    pWM_handle->pATFunction->P_ResetModule = WAVEC_ResetModule;
    pWM_handle->pATFunction->P_ResetResult = WAVEC_ResetResult;
    pWM_handle->pATFunction->P_SleepModule = WAVEC_SleepModule;
    pWM_handle->pATFunction->P_WakeupModule = WAVEC_WakeupModule;
    */
}
