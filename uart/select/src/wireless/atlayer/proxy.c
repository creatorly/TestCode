
#include "wireless_list.h"
#include "proxy.h"
#include "proxycom.h"
#include "svrcom.h"

#include "../wireless_common.h"
#include "../wireless_debug.h"
#include "../wireless_interface_inc.h"

typedef struct tagRIL{
    URCCALLBACK        fCallback;
    _UINT32            hLine;
    _UINT32            customParam;
    struct          list_head list;
}T_RIL;

typedef struct _REQUEST{
    RESCALLBACK        fCallback;
}T_REQUEST;

extern WIRELESSROOTST stWirelessRootSt;

/**
  *@brief 启动RIL
  *@param _UINT32 ChnlNum    [in] 设定无线模块的通道个数，cmux模式下通常不止一个通道，非cmux模式只有一个通道
  *@param _USHORT16 *ChID [in] 通道ID数组，保存各个通道的ID
  *@param _UINT32 Type [in] 无线模块的类型. 1: WAVECOM
  *@param _UINT32 *phATServer    [out] Radio server的句柄.该句柄标识一个模块，多模手机需要调用该接口多次。
  *@return    _UINT32 0: 启动成功。
*/
_UINT32 RIL_Setup(_UINT32 ChnlNum, _UINT32* ChID, _UINT32 Type, _UINT32* phATServer)
{
    T_SETUP req;
    T_MESSAGE *pMsg = NULL;
    _UINT32    i = 0;
    
    req.type = ECALLSETUP;
    req.ChnlNum = ChnlNum;
    for(i = 0; i < ChnlNum; i++)
    {
        req.ChID[i] = ChID[i];
    }
    req.value = Type;

    ProxySend(ChID[0], (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChID[0]);
    
    *phATServer = pMsg->param.reply.paramter;
    return pMsg->param.reply.returnValue;
}

/**
  *@brief 结束所有启动的Radio server
  *@param
  *@return    _UINT32 0: 结束成功。
*/
_UINT32 RIL_Stop(_VOID)
{
    T_SETUP req;
    T_MESSAGE *pMsg;
    
    req.type = ECALLSTOP;

    ProxySend(0, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(0); /* temp */
    
    return pMsg->param.reply.returnValue;
}

/**
  *@brief 更新Radio server的通道信息
  *@param HRIL hRIL [in] 任意一个在将要更新的Radio server上打开的RIL句柄
  *@param _UINT32 ChnlNum    [in] 设定无线模块的通道个数，cmux模式下通常不止一个通道，非cmux模式只有一个通道
  *@param _USHORT16 *ChID [in] 通道ID数组，保存各个通道的ID

  *@return    _UINT32 0: 更新成功。
*/
_UINT32 RIL_UpdateChannel(_UINT32 ChannelID, HRIL hRIL, _UINT32 ChnlNum, _USHORT16* ChID)
{    
    T_SETUP req;
    T_MESSAGE *pMsg;
    _UINT32    i;
    
    req.type = ECALLUPDATECHNL;
    req.ChnlNum = ChnlNum;
    for(i = 0; i < ChnlNum; i++)
    {
        req.ChID[i] = ChID[i];
    }
    req.value = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 在某个radio server上打开一个RIL句柄
  *@param _UINT32 eventType [in] 指定该RIL希望接收的消息类型，可以是RIL_URC_TYPE的任意组合。
  *@param _UINT32 hATServer [in] 指定在哪个radio server的句柄
  *@param URCCALLBACK fCallback [in] 指定接收radio server消息的回调函数
  *@param _UINT32 customPara [in] 指定附加信息，该信息会在回调函数中返回
  *@param HRIL *phRIL [out] 返回的RIL句柄 
  *@return    _UINT32 0: 打开成功
*/
_UINT32 RIL_Open(_UINT32* ChID, _UINT32 eventType, _UINT32 hATServer, 
                 URCCALLBACK fCallback, _UINT32 customParam, 
                 HRIL* phRIL)
{
    T_RIL *pRIL;
    T_OPENLINE req;
    T_MESSAGE *pMsg;

    pRIL = (T_RIL*)malloc(sizeof(T_RIL));

    pRIL->fCallback = fCallback;
    pRIL->customParam = customParam;

    req.type = ECALLOPENLINE;
    req.hRIL = (_UINT32)pRIL;
    req.eventType = eventType;
    req.hATServer = hATServer;


    ProxySend(ChID[0], (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChID[0]);

    pRIL->hLine = pMsg->param.reply.paramter;
    *phRIL = pRIL;
    return pMsg->param.reply.returnValue;
}

/**
  *@brief 关闭一个RIL句柄
  *@param HRIL hRIL [in] 指定要关闭的RIL 句柄
  *@return    _UINT32 0: 关闭成功
*/
_UINT32 RIL_Close(HRIL hRIL)
{
    T_CLOSELINE req;
    T_MESSAGE *pMsg;
    
    req.type = ECALLCLOSELINE;
    req.hLine = hRIL->hLine;

    ProxySend(0, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(0);/* temp */

    WIRELESS_FREE(hRIL);
    return pMsg->param.reply.returnValue;
}

/**
  *@brief 在指定的RIL上拨号
  *@param HRIL hRIL [in] 指定要拨号的RIL 句柄
  *@param const _UCHAR8* lpszAddress [in] 要拨打的电话号码
  *@param _UINT32 dwType [in] reserved to be use
  *@param _UINT32 dwOptions [in] RIL_DATA_CLASS类型
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _UINT32 *callHandle [out] 该电话对应的HCALL句柄 
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_MakeCall(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* lpszAddress, 
                      _UINT32 dwType, _UINT32 dwOptions, 
                      RESCALLBACK fCallback, _UINT32* callHandle)
{
    T_MAKECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = MakeCall;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.lpszAddress = lpszAddress;
    req.dwType = dwType;
    req.dwOptions = dwOptions;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    *callHandle = pMsg->param.reply.paramter;
    return pMsg->param.reply.returnValue;
}

/**
  *@brief 拒接来电
  *@param HRIL hRIL [in] 指定要拒接来电的RIL 句柄
  *@param _UINT32 hCall [in] 指定要拒接的CALL 句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_RejectCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLREJECTCALL;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 接听来电
  *@param HRIL hRIL [in] 指定要接听来电的RIL 句柄
  *@param _UINT32 hCall [in] 指定要接听的CALL 句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_AnswerCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLANSWERCALL;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 挂断所有电话
  *@param HRIL hRIL [in] 指定要接听来电的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_HangupCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    req.type = ECALLHANGUPCALL;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    WIRELESS_INFO("channel %d RIL_HangupCall\r\n", ChannelID);             
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 取消拨号
  *@param HRIL hRIL [in] 指定要取消拨号的RIL 句柄
  *@param _UINT32 hCall [in] 指定要取消拨号的Call句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_CancelDial(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLCANCELDIAL;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 切换所有hold/active电话的状态
  *@param HRIL hRIL [in] 指定要切换电话的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SwapCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLSWAPCALL;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 电话会议
  *@param HRIL hRIL [in] 指定要执行电话会议的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ConferenceCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLCONFERENCE;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 电话会议状态下结束单路
  *@param HRIL hRIL [in] 指定要执行结束单路的RIL 句柄
  *@param _UINT32 hCall [in] 指定要结束电话的call句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EndSingleCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLENDSINGLE;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 结束所有active状态的电话
  *@param HRIL hRIL [in] 指定要执行结束active操作的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EndActiveCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLENDACTIVE;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 结束所有hold状态的电话
  *@param HRIL hRIL [in] 指定要执行结束hold操作的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EndHoldCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLENDHOLD;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 从active conference call中分离一路，单方通话，同时hold其它电话
  *@param HRIL hRIL [in] 指定要执行结束hold操作的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SplitFromConference(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
        
    req.type = ECALLSPLITFROMCONFER;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_UpdateCurCallList(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
        
    req.type = ECALLCURCALLLIST;
    req.hRequest = 0;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 查询电话的详细信息
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _UINT32 hCall [in] 指定要查询的CALL句柄，如果该参数为0，表示要查询所有电话的详细信息
  *@param RIL_CLCC_ENTRY *pCLCC [out] 保存查询到的电话信息
  *@param _UINT32 *number [out] 保存查询到的电话数目
  *@return    _UINT32 0: 执行成功
*/
_UINT32 RIL_GetCallInfo(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, 
                        RIL_CLCC_ENTRY* pCLCC, _UINT32* number)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    
    req.type = ECALLGETCALLINFO;
    req.hRequest = 0;
    req.hLine = hRIL->hLine;
    req.hCall = hCall;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    if(pMsg->param.reply.returnValue == 0 && pMsg->param.reply.totalSize > 0)
    {
        memcpy(pCLCC, pMsg->param.reply.pBuff, pMsg->param.reply.totalSize);
        /*WIRELESS_FREE(pMsg->param.reply.pBuff);*/
    }
    *number = pMsg->param.reply.usedSize;

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 查询指定类型的电话的个数
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RIL_CALL_STATUS status [in] 电话类型，如果该参数为RIL_CALL_STATUS_NUM，表示要查询所有电话数目
  *@param _UINT32 *callQty [out] 保存查询到的电话数目
  *@return    _UINT32 0: 执行成功
*/
_UINT32 RIL_GetCallQty(_UINT32 ChannelID, HRIL hRIL, RIL_CALL_STATUS status, _UINT32* callQty)
{
    T_OPERATECALL req;
    T_MESSAGE *pMsg;
    
    req.type = ECALLGETCALLQty;
    req.hRequest = 0;
    req.hLine = hRIL->hLine;
    req.hCall = status;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    *callQty = pMsg->param.reply.paramter;

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 发送短信
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param const _UCHAR8* pPDU [in] 打包好的PUD数据包
  *@param _UINT32 option [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SendMsg(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, RESCALLBACK fCallback)
{
    T_SENDMSG req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST   pWrlChannelCtl; 

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWrlChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

    if(NVM_GOIP_MODTYPE_MC323 == pWrlChannelCtl->ModuleType)
    {
        req.type = ECALLSENDMSG_MC323;
    }
    else
    {
        req.type = ECALLSENDMSG;
    }
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.pPDU = pPDU;
    req.option = option;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_QiSendData(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pDataBuf, _UINT32 DataLen, RESCALLBACK fCallback)
{
    T_SENDMSG req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST   pWrlChannelCtl; 

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWrlChannelCtl = &pWirelessRootSt->stChannelSt[ChannelID];

    req.type = EQISENDDATA;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.pPDU = pDataBuf;
    req.option = DataLen;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_SendMsgByTxt(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pTxt, _UINT32 uStringLen, const _UCHAR8* pNumber, RESCALLBACK fCallback)
{
    T_SENDMSG req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLSENDMSGBYTXT;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.pPDU = pTxt;
    req.PDULen = uStringLen;
    req.pNumber = pNumber;
    req.option = 0;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 发送短信给CDMA
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param const _UCHAR8* pPDU [in] 打包好的PUD数据包
  *@param _UINT32 option [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SendMsg_CDMA(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, RESCALLBACK fCallback)
{
    T_SENDMSG req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLSENDMSG_CDMA;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.pPDU = pPDU;
    req.option = option;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 保存短信到SIM卡
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param const _UCHAR8* pPDU [in] 打包好的PUD数据包
  *@param _UINT32 option [in] Reserved to be use
    *@param _BOOL read_flag [in] EOS_TRUE: 短信已读；EOS_FALSE:短信未读
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
    *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SaveMsgToSIM(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, 
                         _UINT32 option, _BOOL read_flag, 
                         _SHORT16 PDUlen, RESCALLBACK fCallback)
{
    T_SAVEMSG req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLSAVEMSG;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.pPDU = pPDU;
    req.option = option;
    req.read_flag = (_UCHAR8)read_flag;
    req.PDUlen = PDUlen;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 读SIM卡短信
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadSMS(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback)
{
    T_ONEU16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLREADMSG;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = index;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ReadSMSByTxt(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback)
{
    T_ONEU16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLREADMSGBYTXT;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = index;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


/**
  *@brief 读SIM卡短信
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadSMS_CDMA(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback)
{
    T_ONEU16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLREADMSG_CDMA;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = index;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


/**
  *@brief 读SIM卡短信,不改变短信在SIM卡上的状态
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadSMSUnchgStat(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback)
{
    T_ONEU16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLREADMSGUNCHGSTAT;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = index;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
  *@brief 设置短信存储状态
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _BOOL bFull [in] EOS_FALSE :短信未存满，EOS_TRUE:短信已经存满
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetSMSStorageStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bFull)
{
    T_ONEU8PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETEXTSMSSTORAGESTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.uValue = (_UCHAR8)bFull;
    
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


/**
  *@brief 删除SIM卡上的短信
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_DeleteSMS(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback)
{
    T_ONEU16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    req.type = ECALLDELETEMSG;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = index;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}
/**
  *@brief 设置小区广播
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SelectBroadCast(_UINT32 ChannelID, HRIL hRIL, _CHAR8* strMode, 
                            _CHAR8* strMids, _CHAR8* strDcss, RESCALLBACK fCallback)
{
    T_THREESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SELECTBROADCAST;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue1 = strMode;
    req.strValue2 = strMids;
    req.strValue3 = strDcss;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/**
*@brief 得到模块的睡眠状态
*@param [in] HRIL hRIL
*@return _BOOL
*@retval EOS_TRUE: sleeping; EOS_FALSE: wakeup
*/
_BOOL RIL_GetSleepState(_UINT32 ChannelID, HRIL hRIL)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    
    req.type = GETSLEEPSTATE;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return (_BOOL)pMsg->param.reply.returnValue;
}

_UINT32 RIL_Attention(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
        
    req.type = ECALLATTENTION;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCurOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
        
    req.type = INQUIRECUROPERATOR;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

/* BS: Base Station    */
_UINT32 RIL_InquireBSInfo(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
        
    req.type = INQUIREBSINFO;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    
    pRequest->fCallback = fCallback;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireClock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIRECLOCK;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCommandEcho(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bOnOff)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCOMMANDECHO;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = bOnOff;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SendAT(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }

    pRequest->fCallback = fCallback;

    req.type = SENDAT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ModulePwrCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }

    pRequest->fCallback = fCallback;

    req.type = PWRCTRL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetProductID(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETPRODUCTID;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetResultPresentation(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETRESULTPRESENTATION;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetResultFormat(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETRESULTFORMAT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetManufDefault(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETMANUFDEFAULT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireIMEI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREIMEI;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCCID(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = INQUIRECCID;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireIMSI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = INQUIREIMSI;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetIMEI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strIMEI)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETIMEI;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strIMEI;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_LockBS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strBSIndex)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = LOCKBS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strBSIndex;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_SetBaudrate(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strRate)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = SETBAUDRATE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strRate;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREBAND;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetNetBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMode)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETNETBAND;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strMode;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetCurrentAcm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETCURRENTACM;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ResetAcm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strPasswd)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = RESETACM;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strPasswd;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetAcmMax(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETACMMAX;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetAcmMax(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,
                            _CHAR8* strAcmax, _CHAR8* strPasswd)
{
    T_TWOSTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETACMMAX;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue1 = strAcmax;
    req.strValue2 = strPasswd;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetCurCallMeter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETCURCALLMETER;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetClock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_SYSTIME *SysTime)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    PWIRELESSROOTST pWirelessRootSt;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCLOCK;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = (_UINT32)SysTime;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetExtendedError(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETEXTENDEDERROR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetFunctionality(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETFUNCTIONALITY;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetSimVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETSIMVOL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetFunctionality(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strFun)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETFUNCTIONALITY;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strFun;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetSimVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMod)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETSIMVOL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strMod;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetRingBack(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETRINGBACK;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strEn;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetVBatt(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETVBATT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strEn;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetNWScanMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETNWSCANMODE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strStr;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetOpenTCPUDP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETOPENTCPUDP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strStr;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireQiStat(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREQISTAT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireQiSack(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREQISACK;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireQiLocIP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREQILOCIP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireQiRcvData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREQIRCVDATA;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQiClose(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQICLOSE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireQIRegApp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREQIREGAPP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQIRegApp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQIREGAPP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strStr;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQICsgp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQICSGP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strStr;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQIAct(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQIACT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQIDeact(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQIDEACT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQindi(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQINDI;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQidnsip(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQIDNSIP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCnetScan(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCNETSCAN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCellLoc(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIRECELLLOC;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQFun(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strFun)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQFUN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strFun;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQMoStat(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQMOSTAT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strEn;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQCCINFOEn(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQCCINFOEN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strEn;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_FSendSimData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strSimData)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETFSENDSIMDATA;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strSimData;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetQURCCFG(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQURCCFG;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_SetQJamDet(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQJAMDET;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strEn;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireSubscriber(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIRESUBSCRIBER;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetSIMSlot(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 simslot)
{
    T_ONES16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETSIMSLOT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = simslot;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_SetSpeakerVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 volume)
{
    T_ONES16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETSPEAKERVOL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = volume;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_SetFreqBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 bandindex)
{
    T_ONES16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETFREQBAND;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = bandindex;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_SetQSFR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 sfrIndex)
{
    T_ONES16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETQSFR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = sfrIndex;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_ReportMobileError(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = REPORTMOBILERROR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_EnterMultiplex(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ENTERMULTIPLEX;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_ReadOperatorNames(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = READOPERATORNAMES;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_InquireAvaiOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREAVAIOPERATOR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_SelectOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                            RIL_NET_MODE mode, _CHAR8* strOper)
{
    T_TWOU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SELECTOPERATOR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value1 = mode;
    req.value2 = (_UINT32)strOper;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetEngMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _UINT32 mode, _UINT32 dump)
{
    T_TWOU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETENGMODE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value1 = mode;
    req.value2 = dump;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquirePinStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREPINSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireInitStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREINITSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_EnterPIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                     _CHAR8* strPIN, _CHAR8* strNewPIN)
{
    T_TWOSTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ENTERPIN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue1 = strPIN;
    req.strValue2 = strNewPIN;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ChangePassword(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, 
                           _CHAR8* strOldPwd, _CHAR8* strNewPwd)
{
    T_THREEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CHANGEPASSWORD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value1 = fac;
    req.value2 = (_UINT32)strOldPwd;
    req.value3 = (_UINT32)strNewPwd;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_NetworkRegistration(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = NETWORKREGISTRATION;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireNetRegStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIRENETREGSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireGNetRegStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREGNETREGSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCharacterSet(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strChset)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCHARACTERSET;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strChset;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireSignalQuality(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIRESIGNALQUALITY;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

//weikk 2016.3.31 add XREG
_UINT32 RIL_InquireNetType(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIRENETTYPE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}
//weikk 20161026 add ADC
_UINT32 RIL_InquireADC(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIREADC;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}
//weikk 20160713 add MMAD
_UINT32 RIL_InquireMMAD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIREMMAD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}



_UINT32 RIL_InquireSideTone(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIRESIDETONE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCallStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = INQUIRECALLSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetSSNote(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,
                      _CHAR8* strCSSI, _CHAR8* strCSSU)
{
    T_TWOSTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    
    pRequest->fCallback = fCallback;

    req.type = SETSSNOTE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue1 = strCSSI;
    req.strValue2 = strCSSU;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SendUSSD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                        _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs)
{
    T_THREEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SENDUSSD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value1 = (_UINT32)strN;
    req.value2 = (_UINT32)strStr;
    req.value3 = (_UINT32)strDcs;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SendUssdData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strUssdString)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SENDUSSDDATA;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strUssdString;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);
    
    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SendATOrigin(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strATString)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SENDATORIGIN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strATString;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_CancelUssdData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = CANCELUSSDDATA;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_DialDTMF(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strDtmfStr)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = DIALDTMF;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strDtmfStr;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetDTMFPrd(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strDtmfStr)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = SETDTMFPRD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strDtmfStr;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetAlarm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CALA_ALARM* alarmData)
{
    T_SETALARM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = SETALARM;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.alarmData = alarmData;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_FieldTestReport(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMode)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = FIELDTESTREPORT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strMode;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SwitchOffModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SWITCHOFFMODULE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetMicGainLevel(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 volume)
{
    T_ONEU8PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETMICGAINLEVEL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.uValue = volume;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetAudioPath(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_AUDIO_PATH_TYPE Mode)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CONFIGUREHANDFREE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = Mode;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetAudioMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_AUDIO_MOD_TYPE Mode)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CONFIGUREAUDMOD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = Mode;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetVoiceHangUpCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bEnable)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = VOICEHANGUPCTRL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = bEnable;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetAudioMute(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bMute)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETAUDIOMUTE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = bMute;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_PlayTone(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                     _USHORT16 type, _USHORT16 volume)
{
    T_TWOU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = PLAYTONE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value1 = type;
    req.value2 = volume;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireHeadsetStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREHEADSETSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireHeadsetBtnStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREHEAdSETBTNSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_InquireBatteryLevel(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREBATTERYLEVEL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquirePinCount(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,_UCHAR8 type)
{
    T_ONEU8PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREPINCOUNT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.uValue = type; 
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquirePukCount(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREPUKCOUNT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_DumpBroadCast(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = DUMPBROADCAST;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetRingerSoundLevel(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 level)
{
    T_ONES16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETRINGERSOUNDLEVEL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = level;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetAudioOutput(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strAudioParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETAUDIOOUTPUT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strAudioParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_QueryModuleState(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = QUERYMODULESTATE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_CheckLatestURCEvent(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strIndex)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CHECKLATESTURCEVENT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strIndex;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ControlURCIndication(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CONTROLURCINDICATION;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ControlModuleDriver(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _UCHAR8 mode)
{
    T_ONEU8PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CONTROLMODULEDRIVER;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.uValue = mode;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_QueryURCIndication(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = QUERYURCINDICATION;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_MobileEquipmentEvent(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = MOBILEEQUIPMENTEVENT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetMSStorageType(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strService)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETMSSTORAGETYPE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strService;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetModuleSleep(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,_UCHAR8 enable)
{
    T_ONEU8PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETMODULESLEEP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.uValue = (_UCHAR8)enable;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetModemVersionNum(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETMODEMVERSIONNUM;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetChargingCurrent(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStep)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCHARGINGCURRENT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strStep;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ConfigURCMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_URC_MODE mode)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETINDICATIONCONTROL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = mode;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;

}

_UINT32 RIL_XDRVURCControl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_sXDRV_TYPE *xdrvType)
{
    T_XDRVURCCONTROL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = XDRVURCCONTROL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.xdrvType = xdrvType;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_XDRVAudioControl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                    RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue)
{
    T_XDRVAUDIOCONTROL req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = XDRVAUDIOCONTROL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.audCtrlType = audCtrlType;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetTraceOutput(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bOnOff)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETTRACEOUTPUT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = bOnOff;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_GetModemExcLog(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = GETMODEMEXCLOG;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_XDRVControlAudioLoopTest(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL enable)
{
    T_ONEU8PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = XDRVCONTROLAUDIOLOOPTEST;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.uValue = (_UCHAR8)enable;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCallForward(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CCFC_FWDCALLS *cfData)
{
    T_CALLFORWARD req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CALLFORWARD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.cfData = cfData;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCallFwd(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class)
{
    T_TWOU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIRECALLFWD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value1 = reason;
    req.value2 = (_UINT32)Class;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCallWait(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass)
{
    T_CALLWAIT req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCALLWAIT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.bURCEnable = bURCEnable;
    req.bEnable = bEnable;
    req.eClass = eClass;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCallWait(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_DATA_CLASS eClass)
{
    T_INQUIRECALLWAIT req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIRECALLWAIT;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.eClass = eClass;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_FacilityLock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                         RIL_FACILITY fac, _BOOL bLock, 
                         _CHAR8* strPasswd, RIL_DATA_CLASS eClass)
{
    T_FACILITYLOCK req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = FACILITYLOCK;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.fac = fac;
    req.bLock = bLock;
    req.strPasswd = strPasswd;
    req.eClass = eClass;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireFacilityLock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                                RIL_FACILITY fac, RIL_DATA_CLASS eClass)
{
    T_FACILITYLOCK req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREFACILITYLOCK;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.fac = fac;
    req.eClass = eClass;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_CallingLineId(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CALLINGLINEID;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_CheckCLIP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CHECKCLIP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_CheckCLIR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CHECKCLIR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCLIRActive(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCLIRACTIVE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetCLIRDisable(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCLIRDISABLE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_DailCLIR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = DAILCLIR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_CheckCOLR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = CHECKCOLR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireCOLP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIRECOLP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetColpStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETCOLPSTATUS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strValue;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ReadPbEntries(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,
                          RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end)
{
    T_READPBENTRIES req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = READPBENTRIES;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.storage = storage;
    req.begin = begin;
    req.end = end;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquirePbSpace(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = INQUIREPBSPACE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = storage;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}


_UINT32 RIL_WritePbEntry(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                         RIL_PB_STORAGE storage, _SHORT16 index, 
                         _CHAR8* strNumber, _USHORT16* strText)
{
    T_WRITEPBENTRY req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;
    
    req.type = WRITEPBENTRY;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = storage;
    req.index = index;
    req.strNumber = strNumber;
    req.strText = strText;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_DelPbEntry(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                       RIL_PB_STORAGE storage, _SHORT16 index)
{
    T_WRITEPBENTRY req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = DELETEPBENTRY;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = storage;
    req.index = index;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireSmsSpace(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_MSG_STORAGE storage)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    if(pRequest == NULL)
    {
        WIRELESS_ERROR(WIRELESS_ERRORNO, "Wireless Malloc(%d) Failed\r\n", (sizeof(T_REQUEST)));
        return EOS_ERROR;
    }
    pRequest->fCallback = fCallback;

    req.type = INQUIRESMSSTORAGE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = storage;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ListSMS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_SMS_MODE stat)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = LISTSMS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = stat;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireSmsCenter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREMSGCENTER;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_NewMsgStoreMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_STORENEWMSG_MODE mode)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = NEWMSGSTOREMODE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = mode;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SetSmsCenter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strSca)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SETMSGCENTER;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strSca;
    
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InitBeforePIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INITBEFOREPIN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InitAfterPIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INITAFTERPIN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_STKSetProfile(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ESTK_SETPROFILE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_STKSetAutoRsp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ESTK_SETAUTORSP;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_STKSetTR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ESTK_SETTR;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_STKSelectMainMenu(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 index)
{
    T_ONES16PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ESTK_SELECTMAINMENU;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = index;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_STKResponse(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_RES_PAR *par)
{
    T_ONEU32PARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    WIRELESS_INFO("!!!RIL_STKResponse\r\n");
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = ESTK_RESPONSE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.value = (_UINT32)par;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_InquireGprsState(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = INQUIREGPRSSTATE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_EstablishDataConnWithGPRS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strAPN, _BOOL bMUX)
{
    T_DATACONN req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = DATACONN_WITHGPRS;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    req.user = strAPN;
    req.bMUX = bMUX;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_EstablishDataConnWithCSD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                                     _CHAR8* dial, _CHAR8* user, 
                                     _CHAR8* pwd, _BOOL bMUX)
{
    T_DATACONN req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = DATACONN_WITHCSD;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    req.user = user;
    req.dial = dial;
    req.pwd = pwd;
    req.bMUX = bMUX;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ReleaseDataConn(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, 
                            _BOOL bMUX, _USHORT16 step)
{
    T_DATACONN req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = DATACONN_RELEASE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.bMUX = bMUX;
    req.step = step;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_IgniteModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = IGNITEMODULE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ResetModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = RESETMODULE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_WMExceptionCallback(_UINT32 ChannelID, HRIL hRIL, RIL_EXCEPTION_TYPE type)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;

    req.type = WMEXCEPTIONCALLBACK;
    req.hRequest = type;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ProcessURC(_UINT32 ChannelID, HRIL hRIL, _UINT32 eventID, 
                       _UINT32 param, _VOID* lpData, _UINT32 size)
{
    /*
    printf("RIL_ProcessURC eventID = %d, param:%d\r\n", eventID, param);
    */
    
    return hRIL->fCallback(ChannelID, eventID, param, lpData, size, hRIL->customParam);
}

_UINT32 RIL_ProcessRsp(_UINT32 ChannelID, HRIL hRIL, _UINT32 resultType, 
                       _UINT32 param, _VOID* lpData, _UINT32 size)
{
    if(param == 0)
    {
        return 0;
    }
    
    if(((T_REQUEST*)param)->fCallback != NULL)
    {
        ((T_REQUEST*)param)->fCallback(ChannelID, hRIL, resultType, lpData, size);
    }
    
    WIRELESS_FREE(param); 
    
    return 0;
}

_UINT32 RIL_BatteryCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = BATTERYCTRL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_ToneDetCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = TONEDETCTRL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_DAICtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = DAICTRL;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SelectVoiceGain(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SELECTVOICEGAIN;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SideToneModify(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam)
{
    T_ONESTRPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;

    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SIDETONEMODIFY;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;
    req.strValue = strParam;
        
    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

_UINT32 RIL_SaveConfigure(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback)
{
    T_BASICPARAM req;
    T_MESSAGE *pMsg;
    T_REQUEST *pRequest;
    pRequest = (T_REQUEST*)WIRELESS_MALLOC(sizeof(T_REQUEST));
    pRequest->fCallback = fCallback;

    req.type = SAVECONFIGURE;
    req.hRequest = (_UINT32)pRequest;
    req.hLine = hRIL->hLine;

    ProxySend(ChannelID, (_CHAR8*)&req, sizeof(req));
    pMsg = RIL_GetResult(ChannelID);

    return pMsg->param.reply.returnValue;
}

