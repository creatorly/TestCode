
#ifndef PROXY_H
#define PROXY_H

#include "wm_struct.h"
#include "wm_interface.h"

#define ATLAYER_VERSION "ATLIB: V1.0.0"
typedef struct tagRIL* HRIL;

/**
  *@brief ��������URC��Ϣ�Ļص���������
  *@param HRIL hRIL ����URC��Ϣ���ĸ�RIL�Ϲ���
  *@param _UINT32 eventID URC��ϢID���ñ�����������RIL_URC_ID
  *@param _UINT32 param ��URC��Ϣͬ�������Ĳ���
  *@param _VOID* lpData ��URC��Ϣͬ�������Ľṹ��ָ�룬���û�У���ֵΪNULL
  *@param _UINT32 size ��URC��Ϣͬ�������Ľṹ��ĸ��������û�У���ֵΪ0
  *@param _UINT32 customParam �ڵ���RIL_Open����ʱ�����customParam
  *@return    _UINT32 0: ִ�гɹ���
*/

/*
typedef _UINT32 (*URCCALLBACK)(HRIL hRIL, _UINT32 eventID, _UINT32 param, _VOID* lpData, _UINT32 size, _UINT32 customParam);
*/

/**
  *@brief ��������ĳ���������첽���ؽ���Ļص���������
  *@param HRIL hRIL �������ش��ĸ�RIL�Ϲ���
  *@param _UINT32 resultType �ò�����ִ�н�����ñ�����������RIL_PARSE_RES
  *@param _VOID* lpData ���������ͬ�������Ľṹ��ָ�룬���û�У���ֵΪNULL
  *@param _UINT32 size ���������ͬ�������Ľṹ��ĸ��������û�У���ֵΪ0. ���resultType��ֵΪRIL_prCMEERROR����RIL_prCMSERROR����ֵ��ʾERROR ID
  *@return    _UINT32 0: ִ�гɹ���
*/
typedef _UINT32 (*RESCALLBACK)(_UINT32 ChannelID, HRIL hRIL, _UINT32 resultType, _VOID* pData, _UINT32 size);

/**
  *@brief ����RIL
  *@param _UINT32 ChnlNum    [in] �趨����ģ���ͨ��������cmuxģʽ��ͨ����ֹһ��ͨ������cmuxģʽֻ��һ��ͨ��
  *@param _USHORT16 *ChID [in] ͨ��ID���飬�������ͨ����ID
  *@param _UINT32 Type [in] ����ģ�������. 1: WAVECOM
  *@param _UINT32 *phATServer    [out] Radio server�ľ��.�þ����ʶһ��ģ�飬��ģ�ֻ���Ҫ���øýӿڶ�Ρ�
  *@return    _UINT32 0: �����ɹ���
*/
_UINT32 RIL_Setup(_UINT32 ChnlNum, _UINT32* ChID, _UINT32 Type, _UINT32* phATServer);

/**
  *@brief ��������������Radio server
  *@param
  *@return    _UINT32 0: �����ɹ���
*/
_UINT32 RIL_Stop();

/**
  *@brief ����Radio server��ͨ����Ϣ
  *@param HRIL hRIL [in] ����һ���ڽ�Ҫ���µ�Radio server�ϴ򿪵�RIL���
  *@param _UINT32 ChnlNum    [in] �趨����ģ���ͨ��������cmuxģʽ��ͨ����ֹһ��ͨ������cmuxģʽֻ��һ��ͨ��
  *@param _USHORT16 *ChID [in] ͨ��ID���飬�������ͨ����ID
  *@return    _UINT32 0: ���³ɹ���
*/
_UINT32 RIL_UpdateChannel(_UINT32 ChannelID, HRIL hRIL, _UINT32 ChnlNum, _USHORT16* ChID);

/**
  *@brief ��ĳ��radio server�ϴ�һ��RIL���
  *@param _UINT32 eventType [in] ָ����RILϣ�����յ���Ϣ���ͣ�������RIL_URC_TYPE��������ϡ�
  *@param _UINT32 hATServer [in] ָ�����ĸ�radio server�ľ��
  *@param URCCALLBACK fCallback [in] ָ������radio server��Ϣ�Ļص�����
  *@param _UINT32 customPara [in] ָ��������Ϣ������Ϣ���ڻص������з���
  *@param HRIL *phRIL [out] ���ص�RIL��� 
  *@return    _UINT32 0: �򿪳ɹ�
*/
_UINT32 RIL_Open(_UINT32* ChID, _UINT32 eventType, _UINT32 hATServer, URCCALLBACK fCallback, _UINT32 customParam, HRIL* phRIL);

/**
  *@brief �ر�һ��RIL���
  *@param HRIL hRIL [in] ָ��Ҫ�رյ�RIL ���
  *@return    _UINT32 0: �رճɹ�
*/
_UINT32 RIL_Close(HRIL hRIL);

/**
  *@brief ��ָ����RIL�ϲ���
  *@param HRIL hRIL [in] ָ��Ҫ���ŵ�RIL ���
  *@param const _UCHAR8* lpszAddress [in] Ҫ����ĵ绰����
  *@param _UINT32 dwType [in] reserved to be use
  *@param _UINT32 dwOptions [in] RIL_DATA_CLASS����
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _UINT32 *callHandle [out] �õ绰��Ӧ��HCALL��� 
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_MakeCall(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* lpszAddress, 
                      _UINT32 dwType, _UINT32 dwOptions, 
                      RESCALLBACK fCallback, _UINT32* callHandle);

/**
  *@brief �ܽ�����
  *@param HRIL hRIL [in] ָ��Ҫ�ܽ������RIL ���
  *@param _UINT32 hCall [in] ָ��Ҫ�ܽӵ�CALL ���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_RejectCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief ��������
  *@param HRIL hRIL [in] ָ��Ҫ���������RIL ���
  *@param _UINT32 hCall [in] ָ��Ҫ������CALL ���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_AnswerCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief �Ҷ����е绰
  *@param HRIL hRIL [in] ָ��Ҫ���������RIL ���
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_HangupCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief ȡ������
  *@param HRIL hRIL [in] ָ��Ҫȡ�����ŵ�RIL ���
  *@param _UINT32 hCall [in] ָ��Ҫȡ�����ŵ�Call���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_CancelDial(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief �л�����hold/active�绰��״̬
  *@param HRIL hRIL [in] ָ��Ҫ�л��绰��RIL ���
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SwapCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief �绰����
  *@param HRIL hRIL [in] ָ��Ҫִ�е绰�����RIL ���
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ConferenceCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief �绰����״̬�½�����·
  *@param HRIL hRIL [in] ָ��Ҫִ�н�����·��RIL ���
  *@param _UINT32 hCall [in] ָ��Ҫ�����绰��call���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EndSingleCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief ��������active״̬�ĵ绰
  *@param HRIL hRIL [in] ָ��Ҫִ�н���active������RIL ���
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EndActiveCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief ��������hold״̬�ĵ绰
  *@param HRIL hRIL [in] ָ��Ҫִ�н���hold������RIL ���
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EndHoldCall(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief ��active conference call�з���һ·������ͨ����ͬʱhold�����绰
  *@param HRIL hRIL [in] ָ��Ҫִ�н���hold������RIL ���
  *@param _UINT32 hCall [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SplitFromConference(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

_UINT32 RIL_UpdateCurCallList(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RESCALLBACK fCallback);

/**
  *@brief ���ֻ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL ���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strDtmfStr [in] DTMF�ַ�
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_DialDTMF(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strDtmfStr);

_UINT32 RIL_SetDTMFPrd(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strDtmfStr);

/**
  *@brief ��ѯָ�����͵ĵ绰�ĸ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RIL_CALL_STATUS status [in] �绰���ͣ�����ò���ΪRIL_CALL_STATUS_NUM����ʾҪ��ѯ���е绰��Ŀ
  *@param _UINT32 *callQty [out] �����ѯ���ĵ绰��Ŀ
  *@return    _UINT32 0: ִ�гɹ�
*/
_UINT32 RIL_GetCallQty(_UINT32 ChannelID, HRIL hRIL, RIL_CALL_STATUS status, _UINT32* callQty);

/**
  *@brief ��ѯ�绰����ϸ��Ϣ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _UINT32 hCall [in] ָ��Ҫ��ѯ��CALL���������ò���Ϊ0����ʾҪ��ѯ���е绰����ϸ��Ϣ
  *@param RIL_CLCC_ENTRY *pCLCC [out] �����ѯ���ĵ绰��Ϣ
  *@param _UINT32 *number [out] �����ѯ���ĵ绰��Ŀ
  *@return    _UINT32 0: ִ�гɹ�
*/
_UINT32 RIL_GetCallInfo(_UINT32 ChannelID, HRIL hRIL, _UINT32 hCall, RIL_CLCC_ENTRY *pCLCC, _UINT32 *number);

/**
  *@brief �趨���еȴ�
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _BOOL bURCEnable [in] �趨���еȴ�URC�Ƿ��ϱ�
  *@param _BOOL bEnable [in] �趨���еȴ�����
  *@param RIL_DATA_CLASS eClass [in] ָ��Ҫ�趨��ͨ������
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetCallWait(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass);

/**
  *@brief ��ѯ���еȴ�״̬���ú������첽���ؽ��������RIL_CCWA_LIST�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_DATA_CLASS eClass [in] ָ��Ҫ��ѯ��ͨ������
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireCallWait(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_DATA_CLASS eClass);

/**
  *@brief �趨����ת��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_CCFC_FWDCALLS *cfData [in] ���еȴ��ṹ��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetCallForward(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CCFC_FWDCALLS *cfData);

/**
  *@brief ��ѯ����ת��״̬���ú������첽���ؽ��������RIL_CCFC_FWDCALLS�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_CCFC_REAS reason [in] ָ��Ҫ��ѯ�ĺ���ת������
  *@param RIL_DATA_CLASS Class [in] ָ��Ҫ��ѯ��ͨ������
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireCallFwd(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class);

/**
  *@brief ���Ͷ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param const _UCHAR8* pPDU [in] ����õ�PUD���ݰ�
  *@param _UINT32 option [in] Reserved to be use
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SendMsg(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, RESCALLBACK fCallback);

_UINT32 RIL_QiSendData(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pDataBuf, _UINT32 DataLen, RESCALLBACK fCallback);

_UINT32 RIL_SendMsgByTxt(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pTxt, _UINT32 uStringLen, const _UCHAR8* pNumber, RESCALLBACK fCallback);

_UINT32 RIL_SendMsg_CDMA(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, RESCALLBACK fCallback);

/**
  *@brief ��SIM�����ţ��������첽���ؽ����RIL_SMS_ENTRY�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _USHORT16 index [in] ������SIM���ϵ�������
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ReadSMS(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

_UINT32 RIL_ReadSMSByTxt(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief ��SIM�����ţ��������첽���ؽ����RIL_SMS_ENTRY�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _USHORT16 index [in] ������SIM���ϵ�������
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ReadSMS_CDMA(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief ������ŵ�SIM��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param const _UCHAR8* pPDU [in] ����õ�PUD���ݰ�
  *@param _UINT32 option [in] Reserved to be use
  *@param _BOOL read_flag [in] EOS_TRUE: �����Ѷ���EOS_FALSE:����δ��
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SaveMsgToSIM(_UINT32 ChannelID, HRIL hRIL, const _UCHAR8* pPDU, _UINT32 option, _BOOL read_flag, _SHORT16 PDUlen, RESCALLBACK fCallback);

/**
  *@brief ��SIM������,���ı������SIM���ϵ�״̬���������첽���ؽ����RIL_SMS_ENTRY�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _USHORT16 index [in] ������SIM���ϵ�������
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ReadSMSUnchgStat(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief ��ѯָ�����ſռ���������������첽���ؽ����RIL_CPMS�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_MSG_STORAGE storage [in] ָ�����ſռ�
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireSmsSpace(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback,RIL_MSG_STORAGE storage);

/**
  *@brief �г�SIM��������ָ��״̬�Ķ��ţ����ı����״̬���������첽���ؽ����RIL_SMS_ENTRY�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_SMS_MODE stat [in] ָ������״̬
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ListSMS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_SMS_MODE stat);

/**
  *@brief ��ѯ��Ϣ���ĺ��롣�������첽���ؽ����RIL_SMS_CENADDRESS�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireSmsCenter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ������Ϣ���ĺ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strSca [in] �������ĺ���
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetSmsCenter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strSca);

/**
  *@brief �����¶��ŵĴ洢ģʽ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_STORENEWMSG_MODE mode [in] ָ�����Ŵ洢ģʽ
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_NewMsgStoreMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_STORENEWMSG_MODE mode);

/**
  *@brief ���ö��Ŵ洢״̬
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _USHORT16 index [in] ������SIM���ϵ�������
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _BOOL bFull [in] EOS_FALSE :����δ������EOS_TRUE:�����Ѿ�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetSMSStorageStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bFull);

/**
  *@brief ����С���㲥
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _USHORT16 index [in] ������SIM���ϵ�������
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SelectBroadCast(_UINT32 ChannelID, HRIL hRIL, _CHAR8* strMode, _CHAR8* strMids, _CHAR8* strDcss, RESCALLBACK fCallback);

/**
  *@brief ɾ��SIM���ϵĶ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param _USHORT16 index [in] ������SIM���ϵ�������
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_DeleteSMS(_UINT32 ChannelID, HRIL hRIL, _USHORT16 index, RESCALLBACK fCallback);

/**
  *@brief ����SIM��STK����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_STKSetProfile(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_STKSetAutoRsp(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_STKSetTR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��STK setup menu�Ĳ˵�����ѡ��һ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _SHORT16 index [in] ѡ�в˵����ID��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_STKSelectMainMenu(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 index);

/**
  *@brief ��ӦSTK proactive command
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_RES_PAR *par [in] ��Ӧ����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_STKResponse(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_RES_PAR *par);

/**
  *@brief ����AT�������ͨ���Ƿ�ͨ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_Attention(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��SIM��ָ���绰������Ϣ���ú������첽���ؽ������RIL_PB_SIM_ENTRY�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_PB_STORAGE storage [in] ָ���绰��
  *@param _SHORT16 begin [in] ��ʼ������
  *@param _SHORT16 end [in] ���������ţ������ֵΪ0��ֻ��begin����ָ����һ����¼
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ReadPbEntries(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage, _SHORT16 begin, _SHORT16 end);

/**
  *@brief ��ѯSIM��ָ���绰�����������ú������첽���ؽ������RIL_SIMPB_SPACE�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_PB_STORAGE storage [in] ָ���绰��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquirePbSpace(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage);

/**
  *@brief д�绰����¼��SIM��ָ���绰����ָ��λ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_PB_STORAGE storage [in] ָ���绰��
  *@param _SHORT16 index [in] ָ���绰����λ�ã������ֵΪ0��д�뵽�绰����һ���յ�λ��
  *@param _CHAR8* strNumber [in] �绰����
  *@param _USHORT16 *strText [in] ��������Ҫ����unicode�����ʽ
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_WritePbEntry(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage, _SHORT16 index, _CHAR8* strNumber, _USHORT16 *strText);

/**
  *@brief ɾ��SIM��ָ���绰��ָ��λ���ϵļ�¼
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_PB_STORAGE storage [in] ָ���绰��
  *@param _SHORT16 index [in] ָ���绰����λ��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_DelPbEntry(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_PB_STORAGE storage, _SHORT16 index);

/**
  *@brief ��ɲ��ֶ�SIM��������ģ��ĳ�ʼ������Щ��ʼ������������PIN���״̬
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InitBeforePIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��ɲ��ֶ�SIM��������ģ��ĳ�ʼ������Щ��ʼ������������PIN����Ч��״̬�²������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InitAfterPIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��ѯPIN���״̬
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquirePinStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_InquireInitStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ���ݵ�ǰSIM��PIN���״̬����������PIN�뻹��PUK��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param [in] strPIN PIN code
  *@param [in] strNewPIN ���Ҫ����PUK�룬Ҫָ���µ�PIN��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EnterPIN(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strPIN, _CHAR8* strNewPIN);

/**
  *@brief ��������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_FACILITY fac[in] facility lock function that need to set new password
  *@param _CHAR8* strOldPwd[in] old password
  *@param _CHAR8* strNewPwd[in] new password
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ChangePassword(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd);
/**
  *@brief ��ѯPIN��ʣ�ೢ�Դ���������Ϊ��ʱSIM���Զ�����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquirePinCount(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _UCHAR8 type);

/**
  *@brief �����豸�������������Ƶ�
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param [in] fac facility
  *@param [in] bLock: EOS_TRUE lock; EOS_FALSE unlock
  *@param [in] strPasswd: password
  *@param [in] eClass: data class
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_FacilityLock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, _BOOL bLock, _CHAR8* strPasswd, RIL_DATA_CLASS eClass);

/**
  *@brief ��ѯ�豸����״̬���ýӿڵ��첽���ؽ������RIL_CLCK_LOCK�ṹ�����ͷ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param [in] fac facility
  *@param [in] eClass: data class
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireFacilityLock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_FACILITY fac, RIL_DATA_CLASS eClass);

/**
  *@brief ��ѯ��ǰע������繩Ӧ�̣��ú������첽���ؽ������RIL_COPS_CURRENTNAME�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireCurOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_InquireBSInfo(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ѡ��ע������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param [in] mode �Զ�ע������,�ֶ�ע�����绹��ȡ��ע��
  *@param [in] strOper: ����ID�ţ�ֻ����ѡ���ֶ�ע��ʱ��Ҫ�ṩ
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SelectOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_NET_MODE mode, _CHAR8* strOper);

/**
  *@brief ��ѯ��ǰ�Ŀ������磬�ú������첽���ؽ������RIL_COPS_NETWORK�ṹ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireAvaiOperator(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��ѯ��ǰGPRS����ĸ���״�����ú������첽���ؽ������T_BOOL�������أ�EOS_TRUE��ʾ�Ѿ�������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireGprsState(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ͨ��GPRS������������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strAPN [in] �����
  *@param _BOOL bMUX [in] ָ���Ƿ�MUXģʽ
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EstablishDataConnWithGPRS(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strAPN, _BOOL bMUX);

/**
  *@brief ͨ��CSD������������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* dial [in] CSD���ź���
  *@param _CHAR8* user [in] �û���
  *@param _CHAR8* pwd [in] ����
  *@param _BOOL bMUX [in] ָ���Ƿ�MUXģʽ
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EstablishDataConnWithCSD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX);

/**
  *@brief �ͷ���������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _BOOL bMUX [in] ָ���Ƿ�MUXģʽ
  *@param _USHORT16 step [in] ָ����ǰ�������Ӵ��ڵĽ׶�
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ReleaseDataConn(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bMUX, _USHORT16 step);

/**
  *@brief ��ģ����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_IgniteModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��λģ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ResetModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief �ر�ģ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SwitchOffModule(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ģ���쳣�ص����������⵽ģ���쳣���磬Ҫ���øýӿ�֪ͨRIL���ƺ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RIL_EXCEPTION_TYPE type [in] �쳣����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
  *@author  mioabaoli
  *@date    2007.11.26
*/
_UINT32 RIL_WMExceptionCallback(_UINT32 ChannelID, HRIL hRIL, RIL_EXCEPTION_TYPE type);

/**
  *@brief ����AT����Ļ���ģʽ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _BOOL bOnOff [in] EOS_TRUE:����ģʽ��. EOS_FALSE:����ģʽ�ر�
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetCommandEcho(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bOnOff);


_UINT32 RIL_SendAT(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

_UINT32 RIL_ModulePwrCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

_UINT32 RIL_SendATOrigin(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strATString);

/**
  *@brief ��ѯ�ֻ�������汾��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strValue [in] NULL
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_GetProductID(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

/**
  *@brief �ָ�ģ��ĳ�������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param SysTime [in] ʱ��ṹ��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetManufDefault(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��ѯ�ֻ���IMEI�ţ��ú������첽���ؽ����һ��T_U8�͵��ַ���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireIMEI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_SetIMEI(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strIMEI);

_UINT32 RIL_SetBaudrate(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strRate);

/**
  *@brief Inquire net band
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief set net band
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strMode [in] 0:stand for 900 (EGSM), 1:stand for 1800 (DCS), 2:stand for 900/1800,auto(DUAL)
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetNetBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMode);

/**
  *@brief ��ѯ�ۼ�ͨ������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_GetCurrentAcm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ͨ���Ʒ�����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strPasswd [in] ��Ҫ����PIN2��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ResetAcm(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strPasswd);

/**
  *@brief ��ѯ�ۼ�ͨ���Ʒѵ����ֵ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_GetAcmMax(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);
/**
  *@brief �����ۼ�ͨ���Ʒѵ����ֵ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strAcmax [in] ACM���ֵ
  *@param _CHAR8* strPasswd [in] ��Ҫ����PIN2��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetAcmMax(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strAcmax, _CHAR8* strPasswd);

/**
  *@brief ��ѯ���ͨ����ͨ������
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_GetCurCallMeter(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ����ģ���ϵͳʱ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param SysTime [in] ʱ��ṹ��
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetClock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_SYSTIME *SysTime);

/**
  *@brief ��ѯ�ֻ���ʱ�䣬�ú������첽���ؽ����һ��RIL_SYSTIME�͵Ľṹ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireClock(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��ѯSIM����IMSI(�����ƶ��û�ʶ��)�� 
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireSubscriber(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ����ͨ����������Ͳ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param volume [in] ָ��Ҫ���õ���������
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetSpeakerVol(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 volume);


_UINT32 RIL_SetFreqBand(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 bandindex);

_UINT32 RIL_SetQSFR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 sfrIndex);

/**
  *@brief ����CMUXģʽ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_EnterMultiplex(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ��ѯ�ֻ���ǰ���ź�ǿ�ȣ��ú������첽���ؽ������_INT32���ͷ��أ���ֵ��ʾ�źż���ȡֵ��Χ0��4
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
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
  *@brief ����ģ�����Ƶͨ��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_AUDIO_PATH_TYPE Mode [in] ͨ������
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetAudioPath(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_AUDIO_PATH_TYPE Mode);

_UINT32 RIL_SetAudioMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_AUDIO_MOD_TYPE Mode);

_UINT32 RIL_SetVoiceHangUpCtrl(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bEnable);

_UINT32 RIL_SetMicGainLevel(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _SHORT16 volume);

/**
  *@brief ����ģ��ľ���ģʽ
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _BOOL bMute [in] EOS_TRUE: mute on; EOS_FALSE: mute off
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SetAudioMute(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _BOOL bMute);

/**
  *@brief ����ģ�鷢��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _USHORT16 type [in] ��������
  *@param _USHORT16 volume [in] ����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_PlayTone(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _USHORT16 type, _USHORT16 volume);

/**
  *@brief ָ��ģʽ�£���ЩURCҪ�ϱ�
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param RIL_URC_MODE mode [in]
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_ConfigURCMode(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, RIL_URC_MODE mode);

/**
*@brief �õ�ģ���˯��״̬
*@param [in] HRIL hRIL
*@return _BOOL
*@retval EOS_TRUE: sleeping; EOS_FALSE: wakeup
*/
_BOOL RIL_GetSleepState(_UINT32 ChannelID, HRIL hRIL);

/**
  *@brief ��ѯС����Ϣ���ú������첽���ؽ������RIL_CELL_TEST�ṹ�巵��
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strMode [in] reserved to be use
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_FieldTestReport(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strMode);

/**
  *@brief ��ѯ��������״̬����ѯ�������URC��Ϣ�ķ�ʽ֪ͨӦ�ó���
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireHeadsetStatus(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief ����USSD����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strN [in] 0: Disable the result code, 1: Enable the result code, 2: Cancel session
  *@param _CHAR8* strStr [in] USSD-string
  *@param _CHAR8* strDcs [in] Cell Broadcast Data Coding Scheme
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SendUSSD(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs);

/**
  *@brief ���ڼ���USSD����ʱ�������û������USSD�������� 
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@param _CHAR8* strUssdString [in] USSD-string 
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_SendUssdData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strUssdString);

/**
  *@brief ����USSD�Ự����
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_CancelUssdData(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Check the state of Calling line identification presentation
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_CheckCLIP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Check the state of Calling line identification restriction
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_CheckCLIR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);


_UINT32 RIL_SetCLIRActive(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

_UINT32 RIL_SetCLIRDisable(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Dailcall without ";"
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_DailCLIR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback, _CHAR8* strValue);

/**
  *@brief Check the state of COLR
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_CheckCOLR(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief check Colp State
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
*/
_UINT32 RIL_InquireCOLP(_UINT32 ChannelID, HRIL hRIL, RESCALLBACK fCallback);

/**
  *@brief Set Colp Status
  *@param HRIL hRIL [in] ָ��Ҫִ�иò�����RIL���
  *@param RESCALLBACK fCallback [in] ������ִ�н����Ļص�����
  *@return    _UINT32  <0x1000000: �������ID�ţ��û�����ͨ����ID��ȡ���Ѿ�����������; >=0x1000000: ����ID
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

