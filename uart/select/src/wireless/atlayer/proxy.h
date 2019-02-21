
#ifndef PROXY_H
#define PROXY_H

#include "wm_struct.h"
#include "wm_interface.h"

#define ATLAYER_VERSION "ATLIB: V1.0.0"
typedef struct tagRIL* HRIL;

/**
  *@brief 用来接收URC信息的回调函数类型
  *@param HRIL hRIL 表明URC信息从哪个RIL上过来
  *@param _UINT32 eventID URC消息ID，该变量的类型是RIL_URC_ID
  *@param _UINT32 param 跟URC消息同步发来的参数
  *@param _VOID* lpData 跟URC消息同步发来的结构体指针，如果没有，该值为NULL
  *@param _UINT32 size 跟URC消息同步发来的结构体的个数，如果没有，该值为0
  *@param _UINT32 customParam 在调用RIL_Open函数时传入的customParam
  *@return    _UINT32 0: 执行成功。
*/

/*
typedef _UINT32 (*URCCALLBACK)(HRIL hRIL, _UINT32 eventID, _UINT32 param, _VOID* lpData, _UINT32 size, _UINT32 customParam);
*/

/**
  *@brief 用来接收某个操作的异步返回结果的回调函数类型
  *@param HRIL hRIL 表明返回从哪个RIL上过来
  *@param _UINT32 resultType 该操作的执行结果，该变量的类型是RIL_PARSE_RES
  *@param _VOID* lpData 跟操作结果同步发来的结构体指针，如果没有，该值为NULL
  *@param _UINT32 size 跟操作结果同步发来的结构体的个数，如果没有，该值为0. 如果resultType的值为RIL_prCMEERROR或者RIL_prCMSERROR，该值表示ERROR ID
  *@return    _UINT32 0: 执行成功。
*/
typedef _UINT32 (*RESCALLBACK)(_UINT32 ChannelID, HRIL hRIL, _UINT32 resultType, _VOID* pData, _UINT32 size);

/**
  *@brief 启动RIL
  *@param _UINT32 ChnlNum    [in] 设定无线模块的通道个数，cmux模式下通常不止一个通道，非cmux模式只有一个通道
  *@param _USHORT16 *ChID [in] 通道ID数组，保存各个通道的ID
  *@param _UINT32 Type [in] 无线模块的类型. 1: WAVECOM
  *@param _UINT32 *phATServer    [out] Radio server的句柄.该句柄标识一个模块，多模手机需要调用该接口多次。
  *@return    _UINT32 0: 启动成功。
*/
_UINT32 RIL_Setup(_UINT32 ChnlNum, _UINT32* ChID, _UINT32 Type, _UINT32* phATServer);

/**
  *@brief 结束所有启动的Radio server
  *@param
  *@return    _UINT32 0: 结束成功。
*/
_UINT32 RIL_Stop();

/**
  *@brief 更新Radio server的通道信息
  *@param HRIL hRIL [in] 任意一个在将要更新的Radio server上打开的RIL句柄
  *@param _UINT32 ChnlNum    [in] 设定无线模块的通道个数，cmux模式下通常不止一个通道，非cmux模式只有一个通道
  *@param _USHORT16 *ChID [in] 通道ID数组，保存各个通道的ID
  *@return    _UINT32 0: 更新成功。
*/
_UINT32 RIL_UpdateChannel(_UINT32 ChannelID, HRIL hRIL, _UINT32 ChnlNum, _USHORT16* ChID);

/**
  *@brief 在某个radio server上打开一个RIL句柄
  *@param _UINT32 eventType [in] 指定该RIL希望接收的消息类型，可以是RIL_URC_TYPE的任意组合。
  *@param _UINT32 hATServer [in] 指定在哪个radio server的句柄
  *@param URCCALLBACK fCallback [in] 指定接收radio server消息的回调函数
  *@param _UINT32 customPara [in] 指定附加信息，该信息会在回调函数中返回
  *@param HRIL *phRIL [out] 返回的RIL句柄 
  *@return    _UINT32 0: 打开成功
*/
_UINT32 RIL_Open(_UINT32* ChID, _UINT32 eventType, _UINT32 hATServer, URCCALLBACK fCallback, _UINT32 customParam, HRIL* phRIL);

/**
  *@brief 关闭一个RIL句柄
  *@param HRIL hRIL [in] 指定要关闭的RIL 句柄
  *@return    _UINT32 0: 关闭成功
*/
_UINT32 RIL_Close(HRIL hRIL);

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
                      RESCALLBACK fCallback, _UINT32* callHandle);

/**
  *@brief 拒接来电
  *@param HRIL hRIL [in] 指定要拒接来电的RIL 句柄
  *@param _UINT32 hCall [in] 指定要拒接的CALL 句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_RejectCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 接听来电
  *@param HRIL hRIL [in] 指定要接听来电的RIL 句柄
  *@param _UINT32 hCall [in] 指定要接听的CALL 句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_AnswerCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 挂断所有电话
  *@param HRIL hRIL [in] 指定要接听来电的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_HangupCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 取消拨号
  *@param HRIL hRIL [in] 指定要取消拨号的RIL 句柄
  *@param _UINT32 hCall [in] 指定要取消拨号的Call句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_CancelDial(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 切换所有hold/active电话的状态
  *@param HRIL hRIL [in] 指定要切换电话的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SwapCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 电话会议
  *@param HRIL hRIL [in] 指定要执行电话会议的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ConferenceCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 电话会议状态下结束单路
  *@param HRIL hRIL [in] 指定要执行结束单路的RIL 句柄
  *@param _UINT32 hCall [in] 指定要结束电话的call句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EndSingleCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 结束所有active状态的电话
  *@param HRIL hRIL [in] 指定要执行结束active操作的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EndActiveCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 结束所有hold状态的电话
  *@param HRIL hRIL [in] 指定要执行结束hold操作的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EndHoldCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 从active conference call中分离一路，单方通话，同时hold其它电话
  *@param HRIL hRIL [in] 指定要执行结束hold操作的RIL 句柄
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SplitFromConference(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

_UINT32 RIL_UpdateCurCallList(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief 拨分机号
  *@param HRIL hRIL [in] 指定要执行该操作的RIL 句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strDtmfStr [in] DTMF字符
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_DialDTMF(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strDtmfStr);

_UINT32 RIL_SetDTMFPrd(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strDtmfStr);

/**
  *@brief 查询指定类型的电话的个数
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RIL_CALL_STATUS status [in] 电话类型，如果该参数为RIL_CALL_STATUS_NUM，表示要查询所有电话数目
  *@param _UINT32 *callQty [out] 保存查询到的电话数目
  *@return    _UINT32 0: 执行成功
*/
_UINT32 RIL_GetCallQty(_UINT32 ChannelID, HRIL hRIL, RIL_CALL_STATUS status, _UINT32* callQty);

/**
  *@brief 查询电话的详细信息
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _UINT32 hCall [in] 指定要查询的CALL句柄，如果该参数为0，表示要查询所有电话的详细信息
  *@param RIL_CLCC_ENTRY *pCLCC [out] 保存查询到的电话信息
  *@param _UINT32 *number [out] 保存查询到的电话数目
  *@return    _UINT32 0: 执行成功
*/
_UINT32 RIL_GetCallInfo(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RIL_CLCC_ENTRY *pCLCC, _UINT32 *number);

/**
  *@brief 设定呼叫等待
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _BOOL bURCEnable [in] 设定呼叫等待URC是否上报
  *@param _BOOL bEnable [in] 设定呼叫等待开关
  *@param RIL_DATA_CLASS eClass [in] 指定要设定的通话类型
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetCallWait(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass);

/**
  *@brief 查询呼叫等待状态，该函数的异步返回结果将按照RIL_CCWA_LIST结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_DATA_CLASS eClass [in] 指定要查询的通话类型
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireCallWait(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_DATA_CLASS eClass);

/**
  *@brief 设定呼叫转移
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_CCFC_FWDCALLS *cfData [in] 呼叫等待结构体
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetCallForward(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CCFC_FWDCALLS *cfData);

/**
  *@brief 查询呼叫转移状态，该函数的异步返回结果将按照RIL_CCFC_FWDCALLS结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_CCFC_REAS reason [in] 指定要查询的呼叫转移类型
  *@param RIL_DATA_CLASS Class [in] 指定要查询的通话类型
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireCallFwd(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class);

/**
  *@brief 发送短信
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param const _UCHAR8* pPDU [in] 打包好的PUD数据包
  *@param _UINT32 option [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SendMsg(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, RESCALLBACK fCallback);

_UINT32 RIL_QiSendData(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pDataBuf, _UINT32 DataLen, RESCALLBACK fCallback);

_UINT32 RIL_SendMsgByTxt(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pTxt, _UINT32 uStringLen, const _UCHAR8* pNumber, RESCALLBACK fCallback);

_UINT32 RIL_SendMsg_CDMA(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, RESCALLBACK fCallback);

/**
  *@brief 读SIM卡短信，函数的异步返回结果以RIL_SMS_ENTRY结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadSMS(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

_UINT32 RIL_ReadSMSByTxt(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief 读SIM卡短信，函数的异步返回结果以RIL_SMS_ENTRY结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadSMS_CDMA(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief 保存短信到SIM卡
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param const _UCHAR8* pPDU [in] 打包好的PUD数据包
  *@param _UINT32 option [in] Reserved to be use
  *@param _BOOL read_flag [in] EOS_TRUE: 短信已读；EOS_FALSE:短信未读
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SaveMsgToSIM(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, _BOOL read_flag, _SHORT16 PDUlen, RESCALLBACK fCallback);

/**
  *@brief 读SIM卡短信,不改变短信在SIM卡上的状态。函数的异步返回结果以RIL_SMS_ENTRY结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadSMSUnchgStat(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief 查询指定短信空间的容量。函数的异步返回结果以RIL_CPMS结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_MSG_STORAGE storage [in] 指定短信空间
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireSmsSpace(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,RIL_MSG_STORAGE storage);

/**
  *@brief 列出SIM卡上所有指定状态的短信，不改变短信状态。函数的异步返回结果以RIL_SMS_ENTRY结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_SMS_MODE stat [in] 指定短信状态
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ListSMS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_SMS_MODE stat);

/**
  *@brief 查询信息中心号码。函数的异步返回结果以RIL_SMS_CENADDRESS结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireSmsCenter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 设置信息中心号码
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strSca [in] 短信中心号码
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetSmsCenter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strSca);

/**
  *@brief 设置新短信的存储模式
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_STORENEWMSG_MODE mode [in] 指定短信存储模式
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_NewMsgStoreMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_STORENEWMSG_MODE mode);

/**
  *@brief 设置短信存储状态
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _BOOL bFull [in] EOS_FALSE :短信未存满，EOS_TRUE:短信已经存满
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetSMSStorageStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bFull);

/**
  *@brief 设置小区广播
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SelectBroadCast(_UINT32 ChannelID, HRIL hRIL, _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss, RESCALLBACK fCallback);

/**
  *@brief 删除SIM卡上的短信
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param _USHORT16 index [in] 短信在SIM卡上的索引号
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_DeleteSMS(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief 激活SIM卡STK功能
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_STKSetProfile(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_STKSetAutoRsp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_STKSetTR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 从STK setup menu的菜单项中选择一项
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _SHORT16 index [in] 选中菜单项的ID号
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_STKSelectMainMenu(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 index);

/**
  *@brief 响应STK proactive command
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_RES_PAR *par [in] 响应内容
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_STKResponse(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_RES_PAR *par);

/**
  *@brief 发送AT命令测试通道是否通
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_Attention(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 读SIM卡指定电话簿的信息。该函数的异步返回结果将以RIL_PB_SIM_ENTRY结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_PB_STORAGE storage [in] 指定电话簿
  *@param _SHORT16 begin [in] 起始索引号
  *@param _SHORT16 end [in] 结束索引号；如果该值为0，只读begin参数指定的一条记录
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReadPbEntries(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end);

/**
  *@brief 查询SIM卡指定电话簿的容量，该函数的异步返回结果将以RIL_SIMPB_SPACE结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_PB_STORAGE storage [in] 指定电话簿
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquirePbSpace(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage);

/**
  *@brief 写电话簿记录到SIM卡指定电话簿的指定位置
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_PB_STORAGE storage [in] 指定电话簿
  *@param _SHORT16 index [in] 指定电话簿的位置，如果该值为0，写入到电话簿第一个空的位置
  *@param _CHAR8* strNumber [in] 电话号码
  *@param _USHORT16 *strText [in] 姓名，需要采用unicode编码格式
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_WritePbEntry(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText);

/**
  *@brief 删除SIM卡指定电话簿指定位置上的记录
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_PB_STORAGE storage [in] 指定电话簿
  *@param _SHORT16 index [in] 指定电话簿的位置
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_DelPbEntry(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage, _SHORT16 index);

/**
  *@brief 完成部分对SIM卡和无线模块的初始化，这些初始化操作不依赖PIN码的状态
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InitBeforePIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 完成部分对SIM卡和无线模块的初始化，这些初始化操作必须在PIN码有效的状态下才能完成
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InitAfterPIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 查询PIN码的状态
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquirePinStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_InquireInitStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 根据当前SIM卡PIN码的状态决定是输入PIN码还是PUK码
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param [in] strPIN PIN code
  *@param [in] strNewPIN 如果要输入PUK码，要指定新的PIN码
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EnterPIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strPIN, _CHAR8* strNewPIN);

/**
  *@brief 更改密码
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_FACILITY fac[in] facility lock function that need to set new password
  *@param _CHAR8* strOldPwd[in] old password
  *@param _CHAR8* strNewPwd[in] new password
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ChangePassword(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd);
/**
  *@brief 查询PIN码剩余尝试次数，次数为零时SIM卡自动上锁
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquirePinCount(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _UCHAR8 type);

/**
  *@brief 设置设备锁定，呼叫限制等
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param [in] fac facility
  *@param [in] bLock: EOS_TRUE lock; EOS_FALSE unlock
  *@param [in] strPasswd: password
  *@param [in] eClass: data class
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_FacilityLock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass);

/**
  *@brief 查询设备锁定状态，该接口的异步返回结果将以RIL_CLCK_LOCK结构体类型返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param [in] fac facility
  *@param [in] eClass: data class
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireFacilityLock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, RIL_DATA_CLASS eClass);

/**
  *@brief 查询当前注册的网络供应商，该函数的异步返回结果将以RIL_COPS_CURRENTNAME结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireCurOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_InquireBSInfo(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 选择并注册网络
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param [in] mode 自动注册网络,手动注册网络还是取消注册
  *@param [in] strOper: 网络ID号，只有在选择手动注册时需要提供
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SelectOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_NET_MODE mode, _CHAR8* strOper);

/**
  *@brief 查询当前的可用网络，该函数的异步返回结果将以RIL_COPS_NETWORK结构返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireAvaiOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 查询当前GPRS网络的附着状况，该函数的异步返回结果将以T_BOOL变量返回，EOS_TRUE表示已经附着上
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireGprsState(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 通过GPRS创建数据连接
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strAPN [in] 接入点
  *@param _BOOL bMUX [in] 指定是否MUX模式
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EstablishDataConnWithGPRS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strAPN, _BOOL bMUX);

/**
  *@brief 通过CSD创建数据连接
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* dial [in] CSD拨号号码
  *@param _CHAR8* user [in] 用户名
  *@param _CHAR8* pwd [in] 密码
  *@param _BOOL bMUX [in] 指定是否MUX模式
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EstablishDataConnWithCSD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX);

/**
  *@brief 释放数据连接
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _BOOL bMUX [in] 指定是否MUX模式
  *@param _USHORT16 step [in] 指定当前数据连接处在的阶段
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ReleaseDataConn(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bMUX, _USHORT16 step);

/**
  *@brief 给模块点火
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_IgniteModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 复位模块
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ResetModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 关闭模块
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SwitchOffModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 模块异常回调函数，如检测到模块异常掉电，要调用该接口通知RIL做善后处理
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RIL_EXCEPTION_TYPE type [in] 异常类型
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
  *@author  mioabaoli
  *@date    2007.11.26
*/
_UINT32 RIL_WMExceptionCallback(_UINT32 ChannelID, HRIL hRIL, RIL_EXCEPTION_TYPE type);

/**
  *@brief 设置AT命令的回显模式
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _BOOL bOnOff [in] EOS_TRUE:回显模式打开. EOS_FALSE:回显模式关闭
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetCommandEcho(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bOnOff);


_UINT32 RIL_SendAT(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

_UINT32 RIL_ModulePwrCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

_UINT32 RIL_SendATOrigin(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strATString);

/**
  *@brief 查询手机的软件版本号
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strValue [in] NULL
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_GetProductID(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

/**
  *@brief 恢复模块的出厂设置
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param SysTime [in] 时间结构体
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetManufDefault(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 查询手机的IMEI号，该函数的异步返回结果是一个T_U8型的字符串
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireIMEI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_SetIMEI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strIMEI);

_UINT32 RIL_SetBaudrate(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strRate);

/**
  *@brief Inquire net band
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief set net band
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strMode [in] 0:stand for 900 (EGSM), 1:stand for 1800 (DCS), 2:stand for 900/1800,auto(DUAL)
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetNetBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMode);

/**
  *@brief 查询累计通话费用
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_GetCurrentAcm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 通话计费清零
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strPasswd [in] 需要输入PIN2码
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ResetAcm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strPasswd);

/**
  *@brief 查询累计通话计费的最大值
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_GetAcmMax(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
/**
  *@brief 设置累计通话计费的最大值
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strAcmax [in] ACM最大值
  *@param _CHAR8* strPasswd [in] 需要输入PIN2码
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetAcmMax(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strAcmax, _CHAR8* strPasswd);

/**
  *@brief 查询最后通话的通话费用
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_GetCurCallMeter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 设置模块的系统时间
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param SysTime [in] 时间结构体
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetClock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_SYSTIME *SysTime);

/**
  *@brief 查询手机的时间，该函数的异步返回结果是一个RIL_SYSTIME型的结构体
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireClock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 查询SIM卡的IMSI(国际移动用户识别)号 
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireSubscriber(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 设置通话音量（听筒）
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param volume [in] 指定要设置的音量级别
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetSpeakerVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 volume);


_UINT32 RIL_SetFreqBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 bandindex);

_UINT32 RIL_SetQSFR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 sfrIndex);

/**
  *@brief 进入CMUX模式
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_EnterMultiplex(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 查询手机当前的信号强度，该函数的异步返回结果将以_INT32类型返回，该值表示信号级别，取值范围0～4
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireSignalQuality(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
//weikk 2016.3.31 add XREG
_UINT32 RIL_InquireNetType(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
//weikk 20161026 add ADC
_UINT32 RIL_InquireADC(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
//weikk 20161026 add MMAD
_UINT32 RIL_InquireMMAD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);



_UINT32 RIL_InquireSideTone(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_InquireCallStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 设置模块的音频通道
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_AUDIO_PATH_TYPE Mode [in] 通道类型
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetAudioPath(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_AUDIO_PATH_TYPE Mode);

_UINT32 RIL_SetAudioMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_AUDIO_MOD_TYPE Mode);

_UINT32 RIL_SetVoiceHangUpCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bEnable);

_UINT32 RIL_SetMicGainLevel(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 volume);

/**
  *@brief 设置模块的静音模式
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _BOOL bMute [in] EOS_TRUE: mute on; EOS_FALSE: mute off
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetAudioMute(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bMute);

/**
  *@brief 控制模块发声
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _USHORT16 type [in] 声音类型
  *@param _USHORT16 volume [in] 音量
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_PlayTone(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _USHORT16 type, _USHORT16 volume);

/**
  *@brief 指定模式下，哪些URC要上报
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param RIL_URC_MODE mode [in]
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_ConfigURCMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_URC_MODE mode);

/**
*@brief 得到模块的睡眠状态
*@param [in] HRIL hRIL
*@return _BOOL
*@retval EOS_TRUE: sleeping; EOS_FALSE: wakeup
*/
_BOOL RIL_GetSleepState(_UINT32 ChannelID, HRIL hRIL);

/**
  *@brief 查询小区信息，该函数的异步返回结果将以RIL_CELL_TEST结构体返回
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strMode [in] reserved to be use
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_FieldTestReport(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMode);

/**
  *@brief 查询耳机插入状态，查询结果将以URC消息的方式通知应用程序
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireHeadsetStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief 发送USSD请求
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strN [in] 0: Disable the result code, 1: Enable the result code, 2: Cancel session
  *@param _CHAR8* strStr [in] USSD-string
  *@param _CHAR8* strDcs [in] Cell Broadcast Data Coding Scheme
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SendUSSD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs);

/**
  *@brief 用于继续USSD服务时，发送用户输入的USSD请求数据 
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@param _CHAR8* strUssdString [in] USSD-string 
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SendUssdData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strUssdString);

/**
  *@brief 结束USSD会话服务
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_CancelUssdData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Check the state of Calling line identification presentation
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_CheckCLIP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Check the state of Calling line identification restriction
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_CheckCLIR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);


_UINT32 RIL_SetCLIRActive(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_SetCLIRDisable(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Dailcall without ";"
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_DailCLIR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

/**
  *@brief Check the state of COLR
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_CheckCOLR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief check Colp State
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_InquireCOLP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Set Colp Status
  *@param HRIL hRIL [in] 指定要执行该操作的RIL句柄
  *@param RESCALLBACK fCallback [in] 该请求执行结束的回调函数
  *@return    _UINT32  <0x1000000: 该请求的ID号，用户可以通过该ID号取消已经发出的请求; >=0x1000000: 错误ID
*/
_UINT32 RIL_SetColpStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);


_UINT32 RIL_SetFunctionality(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strFun);
_UINT32 RIL_SetSimVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMod);
_UINT32 RIL_SetRingBack(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn);
_UINT32 RIL_SetVBatt(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn);
_UINT32 RIL_SetNWScanMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr);
_UINT32 RIL_InquireQIRegApp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_SetQIRegApp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr);
_UINT32 RIL_SetQICsgp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr);
_UINT32 RIL_SetQIAct(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_SetQIDeact(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_SetQindi(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_SetQidnsip(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_SetCnetScan(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_InquireCellLoc(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_SetQFun(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strFun);
_UINT32 RIL_GetFunctionality(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_GetSimVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_SetQMoStat(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn);
_UINT32 RIL_SetQCCINFOEn(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn);
_UINT32 RIL_FSendSimData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strSimData);
_UINT32 RIL_SetQURCCFG(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_SetEngMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _UINT32 mode, _UINT32 dump);
_UINT32 RIL_CallingLineId(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);
_UINT32 RIL_InquireCCID(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_InquireIMSI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_LockBS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strBSIndex);


_UINT32 RIL_SetOpenTCPUDP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strStr);
_UINT32 RIL_InquireQiRcvData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_SetQiClose(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_InquireQiStat(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
_UINT32 RIL_InquireQiLocIP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);


_UINT32 RIL_XDRVControlAudioLoopTest(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL enable);

_UINT32 RIL_SetModuleSleep(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _UCHAR8 enable);

_UINT32 RIL_ProcessURC(_UINT32 ChannelID, HRIL hRIL, _UINT32 eventID, _UINT32 param, _VOID* lpData, _UINT32 size);
_UINT32 RIL_ProcessRsp(_UINT32 ChannelID, HRIL hRIL, _UINT32 resultType, _UINT32 param, _VOID* lpData, _UINT32 size);

_UINT32 RIL_NetworkRegistration(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);
_UINT32 RIL_InquireNetRegStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);
_UINT32 RIL_InquireGNetRegStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

_UINT32 RIL_BatteryCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_ToneDetCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_DAICtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_SelectVoiceGain(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_SideToneModify(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strParam);
_UINT32 RIL_SaveConfigure(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_SetQJamDet(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strEn);

#endif

