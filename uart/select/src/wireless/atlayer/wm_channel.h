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

/** ͨ����� */
typedef struct tagCHNL
{
    _USHORT16    ChID;                /** ͨ��ID*/
    _LIST       atcmdlist;          /** ָ��ͨ����������еľ��*/
    _LIST       atrsplist;            /** ͨ������Ӧ���еľ��*/

    _BOOL        bWaitingRsp;
    _UINT32     uiSendATTick;       /* the tick start to send at    */
    T_hTask     hSendTask;            /** ͨ���ķ����̵߳ľ��*/
    _UCHAR8        SendStack[SendTaskStackSize];
    T_hTask     hReceiveTask;        /** ͨ���Ľ����̵߳ľ��*/
    _UCHAR8        RcvStack[RcvTaskStackSize];
    _UINT32        ulQueueSize;        /** �����������ܴ�ŵ�AT���������Ŀ*/
    _UINT32        lFreeSpace;            /** ��������л��ܴ�ż���AT�����*/
    HWM            hWM;                /** ������ͨ����ģ����ָ��*/
    T_CHANNEL_MODE channelMode;

    _UINT32        ATLineLen;       /* cached line length */
    _UCHAR8        ATLineBuffer[ATLineBuffSize];
    _UCHAR8        ATBuffer[ATBufferSize];

    T_ATRESPONSE ATRsp;

    T_pURCRESULT pURC;                /**<��Ҫ���ε�URC��Ϣ*/
    T_pURCRESULT pResult;            /**<��Ҫ���ӵķ��ؽ�����־*/
} T_HCHNL;

/**
@brief:ͨ�������������ͨ��������ͨ�����
 *@param    ChID             [in]ͨ����
 *@param pWM_Handle         [in]����ģ����������URC�б�ʶģ��


 *@return     T_HCHNL*
 *@retval    ��������ͨ�����ָ��
*/
HCHNL CreateChannel(HWM pWM_Handle, _USHORT16 ChID, T_HCHNL *pChnl);


_INT32 SuspendChannel(HWM pWM_Handle, _USHORT16 ChID);
_INT32 ResumeChannel(HWM pWM_Handle, _USHORT16 ChID);

_VOID SendThread(_VOID);
_VOID ReceiveThread(_VOID);


/**
@brief:ͨ�������������ͨ��
 *@param    pChnl        [in]ͨ�����

 *@return     _UINT32
 *@retval    >0 ����ͨ���ɹ�
*/
_INT32 DestroyChannel(HCHNL hChnl);

/**
 *@briefͨ������������AT����ͨ��
 *@param    pChnl        [in]ͨ�����

 *@return     _UINT32
 *@retval    >0 ���ͨ���ɹ�
*/
_INT32 EmptyChannel(HCHNL hChnl);

/**
 *@brief �õ�ָ��ͨ��������������ݴ���������
 *@param    pChnl        [in]ͨ�����

 *@return     _INT32
 *@retval    >=0 ���������<0 ����ID
*/
_INT32 GetCmdNumInQueue(HCHNL hChnl);

/**
  *@briefͨ������ӿڽ�������AT����͵���������
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmd(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode);

/**
  *@briefͨ������ӿڽ�������AT����͵��������ͷ��
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��anywhere�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmd_ToHead(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size,  _USHORT16 *ChID, 
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode);

/**
  *@briefͨ������ӿڽ�������AT����͵���������ָ����ʱʱ��
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdWithTimeout(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout);

/**
  *@briefͨ������ӿڽ�������AT����͵��������ͷ,ָ����ʱʱ��
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��anywhere�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmd_ToHeadWithTimeout(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size,  _USHORT16 *ChID, 
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout);

/**
 @brief:ͨ������ӿ���ģ�鷢��Ӳ��������
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    CmdType                [in]hardware command type
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendHardwareCmd(HWM pWM_Handle, T_ECMD_TYPE CmdType, _USHORT16 *ChID,
                        _BOOL Notify, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode);

/**
  *@briefͨ������ӿڽ�������AT����͵���������ָ��URC,result,�ȸ�����Ϣ
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdWithAddiInfo(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout, 
                        _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult);

/**
  *@briefͨ������ӿڽ�������AT����͵��������ͷ����ָ��URC,result,�ȸ�����Ϣ
  *@param    pWM_Handle            [in]ָ������ģ������ָ��
  *@param    Cmd                    [in]����õ�AT����
  *@param    Size                [in]AT��������ݳ���
  *@param    *ChID                [in]�ڸ�ͨ���Ϸ��͸�AT����.
                                �����ֵ��RIL_CHANNEL_ANY�������ͨ���������ж����ĸ�ͨ������,����ʵ��ѡ�е�ͨ����ͨ���ò������ظ�������.
  *@param    Notify                [in]��AT�����ִ�н���Ƿ�֪ͨserver
  *@param    fParse                [in]��AT����ؽ���Ľ�������
  *@param    serviceHandle        [in]���ڽ�AT����ؽ��֪ͨ�ϲ��serviceHandle�����
                                    �ڵ��ص�����ʱ��Ϊ��������
  *@param     serviceType        [in]���ܹ���ģ������
  *@param     customHandle        [in]�ͻ���Ϣ���

  *@param    CmdID                [in]AT�����ID�ţ������ֵ��NEWCMDID����Ҫ���������ˮ��
  *@param    retryTimes            [in]AT����ִ�д���ʱ�����Դ���
  *@param    executeMode            [in]AT����ִ��ģʽ��������T_CMD_EXE_MODE

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
*/
_UINT32 SendCmdToHeadWithAddiInfo(HWM pWM_Handle, _UCHAR8 *Cmd, _UINT32 Size, _USHORT16 *ChID,
                        _BOOL Notify, PARSERESPONSE fParse, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                        _UINT32 CmdID, _USHORT16 retryTimes, _USHORT16 executeMode, _USHORT16 timeout, 
                        _BOOL ignalMode, T_pURCRESULT pURC, T_pURCRESULT pResult);

/**
 *@brief �趨ͨ����ģʽ�����ͨ����ΪPPPģʽ�󣬽����ٽ���AT���ֱ��������ΪAT����ģʽ
 *@param [in] HWM pWM_Handle ģ����
 *@param [in] _USHORT16 ChID ͨ��ID
 *@param [in] T_CHANNEL_MODE mode ͨ��ģʽ
 *@return     _VOID
*/
_VOID SetChannelMode(HWM pWM_Handle, _USHORT16 ChID, T_CHANNEL_MODE mode);

#endif
