 /**
 * @file WM_Device.h
 * @brief Define the data structures for Wirelss Model Device
 */
#ifndef WM_INTERFACE_H
#define WM_INTERFACE_H

#include "eos.h"
#include "wm_struct.h"

#define WAVECOM     1
#define Infineon    2
#define SPREADTRUM  3
#define NEWCMDID    0


typedef    struct tagWM* HWM;
typedef struct tagCHNL* HCHNL;

typedef _INT32    T_hTask;


/**
 *��result callback�����л���ݸ������ж�,��AT�������͸��Ǹ����ܹ���ģ�� conference
 */
typedef enum tagDeviceType 
{
    eDEVICECOMMON = 0,
    eDEVICECALL,
    eDEVICESMS,
    eDEVICESTK,
    eDEVICESIM,
    eDEVICEDATACONN
}T_EDEVICEETYPE;


typedef enum
{
    eCALL_DIAL = 0,
    eCALL_ANSWER,
    eCALL_HANGUP,
    eCALL_ENDHOLD,
    eCALL_ENDACTIVE,
    eCALL_ENDSINGLE,
    eCALL_HOLDACTIVE,
    eCALL_HOLDACTIVEEXP,
    eCALL_CONFERENCE
}T_ECALL_OPERATION;

typedef enum
{
    DATACALL_NULL = 0,
    DATACALL_GPRS,
    DATACALL_CSD
}T_DATACALL_TYPE;

typedef enum
{
    DATACALL_ESTABLISHGPRS = 0,
    DATACALL_ESTABLISHCSD,
    DATACALL_RELEASE,
    DATACALL_SUSPEND,
    DATACALL_RESUME
}T_DATACALL_OPERATION;


typedef enum
{
    CMDTYPE_AT = 0,
    CMDTYPE_RESETWM,
    CMDTYPE_IGNITE,
    CMDTYPE_POWEROFF,
    CMDTYPE_RESETMODULE
}T_ECMD_TYPE;

typedef enum
{
    EXE_NORMAL = 0,            /**<˳��ִ��*/
    EXE_STOPWHENERROR,    /**<��ATִ��ʧ��ʱ���������أ���AT���������и���AT����ͬ��CMD ID��AT�����ٱ�ִ��*/
    EXE_STOPWHENOK        /**<��ATִ�гɹ�ʱ���������أ���AT���������и���AT����ͬ��CMD ID��AT�����ٱ�ִ��*/
}T_CMD_EXE_MODE;

typedef struct 
{
    _UINT32 count;
    _CHAR8  info[10][16];
}T_URCRESULT, *T_pURCRESULT;

/**AT�������������ָ��
typedef _VOID* (*PARSERESPONSE)(RIL_PARSE_RES* result_type, const _CHAR8* strResult, _UINT32* strNum, _UINT32 *strSize);
*/
typedef void* (*PARSERESPONSE)(RIL_PARSE_RES* result_type, const char* strResult, unsigned int* strNum, unsigned int *strSize);

/** AT��Ӧ�ص�������ָ�� */
typedef _UINT32 (*ATCALLBACK)(_UINT32 ChannelId, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customHandle);

/** URC�ص�������ָ�� */
typedef _UINT32 (*URCCALLBACK)(_UINT32 ChnID, _UINT32 eventType, _UINT32 eventID, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize);


/**������е����ݽṹ */
typedef    struct tagATCMD
{
    _NODE    stnode;
    
    _BOOL  bNotify;                /**<�Ƿ�AT����صĽ��֪ͨAT Server*/
    _USHORT16    timeout;                /**<�趨������ĳ�ʱʱ��*/
    _USHORT16   usVoid;
    T_ECMD_TYPE CmdType;            /**<����AT�����Ӳ���������*/
    _VOID* lpData;                 /**<AT����*/
    _UINT32    ulData;                    /**<AT����ĳ���*/
    _UINT32   CmdID;                    /**<AT������AT�����е���ˮ��*/
    _USHORT16    retryTimes;                /**<AT����ִ�д���ʱ���ط�����*/
    _USHORT16    executeMode;            /**<AT�����ִ��ģʽ,T_CMD_EXE_MODE����*/
    PARSERESPONSE pParseRsp;         /**<ָ����Ӧ����������ָ��*/
    _UINT32 customHandle;                /**<����Ĳ���*/
    T_EDEVICEETYPE serviceType;        /**<�豸������*/
    _UINT32  serviceHandle;            /**<�豸�ľ��,���ص�����ʱ����������AT����Ľ��֪ͨ�ϲ�*/

    T_pURCRESULT pURC;                /**<��Ҫ���ε�URC��Ϣ*/
    T_pURCRESULT pResult;            /**<��Ҫ���ӵķ��ؽ�����־*/
} T_ATCMD, *PT_ATCMD;

/**��Ӧ���е����ݽṹ */
typedef struct tagATRSP
{
    _NODE    stnode;

    _UCHAR8        *lpData;            /**<ָ��AT��Ӧ��ָ��*/
    _UINT32        bufferSize;            /**<����AT��Ӧ�Ļ������Ĵ�С*/
    _UINT32        bNeedFree;            /**<�û������Ƿ���Ҫ�ͷ�*/
    _UINT32        ulData;                /**<AT��Ӧ�Ĵ�С*/
} T_ATRESPONSE, *PT_ATRESPONSE;

/**AT�����ָ�� */
typedef struct tagATFUNCTION
{
/*GSM0705 AT command*/
_UINT32 (*P_Delete_SMS)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strIndex);
_UINT32 (*P_Set_SmsFormat)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strMode);
_UINT32 (*P_Set_TxtSmsShow)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strMode);
_UINT32 (*P_List_SMS)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_SMS_MODE stat);
_UINT32 (*P_Read_SMS)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _USHORT16 index);
_UINT32 (*P_Read_SMS_ByTxt)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _USHORT16 index);
_UINT32 (*P_Read_SMS_CDMA)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _USHORT16 index);
_UINT32 (*P_SendSms_ByPduMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strLength);
_UINT32 (*P_SendQidata)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strLength);
_UINT32 (*P_SendSms_ByPduMode_CDMA)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strPDU);
_UINT32 (*P_SendSms_ByTxtMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strLength);
_UINT32 (*P_SaveSms_ByPduMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strLength, _UCHAR8 read_flag);
_UINT32 (*P_SendSms_TxtData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strPDU, _UINT32 uiLen);
_UINT32 (*P_SendSms_PduData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strPDU);
_UINT32 (*P_SendQiRawData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strData, _UINT32 len);
_UINT32 (*P_SaveSms_PduData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strPDU);
_UINT32 (*P_StoreNewMsgMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_STORENEWMSG_MODE mode);
_UINT32 (*P_NewSMS_Ack)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_SmsStorage)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_MSG_STORAGE storage);
_UINT32 (*P_Inquire_SmsCenter)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_SmsCenter)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strSca);
_UINT32 (*P_Inquire_Broadcast)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Select_Broadcast)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss);
_UINT32 (*P_Select_MsgService)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strService);

/*v25ter AT command */
_UINT32 (*P_SwitchData_ToAtMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Attention)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Answer_Call)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Dial_Call)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strNumber, RIL_DATA_CLASS eClass);
_UINT32 (*P_Set_CommandEcho)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bOnOff);
_UINT32 (*P_Disconnect_Call)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Get_ProductID)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_SwitchAt_ToDataMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_ResultPresentation)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Set_ResultFormat)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Set_ManufDefault)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_CCID)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_IMSI)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_IMEI)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_IMEI)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strIMEI);
_UINT32 (*P_Lock_BS)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strBS);
_UINT32 (*P_Set_Baudrate)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strRate);
_UINT32 (*P_Inquire_Band)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_NetBand)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strMode);



/*GSM0707 AT command*/
_UINT32 (*P_Get_CurrentAcm)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Reset_Acm)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strPasswd);
_UINT32 (*P_Get_AcmMax)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_AcmMax)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strAcmax, _CHAR8* strPasswd);
_UINT32 (*P_Get_CurCallMeter)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Call_Forward)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_CCFC_FWDCALLS *cfData);
_UINT32 (*P_Inquire_CallFwd)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class);
_UINT32 (*P_Inquire_Clock)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_Clock)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_SYSTIME *SysTime);
_UINT32 (*P_SetCall_Wait)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass);
_UINT32 (*P_Inquire_CallWait)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_DATA_CLASS eClass);
_UINT32 (*P_Get_ExtendedError)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Get_Functionality)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Get_SimVol)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_Functionality)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strFun);
_UINT32 (*P_Set_SimVol)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strMod);
_UINT32 (*P_Set_RingBackCfg)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strFun);
_UINT32 (*P_Set_VBatt)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strFun);
_UINT32 (*P_Set_NWScanMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr);
_UINT32 (*P_Set_OpenTCPUDP)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr);
_UINT32 (*P_InquireQiStat)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_InquireQiSack)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_InquireQiLocIP)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_InquireQiRcvData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_SendQiClose)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_Qindi)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Set_Qidnsip)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Set_CnetScan)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Inquire_CellLoc)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);/*for location*/
_UINT32 (*P_InquireQIRegApp)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_QIRegApp)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr);
_UINT32 (*P_Set_QICsgp)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStr);
_UINT32 (*P_Set_QIAct)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_QIDeact)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_QFun)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strEn);
_UINT32 (*P_Set_QMoStat)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strEn);
_UINT32 (*P_Set_QCCINFOEn)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strEn);
_UINT32 (*P_Set_FSendSimData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strSimData);
_UINT32 (*P_Set_QURCCFG)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_QJamDet)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strEn);
_UINT32 (*P_HoldAnd_Multiparty)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, T_ECALL_OPERATION operation, _USHORT16 index);
_UINT32 (*P_Inquire_Subscriber)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_List_CurCalls)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Facility_Lock)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass);
_UINT32 (*P_InqFacility_Lock)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_FACILITY fac, RIL_DATA_CLASS eClass);
_UINT32 (*P_Calling_LineId)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Check_CLIP)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Check_CLIR)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_CLIRActive)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_CLIRDisable)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Dail_CLIR)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Check_COLR)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_SetSIMSlot)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 simslot);
_UINT32 (*P_Set_SpeakerVol)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 volume);
_UINT32 (*P_Set_FreqBand)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 bandindex);
_UINT32 (*P_Set_QSFRParam)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 sfrindex);
_UINT32 (*P_Report_MobileError)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Enter_Multiplex)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Read_OperatorNames)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_CurOperator)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_BSInfo)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_AvaiOperator)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Select_Operator)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_NET_MODE mode, _CHAR8* strOper);
_UINT32 (*P_Set_EngMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _UINT32 mode, _UINT32 dump);
_UINT32 (*P_Read_PbEntries)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end);
_UINT32 (*P_Inquire_PbSpace)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_PB_STORAGE storage);
_UINT32 (*P_Write_PbEntry)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText);
_UINT32 (*P_Inquire_PinStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_InitStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Enter_PIN)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strPIN, _CHAR8* strNewPIN);
_UINT32 (*P_Change_Password)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd);
_UINT32 (*P_Network_Registration)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Inquire_NetworkRegStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Inquire_GNetworkRegStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32 (*P_Set_CharacterSet)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strChset);
_UINT32 (*P_Inquire_SignalQuality)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_NetType)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);//weikk 2016.3.31 add xreg
_UINT32 (*P_Inquire_ADC)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);//weikk 20160713 add adc
_UINT32 (*P_Inquire_MMAD)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);//weikk 20160713 add mmad


_UINT32 (*P_Inquire_SideTone)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Inquire_CallStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Set_SSNote)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strCSSI, _CHAR8* strCSSU);
_UINT32 (*P_Send_USSD)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs);
_UINT32 (*P_Send_UssdData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strUssdString);
_UINT32 (*P_Cancel_UssdData)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 (*P_Dial_DTMF)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strDtmfStr);
_UINT32 (*P_Set_DTMFPrd)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strDtmfStr);

/*extended AT command*/
_UINT32  (*P_Set_Cancel)(_USHORT16 chID);
_UINT32  (*P_Cancel_Dial)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Set_Alarm)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_CALA_ALARM *alarmData);
_UINT32  (*P_Field_TestReport)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strMode);
_UINT32  (*P_Read_SMSUnchgStat)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _USHORT16 index);
_UINT32  (*P_SwitchOff_Module)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Set_MicGainLevel)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 volume);        
_UINT32  (*P_SetAudioPath)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_AUDIO_PATH_TYPE Mode);
_UINT32  (*P_SetAudioMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_AUDIO_MOD_TYPE Mode);
_UINT32  (*P_SetVoiceHangupCtrl)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bEnable);
_UINT32  (*P_SetAudioMute)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMute);
_UINT32  (*P_PlayTone)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _USHORT16 type, _USHORT16 volume);
_UINT32  (*P_Inquire_HeadsetStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Inquire_HeadsetBtnStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Inquire_BatteryLevel)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Inquire_PinCount)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_UCHAR8 type);
_UINT32  (*P_Inquire_PukCount)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Dump_BroadCast)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Set_RingerSoundLevel)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 level);
_UINT32  (*P_Set_AudioOutput)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strAudioParam);
_UINT32  (*P_Set_SmsFullUrc)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32  (*P_Query_ModuleState)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Inquire_COLP)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Set_ColpStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32  (*P_Check_LatestURCEvent)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_CHAR8* strIndex);
_UINT32  (*P_Control_URCIndication)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strValue);
_UINT32  (*P_Control_ModuleDriver)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _UCHAR8 mode);
_UINT32  (*P_Query_URCIndication)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Mobile_EquipmentEvent)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_CHAR8* strValue);
_UINT32  (*P_Send_AT)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_PwrCtrl)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Send_ATOrigin)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strATString);
_UINT32  (*P_Set_MSStorageType)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strService);
_UINT32  (*P_Set_ExtSMSStorageStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bFull);
_UINT32  (*P_Set_ModuleSleep)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_UCHAR8 enable);
_UINT32  (*P_Get_ModemVersionNum)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Set_ChargingCurrent)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strStep);

_UINT32  (*P_ConfigURCMode)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_URC_MODE mode);
_UINT32  (*P_XDRV_URCControl)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_sXDRV_TYPE *xdrvType);
_UINT32  (*P_XDRV_AudioControl)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue);
_UINT32  (*P_Set_TraceOutput)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle,_BOOL bOnOff);
_UINT32  (*P_Get_ModemExcLog)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_XDRV_ControlNewSMSStatus)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL enable);
_UINT32  (*P_XDRV_ControlAudioLoopTest)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL enable);
_BOOL (*P_NewMsgNeedAck)(_VOID);
_BOOL (*P_ATDReturnDirectly)(_VOID);
_BOOL (*P_CheckATResult)(const _CHAR8* ATResponse, _USHORT16 size);
_UCHAR8   (*P_CheckUrcData)(_UINT32 ChnId, RIL_URC_TYPE *UrcEventType, RIL_URC_ID *UrcEventID, _VOID **UrcParse, 
                            _UINT32 *strNum, _UINT32 *strSize, const _CHAR8* Data, _USHORT16 length);
_UINT32  (*P_InitBeforePIN)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_InitAfterPIN)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);

_UINT32  (*P_STKSetProfile)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_STKSetAutoRsp)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_STKSetTR)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_STKSelectMainMenu)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _SHORT16 index);
_UINT32  (*P_STKResponse)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_RES_PAR *par);
_BOOL (*P_STKDetailInfoInURC)(_VOID);
_UINT32  (*P_STKGetDetailInfo)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, RIL_STK_CMD cmd);

_UINT32  (*P_InquireGprsState)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_EstablishDataConnWithGPRS)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strAPN, _BOOL bMUX, _USHORT16 step);
_UINT32  (*P_EstablishDataConnWithCSD)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _USHORT16 step);
_UINT32  (*P_ReleaseDataConn)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step);
_UINT32  (*P_SuspendDataConn)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step);
_UINT32  (*P_ResumeDataConn)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step);
_VOID (*P_SetDataConnMode)(HWM pWM_Handle, T_DATACALL_TYPE DataMode);

_UINT32  (*P_ResetWM)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_IgniteWM)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32  (*P_Reset)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);

_UINT32  (*P_IgniteModule)(_VOID);
_BOOL (*P_IgniteResult)(_VOID);
_UINT32  (*P_PowerOffModule)(_VOID);
_BOOL (*P_PowerOffResult)(_VOID);
_UINT32  (*P_ResetModule)(_VOID);
_BOOL (*P_ResetResult)(_VOID);
_VOID (*P_SleepModule)(_VOID);
_VOID (*P_WakeupModule)(_VOID);

_UINT32 (*P_Set_BatteryParam)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Set_ToneDetParam)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Set_DAIParam)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Set_VoiceGain)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Set_SideTone)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strParam);
_UINT32 (*P_Save_Configuration)(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);

}T_ATFUNCTION;


/**����ģ���� */
typedef    struct tagWM
{
    HCHNL        Chnl_Array[MAX_SUPPORT_CHANNELS];   /**<ָ��һ��ͨ����������ָ��*/
    _UINT32     uiChnlId;
    _UINT32        ulChnlNum;                            /**<��ģ���ͨ����Ŀ*/
    T_ATFUNCTION* pATFunction;                        /**<ָ��AT�����ָ���ָ��*/
    _UINT32        cmdID;                                /**<������¼�������ˮ��*/
    _UINT32        InvalidCmdID;                        /**<������¼��Ч��AT�����ID��*/
    T_DATACALL_TYPE        DataConnCmd;                        /**<�����������ӵ�AT����*/
    _BOOL        bInitOK;                            /**<����ģ���Ƿ��ʼ�����*/
    _BOOL        bSleeping;                            /**<����ģ���Ƿ���˯��״̬*/
} T_HWM;

/**
  *@brief �ýӿ���Ҫ�ṩ��AT Server���ã�����ʵ���������ģ�飬ͬʱ����������ģ���豸
  *@param    ChnlNum        [in]��ģ���ͨ����Ŀ
  *@param    ChID        [in]ָ��һ�����飬��������ÿ��ͨ����Ӧ��ID��
  *@param    Type        [in]ģ�������
  *@param    WM_Handle    [out]����ģ��ľ��ָ��

  *@retval    =0    �����ɹ�, <0 ����ʧ�� 
*/
_INT32 WM_Create(_UINT32 ChnlNum, _USHORT16 *ChID, _UINT32 Type, HWM *WH_Handle);

/**
  *@brief �ýӿ���Ҫ�ṩ��AT Server���ã��������������豸ģ��
  *@param    pWM_Handle            [in]�����豸ģ����
  
  *@return    _INT32                [out]�����Ƿ�ɹ�
   *@retval   ��ʾִ�гɹ�
*/
_INT32 WM_Destory(HWM pWM_Handle);

/**
*@brief:�ýӿ���Ҫ�ṩ��AT Server���ã�������������ģ���豸��ͨ����Ϣ
*@param [in] HWM hWM �Ѿ������õ�����ģ���豸���
*@param    [in] ChnlNum ���µ�ͨ����Ŀ
*@param    [in] ChID ָ��һ�����飬��������ÿ��ͨ����Ӧ��ID��
*@retval    =0    �����ɹ�, <0 ����ʧ�� 
*/
_INT32 WM_UpdateChannel(HWM hWM, _UINT32 ChnlNum, _USHORT16 *ChID);

/**
*@brief ��ģ�����쳣ʱ����Ҫ�øýӿ�ȥ��λ����ģ���豸
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 WM_Reset(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);

/**
  *@brief ͨ������ӿڽ������AT��Ӧ�ص���AT Server
  *@param    ResultCallback   [in]ָ��AT��Ӧ�ص�������ָ��
  *@param    UrcCallback          [in]ָ��URC�ص�������ָ��
  
  *@return    _VOID                             
*/
_VOID WM_SetCallback (HWM pWM_Handle, ATCALLBACK ResultCallback, URCCALLBACK UrcCallback);

/**
  *@brief ָ������ģ�������������AT����ǽ�����ͨAT����
  *@param [in] _BOOL bDataMode: EOS_TRUE ��������ģʽ
  *@return    _VOID                             
*/
_VOID WM_SetDataConnMode(HWM pWM_Handle, T_DATACALL_TYPE DataMode);

/**
*@brief ͨ������ӿ�������Ч������ID����Щ������AT�����н�����ִ��
*@return _VOID                             
*/
_VOID WM_SetCancelCmdID(HWM pWM_Handle, _UINT32 InvalidCmdID);

/**
  *@brief AT Serverͨ������ӿڵ��ò��ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@param    strNumber        [in]���Ų���
  *@param    eClass            [in]���ŵ�����

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��            
*/
_UINT32 WM_DialCall(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, 
                    _UINT32 customHandle,    _CHAR8* strNumber, RIL_DATA_CLASS eClass);


/**
  *@brief AT Serverͨ������ӿڵ���ɾ�����ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strIndex        [in]SMS index

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��            
*/
_UINT32 WM_DeleteSMS(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strIndex);


/**
  *@brief AT Serverͨ������ӿڵ������ö��Ÿ�ʽ��AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strMode            [in]SMS strMode

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetSmsFormat(HWM pWM_Handle, _UINT32 serviceHandle, 
    T_EDEVICEETYPE serviceType,    _UINT32 customHandle, _CHAR8* strMode);

_UINT32 WM_SetTxtSmsShow(HWM pWM_Handle, _UINT32 serviceHandle, 
    T_EDEVICEETYPE serviceType,    _UINT32 customHandle, _CHAR8* strMode);


/**
  *@brief AT Serverͨ������ӿڵ����г����ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    RIL_SMS_MODE stat    [in]0: Received unread; 1: Reveived read;
                                  2: stored unsend; 3: stored send; 4: All message

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ListSMS(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_SMS_MODE stat);



/**
  *@brief AT Serverͨ������ӿڵ��ö����ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    _USHORT16 index        [in]SMS index

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ReadSMS(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index);

_UINT32 WM_ReadSMS_ByTxt(HWM pWM_Handle, _UINT32 serviceHandle,
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index);

/**
  *@brief AT Serverͨ������ӿڵ��ö����ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    _USHORT16 index        [in]SMS index

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ReadSMS_CDMA(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index);


/**
  *@brief AT Serverͨ������ӿڵ���ͨ��pduģʽ���Ͷ��ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strLength        [in]SMS length

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SendSmsByPduMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strLength);


/**
  *@brief AT Serverͨ������ӿڵ���ͨ��pduģʽ������ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strLength        [in]SMS length
  *@param    read_flag        [in]0:unread sms,1:read sms

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SaveSmsByPduMode(HWM pWM_Handle, _UINT32 serviceHandle, 
    T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strLength, _UCHAR8 read_flag);


/**
  *@brief AT Serverͨ������ӿڵ��÷���pdu���ݵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strPDU            [in]PUD data

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SendSmsPduData(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPDU);


/***********************************************************************************
@brief:AT Serverͨ������ӿڵ��ñ���pdu������SIM ����AT����
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���
*@param    strPDU            [in]PUD data

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��                
***********************************************************************************/
_UINT32 WM_SaveSmsPduData(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPDU);


/**
  *@brief AT Serverͨ������ӿڵ���ָʾ�¶��ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strParam        [in]mode+mt+bm+ds+bfr

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_StoreNewMsgMode(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_STORENEWMSG_MODE mode);



/**
  *@brief AT Serverͨ������ӿڵ��û�Ӧ�¶��ŵ�AT����
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_NewSMSAck(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Get the preferfed SMS storage
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param     RIL_MSG_STORAGE storage [in]

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireSmsStorage(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                        RIL_MSG_STORAGE storage);


/**
  *@brief Request SMS server centre address
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireSmsCenter(HWM pWM_Handle,     _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Set SMS server centre address
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strSca            [in]Service centre address

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetSmsCenter(HWM pWM_Handle, _UINT32 serviceHandle,
                T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strSca);



/**
  *@brief Inquire cell broadcast messages status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireBroadcast(HWM pWM_Handle, _UINT32 serviceHandle, 
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Select cell broadcast messages
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strMode            [in]Accepts messages or not, 0: accept
  *@param    strMids            [in]Combinations of CBM message IDs
  *@param    strDcss            [in]Combinations of CBM data coding schemes
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SelectBroadcast(HWM pWM_Handle, _UINT32 serviceHandle,  
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle,    
                    _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss);



/**
  *@brief Select message service
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strService        [in]GMS service

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SelectMsgService(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strService);



/**
  *@brief Switch from data mode or PPP online mode to command mode
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SwitchDataToAtMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Send AT
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_Attention(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Answer a call
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_AnswerCall(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Enable/Disable command echo
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    bOnOff            [in]0: off, 1: on

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetCommandEcho(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bOnOff);




/**
  *@brief Disconnect existing connection
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_DisconnectCall(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief   Display product/additional identification information
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]value

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_GetProductID(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief Switch from command mode to data mode / PPP online mode
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SwitchAtToDataMode(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief   Set result code presentation mode
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]0: DCE transmits result code, 1: off

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetResultPresentation(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief   Set result code format mode
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]0: response with numeric code, 1: verbose code

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetResultFormat(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);




/**
  *@brief Set module to manufacture default parameter
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetManufDefault(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);


_UINT32 WM_InquireCCID(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_InquireIMSI(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief Request TA serial number identification(IMEI)
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireIMEI(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_SetIMEI(HWM pWM_Handle, _UINT32 serviceHandle, 
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strIMEI);

/**
  *@brief   Set fixed local rate
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strRate            [in]baudrate

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetBaudrate(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strRate);




/**
  *@brief Set Band
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireBand(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief  Set Band
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strMode            [in]Mode

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetNetBand(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strMode);




/**
  *@brief Get the current ACM(accumulated call meter) value
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_GetCurrentAcm(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief  Resets the Advice of Charge related to the ACM value in SIM file EF
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strPasswd        [in]SIM PIN2

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ResetAcm(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strPasswd);



/**
  *@brief Get ACMmax(accumulated call meter maximum) value
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_GetAcmMax(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief Sets the Advice of Charge related to the ACMmax value in SIM file EF
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strAcmax        [in]Three bytes of the max. ACM value in hexadecimal format
  *@param    strPasswd        [in]SIM PIN2

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetAcmMax(HWM pWM_Handle, _UINT32 serviceHandle,
                T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                _CHAR8* strAcmax, _CHAR8* strPasswd);



/**
  *@brief Get the current call meter value
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_GetCurCallMeter(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief Set or inquire call forward
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    cfData            [in]call forward setting data


  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CallForward(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_CCFC_FWDCALLS *cfData);



/**
  *@brief Inquire call forward
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���


  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireCallFwd(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class);



/**
  *@brief Inquire Real Time Clock
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireClock(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief Set Real Time Clock
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetClock(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle,RIL_SYSTIME *SysTime);


/**
  *@brief  Set or inquire call waiting
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strN            [in]Sets/shows the presentation mode of URC, 0: disable
  *@param      strMode            [in]0: disable, 1: enable, 2: query status
  *@param      eClass            [in]class

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetCallWait(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
        _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass);



/**
  *@brief  Inquire call forward
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireCallWait(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_DATA_CLASS eClass); 
                        


/**
  *@brief  Get extended error report
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_GetExtendedError(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);




_UINT32 WM_GetFunctionality(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle);                       

_UINT32 WM_SetFunctionality(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strFun);

_UINT32 WM_SetRingBack(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn);

_UINT32 WM_SetVBatt(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn);

_UINT32 WM_SetQFun(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strFun);

_UINT32 WM_SetQMoStat(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn);

_UINT32 WM_SetQCCINFOEn(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strEn);

_UINT32 WM_SetQURCCFG(HWM pWM_Handle, _UINT32 serviceHandle,
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief  Call hold and multiparty
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    sValue            [in]value

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_HoldAndMultiparty(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, T_ECALL_OPERATION operation, _USHORT16 index);



/**
  *@brief  Inquire international mobile subscriber identity
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireSubscriber(HWM pWM_Handle,     _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief List current calls of ME
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ListCurCalls(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief Facility lock
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strFac            [in]Facility
  *@param      strMode            [in]0: disable, 1: enable, 2: query status
  *@param      strPasswd        [in]password
  *@param      eClass            [in]class

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_FacilityLock(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle,    
            RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass);


_UINT32 WM_InqFacilityLock(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                      RIL_FACILITY fac, RIL_DATA_CLASS eClass);

/**
  *@brief Calling line identification presentation
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]1: display URC, 0: suppress

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CallingLineId(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief Check the state of Calling line identification presentation
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CheckCLIP(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief Check the state of Calling line identification restriction
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CheckCLIR(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_SetCLIRActive(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_SetCLIRDisable(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief Dailcall without ";"
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]1: display URC, 0: suppress

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_DailCLIR(HWM pWM_Handle, _UINT32 serviceHandle,
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief Check the state of COLR
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CheckCOLR(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_SetSIMSlot(HWM pWM_Handle, _UINT32 serviceHandle,
                       T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 simslot);

/**
  *@brief Set loudspeaker volume level
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    volume            [in]volume level

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetSpeakerVol(HWM pWM_Handle, _UINT32 serviceHandle,
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 volume);



/**
  *@brief Set loudspeaker volume level
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]0: disable result code, 1: enable, use numeric values,
                              2: enable, use verbose values

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ReportMobileError(HWM pWM_Handle, _UINT32 serviceHandle,
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);


/**
  *@brief Enter multiplex mode
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_EnterMultiplex(HWM pWM_Handle, _UINT32 serviceHandle,
                     T_EDEVICEETYPE serviceType, _UINT32 customHandle);


/**
  *@brief Read operator names
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ReadOperatorNames(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);


/**
  *@brief Inquire the current mode and the currently used operator(if registered)
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireCurOperator(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_InquireBSInfo(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief Inquiry all the available operators
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireAvaiOperator(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
*@brief Operator selection
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���
*@param _BOOL bAuto    [in]EOS_TRUE:�Զ�ѡ��ע�����磬�ֶ�ѡ��ע������
*@param strOper            [in]����ID�ţ�ֻ���յ�ѡ������ʱ��Ҫ�ṩ

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SelectOperator(HWM pWM_Handle, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                        RIL_NET_MODE mode, _CHAR8* strOper);

_UINT32 WM_SetEngineerMode(HWM pWM_Handle, _UINT32 serviceHandle,
                        T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                        _UINT32 mode, _UINT32 dump);

/**
  *@brief  Read current phone book entries
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   begin            [in]location number where reading starts
  *@param      end                [in]location number where reading ends. Set it as 0 if needn't

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ReadPbEntries(HWM pWM_Handle, _UINT32 serviceHandle,
                T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end);


/**
  *@brief   Inquire phone book entry space
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquirePbSpace(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_PB_STORAGE storage);





/**
  *@brief  Write a phone book entry to the specific phone book "index"
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   index            [in]If "index" equal or less than 0, then write this entry to the first free space
  *@param      strFormat        [in]telephone number. Set this parameter as NULL to delete a entry
  *@param      strText            [in]name

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_WritePbEntry(HWM pWM_Handle, _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
                    RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText);



/**
  *@brief  Inquire PIN status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquirePinStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);


_UINT32 WM_InquireInitStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);


/**
  *@brief  Enter PIN
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strPIN            [in]password
  *@param      strNewPIN        [in]if strPIN is PUK, a new PIN need to be input here

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_EnterPIN(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                _CHAR8* strPIN, _CHAR8* strNewPIN);



/**
  *@brief  Change password
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strFac            [in]Facility
  *@param      strOldPwd        [in]Password specified for the facility
  *@param      strNewPwd        [in]Password specified for the facility

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ChangePassword(HWM pWM_Handle, _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                    RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd);



/**
  *@brief Network registration
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strValue        [in]0: disable URCs, 1,2: enable URC

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_NetworkRegistration(HWM pWM_Handle, _UINT32 serviceHandle,
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);


_UINT32 WM_InquireNetworkRegStatus(HWM pWM_Handle, _UINT32 serviceHandle,
                             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);

/**
  *@brief Set TE character set
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    strChset        [in]character set

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetCharacterSet(HWM pWM_Handle, _UINT32 serviceHandle, 
                 T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strChset);


/**
  *@brief  Get the signal quality
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireSignalQuality(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_InquireSideTone(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_InquireCallStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                              T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief  Set supplementary service notifications
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strCSSI            [in]cssi value
  *@param      strCSSU            [in]cssu value
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetSSNote(HWM pWM_Handle, _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                    _CHAR8* strCSSI, _CHAR8* strCSSU);



/**
  *@brief  Send Unstructured supplementary service data
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strN            [in]0: Disable the result code, 
                                1: Enable the result code, 2: Cancel session
  *@param      strStr            [in]USSD-string
  *@param      strDcs            [in]Cell Broadcast Data Coding Scheme
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SendUSSD(HWM pWM_Handle, _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle,
                    _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs);



/**
  *@brief  Send USSD data
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strUssdString    [in]USSD string
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SendUssdData(HWM pWM_Handle, _UINT32 serviceHandle,
             T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strUssdString);

_UINT32 WM_SendATOrigin(HWM pWM_Handle, _UINT32 serviceHandle,
                      T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strATString);

/**
  *@brief  Cancel USSD data
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CancelUssdData(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief  Send DTMF string
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strDtmfStr        [in]DTMF string
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_DialDTMF(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strDtmfStr);

_UINT32 WM_SetDTMFPrd(HWM pWM_Handle, _UINT32 serviceHandle, 
                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strDtmfStr);

/**
  *@brief   when a ATCommand is executing, send this AT command to cancel
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@return    _UINT32            
*/
_UINT32 WM_SetCancel(HWM pWM_Handle, _USHORT16 chID);


/**
 *@brief   Cancel dail
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CancelDial(HWM pWM_Handle, _UINT32 serviceHandle, 
                   T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief  Set alarm time
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   alarmData        [in]alarm parameter
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetAlarm(HWM pWM_Handle, _UINT32 serviceHandle, 
             T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_CALA_ALARM *alarmData);



/**
  *@brief  Get field test report
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strMode            [in]mode
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_FieldTestReport(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strMode);



/**
  *@brief  Read SMS message and do not change the SMS stat
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   _USHORT16 index        [in]SMS index
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ReadSMSUnchgStat(HWM pWM_Handle, _UINT32 serviceHandle,
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _USHORT16 index);



/**
  *@brief   Switch off mobile station
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SwitchOffModule(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief  Set microphone gain level
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   channel            [in]main microphone or earphone microphone
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetMicGainLevel(HWM pWM_Handle, _UINT32 serviceHandle, 
                         T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 volume);


/**
  *@brief  Enable/disable earphone
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strMode            [in]1: insert, 0: out
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetAudioPath(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle,RIL_AUDIO_PATH_TYPE Mode);

_UINT32 WM_SetAudioMode(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,RIL_AUDIO_MOD_TYPE Mode);

_UINT32 WM_SetVoiceHangupCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bEnable);

/**
@brief: Mute Audio
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���
*@param bMute            [in]EOS_TRUE: mute on; EOS_FALSE: mute off
  
    *@return    _UINT32                
    *@retval    >0                    ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetAudioMute(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,_BOOL bMute);

/**
@brief: Play tone
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���
*@param type            [in]tone type
*@param volume            [in]volume
*@return    _UINT32                
*@retval    >0                    ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_PlayTone(HWM pWM_Handle, _UINT32 serviceHandle, 
                           T_EDEVICEETYPE serviceType, _UINT32 customHandle,_USHORT16 type, _USHORT16 volume);

/**
  *@brief   Inquire headset status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireHeadsetStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief   Inquire headset button status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireHeadsetBtnStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief   Get the battery level
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireBatteryLevel(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief   Inquire PIN remain count
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquirePinCount(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle,_UCHAR8 type);



/**
  *@brief   Inquire PUK remain count
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquirePukCount(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);












/**
  *@brief   Dump BroadCast
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_DumpBroadCast(HWM pWM_Handle, _UINT32 serviceHandle, 
                    T_EDEVICEETYPE serviceType, _UINT32 customHandle);


/**
  *@brief  Ringer Sound Level
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
 *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   level            [in]volume
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetRingerSoundLevel(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 level);



/**
  *@brief  Echo cancellation control
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strAudioParam    [in]audio parameter
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetAudioOutput(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strAudioParam);



/**
  *@brief  SMS Full URC Enable
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strValue        [in]value
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetSmsFullUrc(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief   query module state
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
 *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_QueryModuleState(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);


/**
  *@brief   AT+COLP=?
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_InquireCOLP(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief  Set Colp Status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strValue        [in]value
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetColpStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief  Check latest URC event 
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strIndex        [in]index
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_CheckLatestURCEvent(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strIndex);



/**
  *@brief  control whether URC indicate to AK23xx
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strValue        [in]value
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ControlURCIndication(HWM pWM_Handle, _UINT32 serviceHandle, 
        T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);



/**
  *@brief  control module audio driver including path,handfree and mute 
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   mode            [in]mode
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_ControlModuleDriver(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _UCHAR8 mode);



/**
  *@brief  Query the URC indicate to AK23xx
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_QueryURCIndication(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);



/**
  *@brief  Mobile equipment event reporting
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strValue        [in]value
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_MobileEquipmentEvent(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle,_CHAR8* strValue);


/**
  *@brief  only send "at"
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SendAT(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_PwrCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  *@brief set ms storage type
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strService        [in]service type
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetMSStorageType(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strService);




/**
  *@brief set ext sms storage status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strService        [in]service type
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetExtSMSStorageStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bFull);




/**
  *@brief  forbid modem to sleep
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetModuleSleep(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle,_UCHAR8 enable);



/**
  *@brief  get modem version information
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_GetModemVersionNum(HWM pWM_Handle, _UINT32 serviceHandle, 
                T_EDEVICEETYPE serviceType, _UINT32 customHandle);




/**
  *@brief set ext sms storage status
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strStep            [in]'0'~'7' (400mA~1100mA)
 
  
  *@return    _UINT32                
  *@retval                        ��AT������AT��������е���ˮ��                
*/
_UINT32 WM_SetChargingCurrent(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strStep);


/**
  *@brief Indicator control use for ulc2
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   strValue        
 
  
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��

*/
_UINT32  WM_ConfigURCMode(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_URC_MODE mode);

/**
  *@brief URC control for ulc2
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   xdrvType        
 
  
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��

*/
_UINT32  WM_XDRVURCControl(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_sXDRV_TYPE *xdrvType);

/**
  *@brief control ULC2 audio volume 
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   audCtrlType
  *@param   strValue    
 
  
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��

*/
_UINT32  WM_XDRVAudioControl(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, 
            RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue);

/**
  *@brief control infineon trace output 
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param    bOnOff            [in]EOS_TRUE-enable trace output,EOS_FALSE-disable trace output
 
  
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��

*/
_UINT32  WM_SetTraceOutput(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL bOnOff);

/**
  *@brief read exception log of modem  
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
 
  
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��

*/
_UINT32  WM_GetModemExcLog(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle);


/**
  *@brief read exception log of modem  
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���
  *@param   enable            [in]    
  "EOS_TRUE"-new message will keep unread after reading
  "EOS_FALSE"- new message state is read after reading
  
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��

*/
_UINT32  WM_XDRVControlNewSMSStatus(HWM pWM_Handle, _UINT32 serviceHandle, 
            T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL enable);


/**
*@brief:enable or disable audio loop test
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���
*@param   enable            [in]    
  "EOS_TRUE"-enable audio loop test
  "EOS_FALSE"-disable audio loop test
 * @return _VOID 
  *@return    _UINT32                
  *@retval    >0                    ��AT������AT��������е���ˮ��
  
*/
_UINT32  WM_XDRVControlAudioLoopTest(HWM pWM_Handle, _UINT32 serviceHandle, 
                                  T_EDEVICEETYPE serviceType, _UINT32 customHandle, _BOOL enable);

/**
* *@brief  If the new incoming message need acknowledge
*@return    _BOOL         true: need acknowledge
*@author  MIAOBAOLI
*@date    2007.11.26
*/        
_BOOL WM_NewMsgNeedAck(HWM pWM_Handle);

/**
* *@brief  If the ATD command return directly before call establishing.
*@return    _BOOL         true: need acknowledge
*@author  MIAOBAOLI
*@date    2007.11.26
*/        
_BOOL WM_ATDReturnDirectly(HWM pWM_Handle);

/**
  * *@brief  Init wireless module before PIN
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return >0                    ��AT������AT��������е���ˮ��
  *@author  MiaoBaoli
  *@date    2007.11.26
*/
_UINT32 WM_InitBeforePIN(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
  * *@brief  Init wireless module after PIN
  *@param    pWM_Handle        [in]ָ������ģ������ָ��
  *@param    serviceHandle    [in]���ܹ���ģ����
  *@param    serviceType        [in]���ܹ���ģ������
  *@param    customHandle    [in]�ͻ���Ϣ���

  *@return >0                    ��AT������AT��������е���ˮ��
  *@author  MiaoBaoli
  *@date    2007.11.26
*/
_UINT32 WM_InitAfterPIN(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
* *@brief  download STK profile to active STK
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���

  *@return >0                    ��AT������AT��������е���ˮ��
  *@author  MiaoBaoli
  *@date    2007.11.26
*/        
_UINT32 WM_STKSetProfile(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_STKSetAutoRsp(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle);

_UINT32 WM_STKSetTR(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle);

/**
* *@brief  Select STK main menu item
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���

  *@return >0                    ��AT������AT��������е���ˮ��
  *@author  MiaoBaoli
  *@date    2007.11.26
*/        
_UINT32 WM_STKSelectMainMenu(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle, _SHORT16 index);

/**
* *@brief  Send response for the STK proactive command
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���

  *@return >0                    ��AT������AT��������е���ˮ��
  *@author  MiaoBaoli
  *@date    2007.11.26
*/        
_UINT32 WM_STKResponse(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_RES_PAR *par);

/**
* *@brief  If the STK detail info within URC.
*@return    _BOOL
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL WM_STKDetailInfoInURC(HWM pWM_Handle);

/**
* *@brief  Send response for the STK proactive command
*@param    pWM_Handle        [in]ָ������ģ������ָ��
*@param    serviceHandle    [in]���ܹ���ģ����
*@param    serviceType        [in]���ܹ���ģ������
*@param    customHandle    [in]�ͻ���Ϣ���

  *@return >0                    ��AT������AT��������е���ˮ��
  *@author  MiaoBaoli
  *@date    2007.11.26
*/
_UINT32 WM_STKGetDetailInfo(HWM pWM_Handle, _UINT32 serviceHandle, T_EDEVICEETYPE serviceType, _UINT32 customHandle, RIL_STK_CMD cmd);

_UINT32 WM_InquireGprsState(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);
_UINT32 WM_EstablishDataConnWithGPRS(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* strAPN, _BOOL bMUX, _USHORT16 step);
_UINT32 WM_EstablishDataConnWithCSD(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _USHORT16 step);
_UINT32 WM_ReleaseDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step);
_UINT32 WM_SuspendDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step);
_UINT32 WM_ResumeDataConn(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle, _BOOL bMUX, _USHORT16 step);

/**
*@brief ��ģ����
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 WM_IgniteModule(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);

/**
*@brief ����ģ��
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 WM_ResetModule(HWM pWM_Handle, _UINT32 ManagerHandle, T_EDEVICEETYPE ManagerType, _UINT32 CustomHandle);

/**
*@brief:��ѯģ���Ƿ�����˯��
*@param [in] HWM hWM �Ѿ������õ�����ģ���豸���
*@retval EOS_TRUE: ����˯��, EOS_FALSE: ������˯�� 
*/
_BOOL WM_IsSleepAllowed(HWM hWM);

/**
*@brief ��ģ�����˯��ģʽ
*@param [in] HWM hWM �Ѿ������õ�����ģ���豸���
*@return _VOID 
*/
_VOID WM_SleepModule(HWM hWM);

/**
*@brief ����ģ��
*@param [in] HWM hWM �Ѿ������õ�����ģ���豸���
*@return _VOID 
*/
_VOID WM_WakeupModule(HWM hWM);

/**
*@brief �õ�ģ���˯��״̬
*@param [in] HWM hWM �Ѿ������õ�����ģ���豸���
*@return _BOOL
*@retval EOS_TRUE: sleeping; EOS_FALSE: wakeup
*/
_BOOL WM_GetSleepState(HWM hWM);

_BOOL FindInURCResult(const _CHAR8* result, _USHORT16 size, T_pURCRESULT pStru);

_UINT32 WM_BatteryCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);
                          
_UINT32 WM_ToneDetCtrl(HWM pWM_Handle, _UINT32 serviceHandle, 
                          T_EDEVICEETYPE serviceType, _UINT32 customHandle, _CHAR8* strValue);
#endif


