
#include "wm_interface.h"
#include "wm_channel.h"
#include "WAVEC_Interface.h"

#include "../wireless_common.h"
#include "../wireless_interface_inc.h"
#include "../wireless_debug.h"

extern WIRELESSROOTST    stWirelessRootSt;

static _VOID InstantiationModem(HWM pWM_handle, _UINT32 Type);


/***********************************************************************************
@brief:该接口主要提供给AT Server调用，用来实例化具体的模块，同时启动该无线模块设备。
*@param    ChnlNum        [in]该模块的通道数目
*@param    ChID        [in]指向一个数组，数组中有每个通道对应的ID号
*@param    Type        [in]模块的类型
*@param    WM_Handle    [out]无线模块的句柄指针

*@retval    =0    创建成功, <0 创建失败 
**********************************************************************************/
_INT32 WM_Create(_UINT32 ChnlNum, _USHORT16* ChID, _UINT32 Type, HWM* WM_Handle)
{
    HWM pWM_handle = NULL;
    _UINT32 uiChannelIndex = 0;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChID[0]];
    
    /* Create Module Handle    */
    pWM_handle = (HWM)malloc(sizeof(T_HWM));

    pWM_handle->bInitOK = EOS_FALSE;

    /*create AT channels*/
    /* ChnlNum always = 1, if ChnlNum > 1, must modify here!!! */
    for (uiChannelIndex = 0; uiChannelIndex < ChnlNum; uiChannelIndex++)
    {
        CreateChannel(pWM_handle, ChID[uiChannelIndex], pWirelessChannelCtl->pChnl);
    }
                        
    /* Fill Module Handle    */
    pWM_handle->ulChnlNum = ChnlNum;
    pWM_handle->cmdID = NEWCMDID;
    pWM_handle->InvalidCmdID = NEWCMDID;
    pWM_handle->DataConnCmd = DATACALL_NULL;
    pWM_handle->uiChnlId = ChID[0];
    
    pWM_handle->pATFunction = (T_ATFUNCTION*)malloc(sizeof(T_ATFUNCTION));
    /*实例化函数指针*/
    InstantiationModem(pWM_handle, Type);
    
    pWM_handle->bInitOK = EOS_TRUE;
    pWM_handle->bSleeping = EOS_TRUE;
    /*返回创建好的模块句柄指针*/
    *WM_Handle = pWM_handle;
    
    return 0;
}


static _VOID InstantiationModem(HWM pWM_handle, _UINT32 Type)
{
    switch (Type)
    {
        case WAVECOM:
        {
            WAVEC_Create(pWM_handle);
            break;
        }
        default:
        {
            break;
        }
    }
}


/***********************************************************************************
@brief:该接口主要提供给AT Server调用，用来销毁无线设备模块
*@param    pWM_Handle            [in]无线设备模块句柄
 
*@return    _INT32                [out]调用是否成功
*@retval  >0 表示执行成功
**********************************************************************************/
_INT32 WM_Destory(HWM pWM_Handle)
{    
    _USHORT16 count = 0;
    _INT32 status = 0;
    
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];
    
    /*释放各个通道句柄占用的空间*/
    for (count = 0; count < (pWM_Handle->ulChnlNum); count++)
    {
        status = DestroyChannel(pWirelessChannelCtl->pChnl);
    }
    
    WIRELESS_TRACE;
    WIRELESS_FREE(pWM_Handle->pATFunction);
    pWM_Handle->pATFunction = NULL;
    
    WIRELESS_TRACE;
    WIRELESS_FREE(pWM_Handle);
    pWM_Handle = NULL;
    
    return status;
}

/**
*@brief:该接口主要提供给AT Server调用，用来更新无线模块设备的通道信息
*@param [in] HWM hWM 已经创建好的无线模块设备句柄
*@param    [in] ChnlNum 更新的通道数目
*@param    [in] ChID 指向一个数组，数组中有每个通道对应的ID号
*@retval    =0    操作成功, <0 操作失败 
*/
_INT32 WM_UpdateChannel(HWM hWM, _UINT32 ChnlNum, _USHORT16 *ChID)
{
    _UINT32 count = 0;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    hWM->bInitOK = EOS_FALSE;
    
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[ChID[0]];

    /*释放各个通道句柄占用的空间*/
    for(count=0; count<(hWM->ulChnlNum); count++)
    {
        DestroyChannel(pWirelessChannelCtl->pChnl);
    }

    /*create AT channels*/
    for(count=0; count<ChnlNum; count++)
    {        
        CreateChannel(hWM, ChID[count], pWirelessChannelCtl->pChnl);
    }
    hWM->ulChnlNum = ChnlNum;

    hWM->bInitOK = EOS_TRUE;
    return 0;
}

/**
*@brief 当模块有异常时，需要用该接口去复位无线模块设备
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 WM_Reset(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    _UINT32 count = 0;
    _UINT32 rtn;

    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[pWM_Handle->uiChnlId];

    rtn = (*(pWM_Handle->pATFunction->P_Reset))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle);
    
    pWM_Handle->bInitOK = EOS_FALSE;
    /*清空各个通道*/
    for(count=0; count<(pWM_Handle->ulChnlNum); count++)
    {
        EmptyChannel(pWirelessChannelCtl->pChnl);
    }
    return rtn;
}

#if 0 /* kinson.xiao 2011-12-18    */
 /***********************************************************************************
 @brief:通过这个接口解析后的AT响应回调给AT Server
 *@param    ResultCallback   [in]指向AT响应回调函数的指针
 *@param    UrcCallback          [in]指向URC回调函数的指针
 
 *@return    _VOID                             
***********************************************************************************/
_VOID WM_SetCallback (HWM pWM_Handle, ATCALLBACK ResultCallback, URCCALLBACK UrcCallback)
{
    pWM_Handle->pATCallback = ResultCallback;
    pWM_Handle->pUrcCallback = UrcCallback;
}
#endif

_VOID WM_SetDataConnMode(HWM pWM_Handle, T_DATACALL_TYPE DataMode)
{
    pWM_Handle->DataConnCmd = DataMode;
    (*(pWM_Handle->pATFunction->P_SetDataConnMode))(pWM_Handle, DataMode);
}

_VOID WM_SetCancelCmdID(HWM pWM_Handle, _UINT32 InvalidCmdID)
{
    pWM_Handle->InvalidCmdID = InvalidCmdID;
}

/***********************************************************************************
@brief:AT Server通过这个接口调用拨号的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

*@param    strNumber        [in]拨号参数
*@param    eClass            [in]拨号的类型
  
*@return    _UINT32                
*@retval    >0            该AT命令在AT命令队列中的流水号            
***********************************************************************************/
_UINT32 WM_DialCall(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, 
                  _UINT32 customHandle,    _CHAR8* strNumber, RIL_DATA_CLASS eClass)
{
    return (*(pWM_Handle->pATFunction->P_Dial_Call))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strNumber, eClass);
} 

/***********************************************************************************
@brief:AT Server通过这个接口调用删除短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strIndex        [in]SMS index

*@return    _UINT32                
*@retval    >0                    该AT命令在AT命令队列中的流水号            
***********************************************************************************/
_UINT32 WM_DeleteSMS(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strIndex)
                   
{
    return (*(pWM_Handle->pATFunction->P_Delete_SMS))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strIndex);
}

/***********************************************************************************
@brief:AT Server通过这个接口调用设置短信格式的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strMode            [in]SMS strMode
***********************************************************************************/
_UINT32 WM_SetSmsFormat(HWM pWM_Handle, _UINT32 serviceHandle, 
                      T_EDEVICEETYPE serviceType,    _UINT32 customHandle, _CHAR8* strMode)
{
    
    return (*(pWM_Handle->pATFunction->P_Set_SmsFormat))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strMode);    
}

_UINT32 WM_SetTxtSmsShow(HWM pWM_Handle, _UINT32 serviceHandle, 
                      T_EDEVICEETYPE serviceType,    _UINT32 customHandle, _CHAR8* strMode)
{
    
    return (*(pWM_Handle->pATFunction->P_Set_TxtSmsShow))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strMode);    
}

/***********************************************************************************
@brief:AT Server通过这个接口调用列出短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    RIL_SMS_MODE stat [in] 0: Received unread; 1: Reveived read;
2: stored unsend; 3: stored send; 4: All message

*@return   _UINT32                
*@retval   >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ListSMS(HWM pWM_Handle, _UINT32 serviceHandle,
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_SMS_MODE stat)
                 
{
    return (*(pWM_Handle->pATFunction->P_List_SMS))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, stat);
    
} 


/***********************************************************************************
@brief:AT Server通过这个接口调用读短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strindex        [in]SMS index

*@return   _UINT32                
*@retval   >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ReadSMS(HWM pWM_Handle, _UINT32 serviceHandle,
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index)           
{
    return (*(pWM_Handle->pATFunction->P_Read_SMS))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, index);    
} 

_UINT32 WM_ReadSMS_ByTxt(HWM pWM_Handle, _UINT32 serviceHandle,
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index)       
{
    return (*(pWM_Handle->pATFunction->P_Read_SMS_ByTxt))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, index);    
} 

/***********************************************************************************
@brief:AT Server通过这个接口调用读短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strindex        [in]SMS index

*@return   _UINT32                
*@retval   >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ReadSMS_CDMA(HWM pWM_Handle, _UINT32 serviceHandle,
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index)
                 
{
    return (*(pWM_Handle->pATFunction->P_Read_SMS_CDMA))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, index);
} 



/***********************************************************************************
@brief:AT Server通过这个接口调用通过pdu模式发送短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strLength        [in]SMS length

*@return   _UINT32                
*@retval   >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SendSmsByPduMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strLength)                      
{
    return (*(pWM_Handle->pATFunction->P_SendSms_ByPduMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strLength);
} 

_UINT32 WM_SendQidata(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strLength)                      
{
    return (*(pWM_Handle->pATFunction->P_SendQidata))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strLength);
} 

_UINT32 WM_SendSmsByPduMode_CDMA(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle,  _CHAR8* strPDU)
                          
{
    return (*(pWM_Handle->pATFunction->P_SendSms_ByPduMode_CDMA))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPDU);
}

_UINT32 WM_SendSmsByTxtMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle,  _CHAR8* strPDU)
                          
{
    return (*(pWM_Handle->pATFunction->P_SendSms_ByTxtMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPDU);
}

/***********************************************************************************
@brief:AT Server通过这个接口调用通过pdu模式保存短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strLength        [in]SMS length
*@param    read_flag        [in]0:unread sms,1:read sms

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SaveSmsByPduMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strLength, _UCHAR8 read_flag)
                          
{
    return (*(pWM_Handle->pATFunction->P_SaveSms_ByPduMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strLength, read_flag);
    
} 

/***********************************************************************************
@brief:AT Server通过这个接口调用发送pdu数据的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strPDU            [in]PUD data

*@return    _UINT32                
*@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SendSmsPduData(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPDU)
                        
{
    return (*(pWM_Handle->pATFunction->P_SendSms_PduData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPDU);
    
} 

_UINT32 WM_SendQiRawData(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPDU, _UINT32 len)
                        
{
    return (*(pWM_Handle->pATFunction->P_SendQiRawData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPDU, len);
    
} 

_UINT32 WM_SendSmsTxtData(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPDU, _UINT32 uiLen)
                        
{
    return (*(pWM_Handle->pATFunction->P_SendSms_TxtData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPDU, uiLen);
    
} 


/***********************************************************************************
@brief:AT Server通过这个接口调用保存pdu数据在SIM 卡的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strPDU            [in]PUD data

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SaveSmsPduData(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPDU)
                        
{
    return (*(pWM_Handle->pATFunction->P_SaveSms_PduData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPDU);    
} 

/***********************************************************************************
@brief:AT Server通过这个接口调用指示新短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strParam        [in]mode+mt+bm+ds+bfr

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_StoreNewMsgMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_STORENEWMSG_MODE mode)
                          
{
    return (*(pWM_Handle->pATFunction->P_StoreNewMsgMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, mode);
} 

/***********************************************************************************
@brief:AT Server通过这个接口调用回应新短信的AT命令
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_NewSMSAck(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_NewSMS_Ack))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
} 



/***********************************************************************************
@brief:Get the preferfed SMS storage
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param     storage        [in]

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireSmsStorage(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                           RIL_MSG_STORAGE storage)
                           
{
    return (*(pWM_Handle->pATFunction->P_Inquire_SmsStorage))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, storage);
    
}


/***********************************************************************************
@brief:Request SMS server centre address
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireSmsCenter(HWM pWM_Handle,     _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Inquire_SmsCenter))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:Set SMS server centre address
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strSca            [in]Service centre address

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetSmsCenter(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strSca)
                      
{
    return (*(pWM_Handle->pATFunction->P_Set_SmsCenter))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strSca);
    
} 



/***********************************************************************************
@brief:Inquire cell broadcast messages status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireBroadcast(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Inquire_Broadcast))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
    
}



/***********************************************************************************
@brief:Select cell broadcast messages
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strMode            [in]Accepts messages or not, 0: accept
*@param    strMids            [in]Combinations of CBM message IDs
*@param    strDcss            [in]Combinations of CBM data coding schemes

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SelectBroadcast(HWM pWM_Handle, _UINT32 serviceHandle,  
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle,    
                         _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss)
                         
                         
{
    return (*(pWM_Handle->pATFunction->P_Select_Broadcast))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strMode, strMids, strDcss);
    
}


/***********************************************************************************
@brief:Select message service
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strService        [in]GMS service

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SelectMsgService(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strService)
                          
{
    return (*(pWM_Handle->pATFunction->P_Select_MsgService))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strService);
    
    
}




/***********************************************************************************
@brief:Switch from data mode or PPP online mode to command mode
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SwitchDataToAtMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                            
{
    return (*(pWM_Handle->pATFunction->P_SwitchData_ToAtMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:Send AT
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_Attention(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Attention))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
    
}



/***********************************************************************************
@brief:Answer a call
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_AnswerCall(HWM pWM_Handle, _UINT32 serviceHandle,
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                    
{
    return (*(pWM_Handle->pATFunction->P_Answer_Call))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
    
}



/***********************************************************************************
@brief:Enable/Disable command echo
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    bOnOff            [in]0: off, 1: on

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetCommandEcho(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bOnOff)
{
    return (*(pWM_Handle->pATFunction->P_Set_CommandEcho))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bOnOff);
}



/***********************************************************************************
@brief:Disconnect existing connection
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_DisconnectCall(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                        
{
    return (*(pWM_Handle->pATFunction->P_Disconnect_Call))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:  Display product/additional identification information
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]value

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_GetProductID(HWM pWM_Handle, _UINT32 serviceHandle, 
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
{
    return (*(pWM_Handle->pATFunction->P_Get_ProductID))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
    
}




/***********************************************************************************
@brief:Switch from command mode to data mode / PPP online mode
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SwitchAtToDataMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                            
{
    return (*(pWM_Handle->pATFunction->P_SwitchAt_ToDataMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:  Set result code presentation mode
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]0: DCE transmits result code, 1: off

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetResultPresentation(HWM pWM_Handle, _UINT32 serviceHandle,
                               T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                               
{
    return (*(pWM_Handle->pATFunction->P_Set_ResultPresentation))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
    
}



/***********************************************************************************
@brief:  Set result code format mode
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]0: response with numeric code, 1: verbose code

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetResultFormat(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                         
{
    return (*(pWM_Handle->pATFunction->P_Set_ResultFormat))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
    
}



/***********************************************************************************
@brief:Set module to manufacture default parameter
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetManufDefault(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                         
{
    return (*(pWM_Handle->pATFunction->P_Set_ManufDefault))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
    
}

_UINT32 WM_InquireCCID(HWM pWM_Handle, _UINT32 serviceHandle,
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_CCID))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

_UINT32 WM_InquireIMSI(HWM pWM_Handle, _UINT32 serviceHandle,
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_IMSI))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

/***********************************************************************************
@brief:Request TA serial number identification(IMEI)
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireIMEI(HWM pWM_Handle, _UINT32 serviceHandle,
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                     
{
    return (*(pWM_Handle->pATFunction->P_Inquire_IMEI))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

_UINT32 WM_SetIMEI(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strIMEI)
                     
{
    return (*(pWM_Handle->pATFunction->P_Set_IMEI))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strIMEI);
}

_UINT32 WM_LockBS(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strBS)
                     
{
    return (*(pWM_Handle->pATFunction->P_Lock_BS))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strBS);
}

/***********************************************************************************
@brief:  Set fixed local rate
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strRate            [in]baudrate

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetBaudrate(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strRate)
                     
{
    return (*(pWM_Handle->pATFunction->P_Set_Baudrate))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strRate);
}



/***********************************************************************************
@brief:Set Band
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireBand(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                     
{
    return (*(pWM_Handle->pATFunction->P_Inquire_Band))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
    
}



/***********************************************************************************
@brief: Set Band
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strMode            [in]Mode

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetNetBand(HWM pWM_Handle, _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strMode)
                    
{
    return (*(pWM_Handle->pATFunction->P_Set_NetBand))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strMode);
}




/***********************************************************************************
@brief:Get the current ACM(accumulated call meter) value
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_GetCurrentAcm(HWM pWM_Handle, _UINT32 serviceHandle, 
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                       
{
    return (*(pWM_Handle->pATFunction->P_Get_CurrentAcm))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief: Resets the Advice of Charge related to the ACM value in SIM file EF
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strPasswd        [in]SIM PIN2

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ResetAcm(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPasswd)
                  
{
    return (*(pWM_Handle->pATFunction->P_Reset_Acm))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPasswd);
    
}



/***********************************************************************************
@brief:Get ACMmax(accumulated call meter maximum) value
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_GetAcmMax(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Get_AcmMax))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:Sets the Advice of Charge related to the ACMmax value in SIM file EF
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strAcmax        [in]Three bytes of the max. ACM value in hexadecimal format
*@param    strPasswd        [in]SIM PIN2

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetAcmMax(HWM pWM_Handle, _UINT32 serviceHandle,
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                   _CHAR8* strAcmax, _CHAR8* strPasswd)
                   
{
    return (*(pWM_Handle->pATFunction->P_Set_AcmMax))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strAcmax, strPasswd);
    
}



/***********************************************************************************
@brief:Get the current call meter value
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_GetCurCallMeter(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                         
{
    return (*(pWM_Handle->pATFunction->P_Get_CurCallMeter))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:Set or inquire call forward
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    cfData            [in]call forward setting data

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CallForward(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_CCFC_FWDCALLS *cfData)
                     
{
    return (*(pWM_Handle->pATFunction->P_Call_Forward))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, cfData);
    
}



/***********************************************************************************
@brief:Inquire call forward
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireCallFwd(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_CallFwd))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, reason, Class);
}


/***********************************************************************************
@brief:Inquire Real Time Clock
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireClock(HWM pWM_Handle, _UINT32 serviceHandle, 
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                      
{
    return (*(pWM_Handle->pATFunction->P_Inquire_Clock))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}




/***********************************************************************************
@brief:Set Real Time Clock
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetClock(HWM pWM_Handle, _UINT32 serviceHandle,
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                  RIL_SYSTIME *SysTime)
{
    return (*(pWM_Handle->pATFunction->P_Set_Clock))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, SysTime);
}

/***********************************************************************************
@brief: Set or inquire call waiting
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strN            [in]Sets/shows the presentation mode of URC, 0: disable
*@param      strMode            [in]0: disable, 1: enable, 2: query status
*@param      eClass            [in]class

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetCallWait(HWM pWM_Handle, _UINT32 serviceHandle,
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                  _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass)
                  
{
    return (*(pWM_Handle->pATFunction->P_SetCall_Wait))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bURCEnable, bEnable, eClass);
    
}



/***********************************************************************************
@brief: Inquire call forward
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireCallWait(HWM pWM_Handle, _UINT32 serviceHandle,
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_DATA_CLASS eClass)
                         
{
    return (*(pWM_Handle->pATFunction->P_Inquire_CallWait))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, eClass);
    
}


/***********************************************************************************
@brief: Get extended error report
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_GetExtendedError(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Get_ExtendedError))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

_UINT32 WM_GetFunctionality(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)                         
{
    return (*(pWM_Handle->pATFunction->P_Get_Functionality))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

_UINT32 WM_GetSimVol(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)                         
{
    return (*(pWM_Handle->pATFunction->P_Get_SimVol))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

_UINT32 WM_SetFunctionality(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strFun)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_Functionality))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strFun);  
}

_UINT32 WM_SetQSimVol(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strMod)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_SimVol))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strMod);  
}

_UINT32 WM_SetRingBack(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_RingBackCfg))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strEn);  
}

_UINT32 WM_SetVBatt(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_VBatt))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strEn);  
}

_UINT32 WM_SetNWScanMode(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strStr)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_NWScanMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strStr);  
}

_UINT32 WM_SetOpenTCPUDP(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strStr)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_OpenTCPUDP))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strStr);  
}

_UINT32 WM_InquireQiStat(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_InquireQiStat))(pWM_Handle, serviceHandle, serviceType,
        customHandle);
}

_UINT32 WM_InquireQiSack(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_InquireQiSack))(pWM_Handle, serviceHandle, serviceType,
        customHandle);
}

_UINT32 WM_InquireQiLocIP(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_InquireQiLocIP))(pWM_Handle, serviceHandle, serviceType,
        customHandle);
}

_UINT32 WM_InquireQiRcvData(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
{
    return (*(pWM_Handle->pATFunction->P_InquireQiRcvData))(pWM_Handle, serviceHandle, serviceType,
        customHandle, strValue);
}

_UINT32 WM_SendQiClose(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_SendQiClose))(pWM_Handle, serviceHandle, serviceType,
        customHandle);
}

_UINT32 WM_SetQindi(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)                
{
    return (*(pWM_Handle->pATFunction->P_Set_Qindi))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}    

_UINT32 WM_SetQidnsip(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
{
    return (*(pWM_Handle->pATFunction->P_Set_Qidnsip))(pWM_Handle, serviceHandle, serviceType,
    customHandle, strValue);
}

_UINT32 WM_SetCnetScan(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)                
{
    return (*(pWM_Handle->pATFunction->P_Set_CnetScan))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}  

_UINT32 WM_InquireCellLoc(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_CellLoc))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

_UINT32 WM_InquireQIRegApp(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)                      
{
    return (*(pWM_Handle->pATFunction->P_InquireQIRegApp))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

_UINT32 WM_SetQIRegApp(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strStr)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QIRegApp))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strStr);  
}

_UINT32 WM_SetQICsgp(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strStr)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QICsgp))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strStr);  
}

_UINT32 WM_SetQIAct(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QIAct))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

_UINT32 WM_SetQIDeact(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QIDeact))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

_UINT32 WM_SetQFun(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strFun)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QFun))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strFun);  
}

_UINT32 WM_SetQMoStat(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QMoStat))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strEn);
    
}

_UINT32 WM_SetQCCINFOEn(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QCCINFOEn))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strEn);
}

_UINT32 WM_SendSimData(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strSimData)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_FSendSimData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strSimData);
    
}

_UINT32 WM_SetQURCCFG(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QURCCFG))(pWM_Handle, serviceHandle, serviceType, customHandle);
    
}

_UINT32 WM_SetQJamDet(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn)                      
{
    return (*(pWM_Handle->pATFunction->P_Set_QJamDet))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strEn);
    
}

/***********************************************************************************
@brief: Call hold and multiparty
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    sValue            [in]value

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_HoldAndMultiparty(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle, T_ECALL_OPERATION operation, _USHORT16 index)
{
    return (*(pWM_Handle->pATFunction->P_HoldAnd_Multiparty))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, operation, index);
}


/***********************************************************************************
@brief: Inquire international mobile subscriber identity
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireSubscriber(HWM pWM_Handle,     _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                           
{
    return (*(pWM_Handle->pATFunction->P_Inquire_Subscriber))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:List current calls of ME
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

*@return    _UINT32                
*@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ListCurCalls(HWM pWM_Handle, _UINT32 serviceHandle, 
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                      
{
    return (*(pWM_Handle->pATFunction->P_List_CurCalls))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:Facility lock
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strFac            [in]Facility
*@param      strMode            [in]0: disable, 1: enable, 2: query status
*@param      strPasswd        [in]password
*@param      eClass            [in]class

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_FacilityLock(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                      RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass)
{
    return (*(pWM_Handle->pATFunction->P_Facility_Lock))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, fac, bLock, strPasswd, eClass);
}

_UINT32 WM_InqFacilityLock(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                      RIL_FACILITY fac, RIL_DATA_CLASS eClass)
{
    return (*(pWM_Handle->pATFunction->P_InqFacility_Lock))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, fac, eClass);
}

/***********************************************************************************
@brief:Calling line identification presentation
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]1: display URC, 0: suppress

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CallingLineId(HWM pWM_Handle, _UINT32 serviceHandle, 
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                       
{
    return (*(pWM_Handle->pATFunction->P_Calling_LineId))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue); 
    
}



/***********************************************************************************
@brief:Check the state of Calling line identification presentation
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CheckCLIP(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Check_CLIP))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:Check the state of Calling line identification restriction
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CheckCLIR(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Check_CLIR))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

/***********************************************************************************
@brief:Check the state of Calling line identification restriction
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetCLIRActive(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Set_CLIRActive))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:Check the state of Calling line identification restriction
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetCLIRDisable(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Set_CLIRDisable))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

/***********************************************************************************
@brief:Dailcall without ";"
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]1: display URC, 0: suppress

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_DailCLIR(HWM pWM_Handle, _UINT32 serviceHandle,
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                  
{
    return (*(pWM_Handle->pATFunction->P_Dail_CLIR))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}



/***********************************************************************************
@brief:Check the state of COLR
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CheckCOLR(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                   
{
    return (*(pWM_Handle->pATFunction->P_Check_COLR))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

_UINT32 WM_SetSIMSlot(HWM pWM_Handle, _UINT32 serviceHandle,
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 simslot)
                       
{
    return (*(pWM_Handle->pATFunction->P_Set_SetSIMSlot))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, simslot);
    
}


/***********************************************************************************
@brief:Set loudspeaker volume level
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    volume            [in]volume level

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetSpeakerVol(HWM pWM_Handle, _UINT32 serviceHandle,
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 volume)
                       
{
    return (*(pWM_Handle->pATFunction->P_Set_SpeakerVol))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, volume);
    
}

_UINT32 WM_SetFreqBand(HWM pWM_Handle, _UINT32 serviceHandle,
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 bandindex)
                       
{
    return (*(pWM_Handle->pATFunction->P_Set_FreqBand))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bandindex);
    
}

/***********************************************************************************
@brief: Set QSfr Param
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetQSFR(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 sfrindex)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_QSFRParam))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, sfrindex);
}

/***********************************************************************************
@brief:Set loudspeaker volume level
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]0: disable result code, 1: enable, use numeric values,
2: enable, use verbose values

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ReportMobileError(HWM pWM_Handle, _UINT32 serviceHandle,
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                           
{
    return (*(pWM_Handle->pATFunction->P_Report_MobileError))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}


/***********************************************************************************
@brief:Enter multiplex mode
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_EnterMultiplex(HWM pWM_Handle, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Enter_Multiplex))(pWM_Handle, serviceHandle, serviceType, customHandle);
}


/***********************************************************************************
@brief:Read operator names
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ReadOperatorNames(HWM pWM_Handle, _UINT32 serviceHandle,
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                           
{
    return (*(pWM_Handle->pATFunction->P_Read_OperatorNames))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:Inquire the current mode and the currently used operator(if registered)
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireCurOperator(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                            
{
    return (*(pWM_Handle->pATFunction->P_Inquire_CurOperator))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

_UINT32 WM_InquireBSInfo(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                            
{
    return (*(pWM_Handle->pATFunction->P_Inquire_BSInfo))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

/***********************************************************************************
@brief:Inquiry all the available operators
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireAvaiOperator(HWM pWM_Handle, _UINT32 serviceHandle, 
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                             
{
    return (*(pWM_Handle->pATFunction->P_Inquire_AvaiOperator))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief: Operator selection
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param _BOOL bAuto    [in]EOS_TRUE:自动选择并注册网络，手动选择注册网络
*@param strOper            [in]网络ID号，只在收到选择网络时需要提供

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SelectOperator(HWM pWM_Handle, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                        RIL_NET_MODE mode, _CHAR8* strOper)
{
    return (*(pWM_Handle->pATFunction->P_Select_Operator))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, mode, strOper);
}


_UINT32 WM_SetEngineerMode(HWM pWM_Handle, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                        _UINT32 mode, _UINT32 dump)
{
    return (*(pWM_Handle->pATFunction->P_Set_EngMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, mode, dump);
}

/***********************************************************************************
@brief: Read current phone book entries
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   begin            [in]location number where reading starts
*@param      end                [in]location number where reading ends. Set it as 0 if needn't

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ReadPbEntries(HWM pWM_Handle, _UINT32 serviceHandle,
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                       RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end)
                       
{
    return (*(pWM_Handle->pATFunction->P_Read_PbEntries))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, storage, begin, end);
    
}

/***********************************************************************************
@brief:  Inquire phone book entry space
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquirePbSpace(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_PB_STORAGE storage)
                        
{
    return (*(pWM_Handle->pATFunction->P_Inquire_PbSpace))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, storage);
}



/***********************************************************************************
@brief: Write a phone book entry to the specific phone book "index"
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   index            [in]If "index" equal or less than 0, then write this entry to the first free space
*@param      strFormat        [in]telephone number. Set this parameter as NULL to delete a entry
*@param      strText            [in]name in unicode format

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_WritePbEntry(HWM pWM_Handle, _UINT32 serviceHandle, 
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                      RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText)
                      
{
    return (*(pWM_Handle->pATFunction->P_Write_PbEntry))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, storage, index, strNumber, strText);
    
}



/***********************************************************************************
@brief: Inquire PIN status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquirePinStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Inquire_PinStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}

_UINT32 WM_InquireInitStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Inquire_InitStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief: Enter PIN
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strPIN            [in]password
*@param      strNewPIN        [in]if strPIN is PUK, a new PIN need to be input here

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_EnterPIN(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                  _CHAR8* strPIN, _CHAR8* strNewPIN)
                  
                  
{
    return (*(pWM_Handle->pATFunction->P_Enter_PIN))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strPIN, strNewPIN);
    
}



/***********************************************************************************
@brief: Change password
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param      strOldPwd        [in]Password specified for the facility
*@param      strNewPwd        [in]Password specified for the facility

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ChangePassword(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                        RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd)
{
    return (*(pWM_Handle->pATFunction->P_Change_Password))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, fac, strOldPwd, strNewPwd);
}


/***********************************************************************************
@brief:Network registration
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]0: disable URCs, 1,2: enable URC

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_NetworkRegistration(HWM pWM_Handle, _UINT32 serviceHandle,
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                             
{
    return (*(pWM_Handle->pATFunction->P_Network_Registration))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}

/***********************************************************************************
@brief:Inquire Network Registration Status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strValue        [in]must be null

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireNetworkRegStatus(HWM pWM_Handle, _UINT32 serviceHandle,
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)                            
{
    return (*(pWM_Handle->pATFunction->P_Inquire_NetworkRegStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}

_UINT32 WM_InquireGNetworkRegStatus(HWM pWM_Handle, _UINT32 serviceHandle,
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)            
{
    return (*(pWM_Handle->pATFunction->P_Inquire_GNetworkRegStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}

/***********************************************************************************
@brief:Set TE character set
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param    strChset        [in]character set

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetCharacterSet(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strChset)
                         
{
    return (*(pWM_Handle->pATFunction->P_Set_CharacterSet))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strChset);
    
}



/***********************************************************************************
@brief: Get the signal quality
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireSignalQuality(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_SignalQuality))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

//weikk 2016.3.31 add xreg
_UINT32 WM_InquireNetType(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_NetType))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}
//weikk 20161026 add ADC
_UINT32 WM_InquireADC(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_ADC))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

//weikk 20160713 add mmad
_UINT32 WM_InquireMMAD(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_MMAD))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}


_UINT32 WM_InquireSideTone(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_SideTone))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

_UINT32 WM_InquireCallStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_CallStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

/***********************************************************************************
@brief: Set supplementary service notifications
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strCSSI            [in]cssi value
*@param      strCSSU            [in]cssu value

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetSSNote(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                   _CHAR8* strCSSI, _CHAR8* strCSSU)
{
    return (*(pWM_Handle->pATFunction->P_Set_SSNote))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strCSSI, strCSSU);
    
}



/***********************************************************************************
@brief: Send Unstructured supplementary service data
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strN            [in]0: Disable the result code, 
1: Enable the result code, 2: Cancel session
*@param      strStr            [in]USSD-string
*@param      strDcs            [in]Cell Broadcast Data Coding Scheme

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SendUSSD(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                  _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs)
                  
{
    return (*(pWM_Handle->pATFunction->P_Send_USSD))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strN, strStr, strDcs);
    
}


/***********************************************************************************
@brief: Send USSD data
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strUssdString    [in]USSD string

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SendUssdData(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strUssdString)
                      
{
    return (*(pWM_Handle->pATFunction->P_Send_UssdData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strUssdString);
    
}

_UINT32 WM_SendATOrigin(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strATString)
                      
{
    return (*(pWM_Handle->pATFunction->P_Send_ATOrigin))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strATString);
    
}


/***********************************************************************************
@brief: Cancel USSD data
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CancelUssdData(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                        
{
    return (*(pWM_Handle->pATFunction->P_Cancel_UssdData))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief: Send DTMF string
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strDtmfStr        [in]DTMF string

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_DialDTMF(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strDtmfStr)                 
{
    return (*(pWM_Handle->pATFunction->P_Dial_DTMF))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strDtmfStr);  
}

_UINT32 WM_SetDTMFPrd(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strDtmfStr)                
{
    return (*(pWM_Handle->pATFunction->P_Set_DTMFPrd))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strDtmfStr);  
}

_UINT32 WM_SetCancel(HWM pWM_Handle, _USHORT16 chID)
{
    return (*(pWM_Handle->pATFunction->P_Set_Cancel))(chID);
}

/***********************************************************************************
@brief:  Cancel dail
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CancelDial(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Cancel_Dial))(pWM_Handle, serviceHandle,
        serviceType, customHandle);
}

/***********************************************************************************
@brief: Set alarm time
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   alarmData        [in]alarm parameter

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetAlarm(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_CALA_ALARM *alarmData)
                  
{
    return (*(pWM_Handle->pATFunction->P_Set_Alarm))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, alarmData);
    
}


/***********************************************************************************
@brief: Get field test report
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strMode            [in]mode

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_FieldTestReport(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strMode)
                         
{
    return (*(pWM_Handle->pATFunction->P_Field_TestReport))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strMode);
    
}



/***********************************************************************************
@brief: Read SMS message and do not change the SMS stat
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strIndex        [in]SMS index

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ReadSMSUnchgStat(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index)
                          
{
    return (*(pWM_Handle->pATFunction->P_Read_SMSUnchgStat))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, index);
    
}



/***********************************************************************************
@brief:  Switch off mobile station
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SwitchOffModule(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_SwitchOff_Module))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}


/***********************************************************************************
@brief: Set microphone gain level
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   channel            [in]main microphone or earphone microphone

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetMicGainLevel(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 volume)                 
{
    return (*(pWM_Handle->pATFunction->P_Set_MicGainLevel))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, volume);
    
}


/***********************************************************************************
@brief: Enable/disable earphone
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strMode            [in]1: insert, 0: out

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetAudioPath(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,RIL_AUDIO_PATH_TYPE Mode)
{
    return (*(pWM_Handle->pATFunction->P_SetAudioPath))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, Mode);
}

_UINT32 WM_SetAudioMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,RIL_AUDIO_MOD_TYPE Mode)
{
    return (*(pWM_Handle->pATFunction->P_SetAudioMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, Mode);
}

_UINT32 WM_SetVoiceHangupCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bEnable)
{
    return (*(pWM_Handle->pATFunction->P_SetVoiceHangupCtrl))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bEnable);
}


/***********************************************************************************
@brief: Mute Audio
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param bMute            [in]EOS_TRUE: mute on; EOS_FALSE: mute off
  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetAudioMute(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,_BOOL bMute)
{
    return (*(pWM_Handle->pATFunction->P_SetAudioMute))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bMute);
}

/***********************************************************************************
@brief: Play tone
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param type            [in]tone type
*@param volume            [in]volume
*@return    _UINT32                
*@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_PlayTone(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,_USHORT16 type, _USHORT16 volume)
{
    return (*(pWM_Handle->pATFunction->P_PlayTone))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, type, volume);
}

/***********************************************************************************
@brief:  Inquire headset status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireHeadsetStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                              
{
    return (*(pWM_Handle->pATFunction->P_Inquire_HeadsetStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:  Inquire headset button status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireHeadsetBtnStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                                 T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Inquire_HeadsetBtnStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:  Get the battery level
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireBatteryLevel(HWM pWM_Handle, _UINT32 serviceHandle, 
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                             
{
    return (*(pWM_Handle->pATFunction->P_Inquire_BatteryLevel))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:  Inquire PIN remain count
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquirePinCount(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle,_UCHAR8 type)
                         
{
    return (*(pWM_Handle->pATFunction->P_Inquire_PinCount))(pWM_Handle, serviceHandle, serviceType, 
        customHandle,type);
    
}



/***********************************************************************************
@brief:  Inquire PUK remain count
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquirePukCount(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                         
{
    return (*(pWM_Handle->pATFunction->P_Inquire_PukCount))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:  Dump BroadCast
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_DumpBroadCast(HWM pWM_Handle, _UINT32 serviceHandle, 
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                       
{
    return (*(pWM_Handle->pATFunction->P_Dump_BroadCast))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief: Ringer Sound Level
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   level            [in]volume

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetRingerSoundLevel(HWM pWM_Handle, _UINT32 serviceHandle, 
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 level)
                             
{
    return (*(pWM_Handle->pATFunction->P_Set_RingerSoundLevel))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, level);
    
}


/***********************************************************************************
@brief: Echo cancellation control
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strAudioParam    [in]audio parameter

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetAudioOutput(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strAudioParam)
                        
{
    return (*(pWM_Handle->pATFunction->P_Set_AudioOutput))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strAudioParam);
    
}



/***********************************************************************************
@brief: SMS Full URC Enable
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strValue        [in]value

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetSmsFullUrc(HWM pWM_Handle, _UINT32 serviceHandle, 
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                       
{
    return (*(pWM_Handle->pATFunction->P_Set_SmsFullUrc))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}


/***********************************************************************************
@brief:  query module state
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_QueryModuleState(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Query_ModuleState))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief:  AT+COLP=?
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_InquireCOLP(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                     
{
    return (*(pWM_Handle->pATFunction->P_Inquire_COLP))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}




/***********************************************************************************
@brief: Set Colp Status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strValue        [in]value

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetColpStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                       
{
    return (*(pWM_Handle->pATFunction->P_Set_ColpStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}



/***********************************************************************************
@brief: Check latest URC event 
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strIndex        [in]index

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_CheckLatestURCEvent(HWM pWM_Handle, _UINT32 serviceHandle, 
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strIndex)
{
    return (*(pWM_Handle->pATFunction->P_Check_LatestURCEvent))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strIndex);
    
}



/***********************************************************************************
@brief: control whether URC indicate to AK23xx
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strValue        [in]value

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ControlURCIndication(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                              
{
    return (*(pWM_Handle->pATFunction->P_Control_URCIndication))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
    
}



/***********************************************************************************
@brief: control module audio driver including path,handfree and mute 
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   mode            [in]mode

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ControlModuleDriver(HWM pWM_Handle, _UINT32 serviceHandle, 
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _UCHAR8 mode)
                             
{
    return (*(pWM_Handle->pATFunction->P_Control_ModuleDriver))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, mode);
    
}


/***********************************************************************************
@brief: Query the URC indicate to AK23xx
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_QueryURCIndication(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                            
{
    return (*(pWM_Handle->pATFunction->P_Query_URCIndication))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}


/***********************************************************************************
@brief: Mobile equipment event reporting
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strValue        [in]value

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_MobileEquipmentEvent(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle,_CHAR8* strValue)
                              
{
    return (*(pWM_Handle->pATFunction->P_Mobile_EquipmentEvent))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}


/***********************************************************************************
@brief: only send "at"
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SendAT(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                
{
    return (*(pWM_Handle->pATFunction->P_Send_AT))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

_UINT32 WM_PwrCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                
{
    return (*(pWM_Handle->pATFunction->P_PwrCtrl))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

/***********************************************************************************
@brief:set ms storage type
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strService        [in]service type

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetMSStorageType(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strService)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_MSStorageType))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strService);
    
}



/***********************************************************************************
@brief:set ext sms storage status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strService        [in]service type

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetExtSMSStorageStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                                T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bFull)
                                
{
    return (*(pWM_Handle->pATFunction->P_Set_ExtSMSStorageStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bFull);
    
}



/***********************************************************************************
@brief: forbid modem to sleep
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetModuleSleep(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle,_UCHAR8 enable)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_ModuleSleep))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, enable);
    
}


/***********************************************************************************
@brief: get modem version information
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_GetModemVersionNum(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Get_ModemVersionNum))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
    
}



/***********************************************************************************
@brief:set ext sms storage status
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strStep            [in]'0'~'7' (400mA~1100mA)

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SetChargingCurrent(HWM pWM_Handle, _UINT32 serviceHandle, 
                            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strStep)
{
    return (*(pWM_Handle->pATFunction->P_Set_ChargingCurrent))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strStep);
    
}

/***********************************************************************************
@brief:Indicator control use for ulc2
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   strValue        [in]

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号
    
***********************************************************************************/
_UINT32  WM_ConfigURCMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                               T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_URC_MODE mode)
{
    return (*(pWM_Handle->pATFunction->P_ConfigURCMode))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, mode);
}
/***********************************************************************************
@brief:URC control for ulc2
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   xdrvType        [in]

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号
    
***********************************************************************************/
_UINT32  WM_XDRVURCControl(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_sXDRV_TYPE *xdrvType)
{
    
    return (*(pWM_Handle->pATFunction->P_XDRV_URCControl))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, xdrvType);
    
}
/***********************************************************************************
@brief:control ULC2 audio volume 
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   audCtrlType        [in]
*@param   strValue        [in]

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号
    
***********************************************************************************/
_UINT32  WM_XDRVAudioControl(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue)
{
    return (*(pWM_Handle->pATFunction->P_XDRV_AudioControl))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, audCtrlType, strValue);
}
/***********************************************************************************
@brief:control infineon trace output 
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param bOnOff            [in]EOS_TRUE-enable trace output, EOS_FALSE-disable trace output

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号
    
***********************************************************************************/
_UINT32  WM_SetTraceOutput(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bOnOff)
{
    return (*(pWM_Handle->pATFunction->P_Set_TraceOutput))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, bOnOff);
}
/***********************************************************************************
@brief:read exception log of modem  
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  
    *@return    _UINT32                
    *@retval    >0                    该AT命令在AT命令队列中的流水号
    
***********************************************************************************/
_UINT32  WM_GetModemExcLog(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_Get_ModemExcLog))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}

/***********************************************************************************
@brief:read exception log of modem  
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   enable            [in]    
"EOS_TRUE"-new message will keep unread after reading
"EOS_FALSE"- new message state is read after reading

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
  
***********************************************************************************/
_UINT32  WM_XDRVControlNewSMSStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL enable)
{
    return (*(pWM_Handle->pATFunction->P_XDRV_ControlNewSMSStatus))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, enable);
}

/***********************************************************************************
@brief:enable or disable audio loop test
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄
*@param   enable            [in]    
  "EOS_TRUE"-enable audio loop test
  "EOS_FALSE"-disable audio loop test
 * @return _VOID 
  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号
  
***********************************************************************************/
_UINT32  WM_XDRVControlAudioLoopTest(HWM pWM_Handle, _UINT32 serviceHandle, 
                                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL enable)
{
    return (*(pWM_Handle->pATFunction->P_XDRV_ControlAudioLoopTest))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, enable);
}

/***********************************************************************************
*@brief: If the new incoming message need acknowledge
*@return    _BOOL         true: need acknowledge
*@author  MIAOBAOLI
*@date    2007.11.26
***********************************************************************************/        
_BOOL WM_NewMsgNeedAck(HWM pWM_Handle)
{
    return (*(pWM_Handle->pATFunction->P_NewMsgNeedAck))();
}

/***********************************************************************************
*@brief: If the ATD command return directly before call establishing.
*@return    _BOOL         true: need acknowledge
*@author  MIAOBAOLI
*@date    2007.11.26
***********************************************************************************/        
_BOOL WM_ATDReturnDirectly(HWM pWM_Handle)
{
    return (*(pWM_Handle->pATFunction->P_ATDReturnDirectly))();
}

/***********************************************************************************
*@brief: Init wireless module before PIN
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return >0                    该AT命令在AT命令队列中的流水号
  *@author  MiaoBaoli
  *@date    2007.11.26
***********************************************************************************/        
_UINT32 WM_InitBeforePIN(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_InitBeforePIN))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

/***********************************************************************************
*@brief: Init wireless module after PIN
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return >0                    该AT命令在AT命令队列中的流水号
  *@author  MiaoBaoli
  *@date    2007.11.26
***********************************************************************************/        
_UINT32 WM_InitAfterPIN(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_InitAfterPIN))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

/***********************************************************************************
*@brief: download STK profile to active STK
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return >0                    该AT命令在AT命令队列中的流水号
  *@author  MiaoBaoli
  *@date    2007.11.26
***********************************************************************************/        
_UINT32 WM_STKSetProfile(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_STKSetProfile))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

_UINT32 WM_STKSetAutoRsp(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_STKSetAutoRsp))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

_UINT32 WM_STKSetTR(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle)
{
    return (*(pWM_Handle->pATFunction->P_STKSetTR))(pWM_Handle, serviceHandle, serviceType, customHandle);
}

/***********************************************************************************
*@brief: Select STK main menu item
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return >0                    该AT命令在AT命令队列中的流水号
  *@author  MiaoBaoli
  *@date    2007.11.26
***********************************************************************************/        
_UINT32 WM_STKSelectMainMenu(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 index)
{
    return (*(pWM_Handle->pATFunction->P_STKSelectMainMenu))(pWM_Handle, serviceHandle, serviceType, customHandle, index);
}

/***********************************************************************************
*@brief: Send response for the STK proactive command
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return >0                    该AT命令在AT命令队列中的流水号
  *@author  MiaoBaoli
  *@date    2007.11.26
***********************************************************************************/        
_UINT32 WM_STKResponse(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_RES_PAR *par)
{
    return (*(pWM_Handle->pATFunction->P_STKResponse))(pWM_Handle, serviceHandle, serviceType, customHandle, par);
}

/***********************************************************************************
*@brief: If the STK detail info within URC.
*@return    _BOOL
*@author  MIAOBAOLI
*@date    2007.11.26
***********************************************************************************/        
_BOOL WM_STKDetailInfoInURC(HWM pWM_Handle)
{
    return (*(pWM_Handle->pATFunction->P_STKDetailInfoInURC))();
}

/***********************************************************************************
*@brief: Send response for the STK proactive command
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return >0                    该AT命令在AT命令队列中的流水号
  *@author  MiaoBaoli
  *@date    2007.11.26
***********************************************************************************/        
_UINT32 WM_STKGetDetailInfo(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_STK_CMD cmd)
{
    return (*(pWM_Handle->pATFunction->P_STKGetDetailInfo))(pWM_Handle, serviceHandle, serviceType, customHandle, cmd);
}

_UINT32 WM_InquireGprsState(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    return (*(pWM_Handle->pATFunction->P_InquireGprsState))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle);
}

_UINT32 WM_EstablishDataConnWithGPRS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strAPN, _BOOL bMUX, _USHORT16 step)
{
    return (*(pWM_Handle->pATFunction->P_EstablishDataConnWithGPRS))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle, strAPN, bMUX, step);
}

_UINT32 WM_EstablishDataConnWithCSD(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _USHORT16 step)
{
    return (*(pWM_Handle->pATFunction->P_EstablishDataConnWithCSD))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle, dial, user, pwd, bMUX, step);
}

_UINT32 WM_ReleaseDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step)
{
    return (*(pWM_Handle->pATFunction->P_ReleaseDataConn))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle, bMUX, step);
}

_UINT32 WM_SuspendDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step)
{
    return (*(pWM_Handle->pATFunction->P_SuspendDataConn))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle, bMUX, step);
}

_UINT32 WM_ResumeDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step)
{
    return (*(pWM_Handle->pATFunction->P_ResumeDataConn))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle, bMUX, step);
}

/**
*@brief 给模块点火
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 WM_IgniteModule(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    return (*(pWM_Handle->pATFunction->P_IgniteWM))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle);
}

/**
*@brief reset module
*@return
*/
_UINT32 WM_ResetModule(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle)
{
    return (*(pWM_Handle->pATFunction->P_ResetWM))(pWM_Handle, ManagerHandle, ManagerType, CustomHandle);
}

/**
*@brief:查询模块是否允许睡眠
*@param [in] HWM hWM 已经创建好的无线模块设备句柄
*@retval EOS_TRUE: 允许睡眠, EOS_FALSE: 不允许睡眠 
*/
_BOOL WM_IsSleepAllowed(HWM hWM)
{
    _UINT32 count;
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[hWM->uiChnlId];

    if(DATACALL_GPRS == hWM->DataConnCmd || DATACALL_CSD == hWM->DataConnCmd)
    {
        return EOS_FALSE;
    }

    for(count=0; count<hWM->ulChnlNum; count++)
    {
        if(GetCmdNumInQueue(pWirelessChannelCtl->pChnl) > 0)
        {
            return EOS_FALSE;
        }
    }
    return EOS_TRUE;
}

/**
*@brief 让模块进入睡眠模式
*@param [in] HWM hWM 已经创建好的无线模块设备句柄
*@return _VOID 
*/
_VOID WM_SleepModule(HWM hWM)
{
    /* Kinson 2011-05-10 (*(hWM->pATFunction->P_SleepModule))();*/
    hWM->bSleeping = EOS_TRUE;
}

/**
*@brief 唤醒模块
*@param [in] HWM hWM 已经创建好的无线模块设备句柄
*@return _VOID 
*/
_VOID WM_WakeupModule(HWM hWM)
{
    if(hWM->bSleeping)
    {
        /* Kinson 2011-05-10 (*(hWM->pATFunction->P_WakeupModule))();*/
        hWM->bSleeping = EOS_FALSE;
    }
}

/**
*@brief 得到模块的睡眠状态
*@param [in] HWM hWM 已经创建好的无线模块设备句柄
*@return _BOOL
*@retval EOS_TRUE: sleeping; EOS_FALSE: wakeup
*/
_BOOL WM_GetSleepState(HWM hWM)
{
    return hWM->bSleeping;
}

_BOOL FindInURCResult(const _CHAR8* result, _USHORT16 size, T_pURCRESULT pStru)
{
    _UINT32 i;
    
    if(pStru->count > 8)
    {
        return EOS_FALSE;
    }

    for(i = 0; i < pStru->count; i++)
    {
        if(strlen(pStru->info[i]) == 0)
        {
            return EOS_FALSE;
        }
        
        if(strstr(result, pStru->info[i]) != NULL)
        {
            return EOS_TRUE;
        }
    }
    return EOS_FALSE;
}

/***********************************************************************************
@brief: Set Battery Param
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_BatteryCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_BatteryParam))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}    

/***********************************************************************************
@brief: Set ToneDetect Param
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_ToneDetCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_ToneDetParam))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}

_UINT32 WM_DAICtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_DAIParam))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}
/***********************************************************************************
@brief: Select Voice Gain
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SelectVoiceGain(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)
                          
{
    return (*(pWM_Handle->pATFunction->P_Set_VoiceGain))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}    

/***********************************************************************************
@brief: Side Tone Modify
*@param    pWM_Handle        [in]指向无线模块句柄的指针
*@param    serviceHandle    [in]功能管理模块句柄
*@param    serviceType        [in]功能管理模块类型
*@param    customHandle    [in]客户信息句柄

  *@return    _UINT32                
  *@retval    >0                    该AT命令在AT命令队列中的流水号                
***********************************************************************************/
_UINT32 WM_SideToneModify(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue)      
{
    return (*(pWM_Handle->pATFunction->P_Set_SideTone))(pWM_Handle, serviceHandle, serviceType, 
        customHandle, strValue);
}    

_UINT32 WM_SaveConfigure(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle)
                          
{
    return (*(pWM_Handle->pATFunction->P_Save_Configuration))(pWM_Handle, serviceHandle, serviceType, 
        customHandle);
}    

