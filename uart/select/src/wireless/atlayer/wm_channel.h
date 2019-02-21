/**
 * @file wm_interface.h
 * @brief external APIs for Wirelss Model Device
*/
#ifndef  WM_CHANNEL_H
#define  WM_CHANNEL_H

#include "wm_interface.h"
#include "WAVEC_Interface.h"
#include "WAVEC_at_pack.h"

#define RIL_CHANNEL_ANY    0xeeee

#define ATQueueSize         2048
#define RspQueueSize        512
#define SendTaskStackSize   20480
#define RcvTaskStackSize    8192
#define ATLineBuffSize      1024//2048
#define ATBufferSize        2048

typedef enum
{
    CHANNEL_MODE_AT = 0,
    CHANNEL_MODE_PPP
}T_CHANNEL_MODE;

/** 通道句柄 */
typedef struct tagCHNL
{
    _USHORT16    ChID;                /** 通道ID*/
    _LIST       atcmdlist;          /** 指向通道的命令队列的句柄*/
    _LIST       atrsplist;            /** 通道的响应队列的句柄*/

    _BOOL        bWaitingRsp;
    _UINT32     uiSendATTick;       /* the tick start to send at    */
    T_hTask     hSendTask;            /** 通道的发送线程的句柄*/
    _UCHAR8        SendStack[SendTaskStackSize];
    T_hTask     hReceiveTask;        /** 通道的接收线程的句柄*/
    _UCHAR8        RcvStack[RcvTaskStackSize];
    _UINT32        ulQueueSize;        /** 命令队列最多能存放的AT命令包的数目*/
    _UINT32        lFreeSpace;            /** 命令队列中还能存放几条AT命令包*/
    HWM            hWM;                /** 包含该通道的模块句柄指针*/
    T_CHANNEL_MODE channelMode;

    _UINT32        ATLineLen;       /* cached line length */
    _UCHAR8        ATLineBuffer[ATLineBuffSize];
    _UCHAR8        ATBuffer[ATBufferSize];

    T_ATRESPONSE ATRsp;

    T_pURCRESULT pURC;                /**<需要屏蔽的URC信息*/
    T_pURCRESULT pResult;            /**<需要增加的返回结束标志*/
} T_HCHNL;

/**
@brief:通过这个函数创建通道，返回通道句柄
 *@param    ChID             [in]通道号
 *@param pWM_Handle         [in]无线模块句柄用于在URC中标识模块


 *@return     T_HCHNL*
 *@retval    创建出的通道句柄指针
*/
HCHNL CreateChannel(HWM pWM_Handle, _USHORT16 ChID, T_HCHNL *pChnl);


_INT32 SuspendChannel(HWM pWM_Handle, _USHORT16 ChID);
_INT32 ResumeChannel(HWM pWM_Handle, _USHORT16 ChID);

_VOID SendThread(_VOID);
_VOID ReceiveThread(_VOID);


/**
@brief:通过这个函数销毁通道
 *@param    pChnl        [in]通道句柄

 *@return     _UINT32
 *@retval    >0 销毁通道成功
*/
_INT32 DestroyChannel(HCHNL hChnl);

/**
 *@brief通过这个函数清空AT队列通道
 *@param    pChnl        [in]通道句柄

 *@return     _UINT32
 *@retval    >0 清空通道成功
*/
_INT32 EmptyChannel(HCHNL hChnl);

/**
 *@brief 得到指定通道的命令队列中暂存的命令个数
 *@param    pChnl        [in]通道句柄

 *@return     _INT32
 *@retval    >=0 命令个数，<0 错误ID
*/
_INT32 GetCmdNumInQueue(HCHNL hChnl);

/**
  *@brief通过这个接口将打包后的AT命令发送到命令队列里。
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    Cmd                    [in]打包好的AT命令
  *@param    Size                [in]AT命令的数据长度
  *@param    *ChID                [in]在该通道上发送该AT命令.
                                如果该值是RIL_CHANNEL_ANY，则根据通道负荷来判断向哪个通道发送,并把实际选中的通道号通过该参数传回给调用者.
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    fParse                [in]该AT命令返回结果的解析函数
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendCmd(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode);

/**
  *@brief通过这个接口将打包后的AT命令发送到命令队列头。
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    Cmd                    [in]打包好的AT命令
  *@param    Size                [in]AT命令的数据长度
  *@param    *ChID                [in]在该通道上发送该AT命令.
                                如果该值是anywhere，则根据通道负荷来判断向哪个通道发送,并把实际选中的通道号通过该参数传回给调用者.
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    fParse                [in]该AT命令返回结果的解析函数
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendCmd_ToHead(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size,  _USHORT16 *ChID, 
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode);

/**
  *@brief通过这个接口将打包后的AT命令发送到命令队列里，指定超时时间
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    Cmd                    [in]打包好的AT命令
  *@param    Size                [in]AT命令的数据长度
  *@param    *ChID                [in]在该通道上发送该AT命令.
                                如果该值是RIL_CHANNEL_ANY，则根据通道负荷来判断向哪个通道发送,并把实际选中的通道号通过该参数传回给调用者.
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    fParse                [in]该AT命令返回结果的解析函数
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendCmdWithTimeout(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout);

/**
  *@brief通过这个接口将打包后的AT命令发送到命令队列头,指定超时时间
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    Cmd                    [in]打包好的AT命令
  *@param    Size                [in]AT命令的数据长度
  *@param    *ChID                [in]在该通道上发送该AT命令.
                                如果该值是anywhere，则根据通道负荷来判断向哪个通道发送,并把实际选中的通道号通过该参数传回给调用者.
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    fParse                [in]该AT命令返回结果的解析函数
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendCmd_ToHeadWithTimeout(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size,  _USHORT16 *ChID, 
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout);

/**
 @brief:通过这个接口向模块发送硬件相关命令。
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    CmdType                [in]hardware command type
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendHardwareCmd(HWM pWM_Handle, T_ECMD_TYPE CmdType, _USHORT16 *ChID,
                        _BOOL Notify, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode);

/**
  *@brief通过这个接口将打包后的AT命令发送到命令队列里，指定URC,result,等附加信息
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    Cmd                    [in]打包好的AT命令
  *@param    Size                [in]AT命令的数据长度
  *@param    *ChID                [in]在该通道上发送该AT命令.
                                如果该值是RIL_CHANNEL_ANY，则根据通道负荷来判断向哪个通道发送,并把实际选中的通道号通过该参数传回给调用者.
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    fParse                [in]该AT命令返回结果的解析函数
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendCmdWithAddiInfo(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout, 
                        _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult);

/**
  *@brief通过这个接口将打包后的AT命令发送到命令队列头部，指定URC,result,等附加信息
  *@param    pWM_Handle            [in]指向无线模块句柄的指针
  *@param    Cmd                    [in]打包好的AT命令
  *@param    Size                [in]AT命令的数据长度
  *@param    *ChID                [in]在该通道上发送该AT命令.
                                如果该值是RIL_CHANNEL_ANY，则根据通道负荷来判断向哪个通道发送,并把实际选中的通道号通过该参数传回给调用者.
  *@param    Notify                [in]该AT命令的执行结果是否通知server
  *@param    fParse                [in]该AT命令返回结果的解析函数
  *@param    serviceHandle        [in]用于将AT命令返回结果通知上层的serviceHandle句柄，
                                    在调回调函数时作为参数传入
  *@param     serviceType        [in]功能管理模块类型
  *@param     customHandle        [in]客户信息句柄

  *@param    CmdID                [in]AT命令的ID号，如果该值是NEWCMDID，需要给该命令赋流水号
  *@param    retryTimes            [in]AT命令执行错误时的重试次数
  *@param    executeMode            [in]AT命令执行模式，类型是T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
*/
_UINT32 SendCmdToHeadWithAddiInfo(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout, 
                        _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult);

/**
 *@brief 设定通道的模式，如果通道设为PPP模式后，将不再接收AT命令，直到重新设为AT命令模式
 *@param [in] HWM pWM_Handle 模块句柄
 *@param [in] _USHORT16 ChID 通道ID
 *@param [in] T_CHANNEL_MODE mode 通道模式
 *@return     _VOID
*/
_VOID SetChannelMode(HWM pWM_Handle, _USHORT16 ChID, T_CHANNEL_MODE mode);

#endif
