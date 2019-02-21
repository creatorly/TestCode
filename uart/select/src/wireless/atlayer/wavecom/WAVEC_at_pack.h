
#ifndef WAVEC_AT_PACK_H
#define WAVEC_AT_PACK_H

#include "eos.h"
#include "WAVEC_at_parse.h"
#include "wm_interface.h"

_UCHAR8  WaveC_CheckUrcData(_UINT32 ChnId, RIL_URC_TYPE *UrcEventType, RIL_URC_ID *UrcEventID, _VOID **UrcParse, 
                            _UINT32 *strNum, _UINT32 *strSize, const _CHAR8* Data, _USHORT16 length);

/*GSM0705 AT command*/
_VOID WaveC_WmAt0705_DeleteSMS(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strIndex);
_VOID WaveC_WmAt0705_SetSmsFormat(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strMode);
_VOID WaveC_WmAt0705_SetTxtSmsShow(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strMode);
_VOID WaveC_WmAt0705_ListSMS(_CHAR8* ATCmd, _UINT32 *size, RIL_SMS_MODE stat);
_VOID WaveC_WmAt0705_ReadSMS(_CHAR8* ATCmd, _UINT32 *size, _USHORT16 index);
_VOID MC323_WmAt0705_ReadSMS(_CHAR8* ATCmd, _UINT32* size, _USHORT16 index);
_VOID WaveC_WmAt0705_SendSmsByPduMode(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strLength);
_VOID WaveC_WmAt0705_SetOpenTCPUDP(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr);
_VOID WaveC_WmAt0705_InquireQiStat(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0705_InquireQiSack(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0705_InquireQiLocIP(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0705_InquireQiRcvData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SetQiClose(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0705_SendQiData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strLength);
_VOID WaveC_WmAt0707_InquireQIRegApp(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_SetQIRegApp(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr);
_VOID SIM800_WmAt0707_SetAPN(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr);
_VOID WaveC_WmAt0707_SetQICsgp(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strStr);
_VOID WaveC_WmAt0707_SetQIAct(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_SetQIDeact(_CHAR8* ATCmd, _UINT32* size);
_VOID MC323_WmAt0705_SendSmsByPduMode(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strLength);
_VOID WaveC_WmAt0705_SendSmsByPduMode_CDMA(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU);
_VOID WaveC_WmAt0705_SendSmsByTxtMode(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU);

_VOID WaveC_WmAt0705_SaveSmsByPduMode(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strLength, _UCHAR8 read_flag);
_VOID WaveC_WmAt0705_SendSmsPduData(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strPDU);
_VOID WaveC_WmAt0705_SendSmsTxtData(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strPDU, _UINT32 uiLen);
_VOID WaveC_WmAt0705_NewSMSIndication(_CHAR8* ATCmd, _UINT32 *size, RIL_STORENEWMSG_MODE mode);
_VOID WaveC_WmAt0705_NewSMSAck(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0705_InquireSmsStorage(_CHAR8* ATCmd, _UINT32 *size, RIL_MSG_STORAGE storage);
_VOID WaveC_WmAt0705_InquireSmsCenter(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0705_SetSmsCenter(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strSca);
_VOID WaveC_WmAt0705_InquireBroadcast(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0705_SelectBroadcast(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strMode, const _CHAR8* strMids, const _CHAR8* strDcss);
_VOID WaveC_WmAt0705_SelectMsgService(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strService);
/*v25ter AT command */
_VOID WaveC_WmAtV25_SwitchDataToAtMode(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_Attention(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_AnswerCall(_CHAR8* ATCmd, _UINT32 *size);
_VOID CM300_WmAtV25_AnswerCall(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_DialCall(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strNumber, RIL_DATA_CLASS eClass);
_VOID C202_WmAtV25_DialCall(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strNumber, RIL_DATA_CLASS eClass);
_VOID CM300_WmAtV25_DialCall(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strNumber, RIL_DATA_CLASS eClass);
_VOID WaveC_WmAtV25_SetCommandEcho(_CHAR8* ATCmd, _UINT32 *size, _BOOL bOnOff);
_VOID WaveC_WmAtV25_DisconnectCall(_CHAR8* ATCmd, _UINT32 *size);
_VOID C202_WmAtV25_DisconnectCall(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_GetProductID(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strValue);
_VOID WaveC_WmAtV25_SwitchAtToDataMode(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_SetResultPresentation(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strValue);
_VOID WaveC_WmAtV25_SetResultFormat(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strValue);
_VOID WaveC_WmAtV25_SetManufDefault(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_InquireIMEI(_CHAR8* ATCmd, _UINT32 *size);
_VOID M10_WmAtV25_InquireIMEI(_CHAR8* ATCmd, _UINT32 *size);
_VOID C202_WmAtV25_InquireESN(_CHAR8* ATCmd, _UINT32 *size);
_VOID CM300_WmAtV25_InquireESN(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_SetIMEI(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strIMEI);
_VOID WaveC_WmAtV25_LockBS(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strBS);
_VOID WaveC_WmAtV25_SetBaudrate(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strRate);
_VOID WaveC_WmAtV25_InquireBand(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtV25_SetNetBand(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strMode);
/*GPRS AT command */
_VOID WaveC_WmAtGprs_InquireGprsState(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtGprs_GprsAttachDetach(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strState);
_VOID WaveC_WmAtGprs_DefinePDPContext(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strCid, const _CHAR8* strPdpType, const _CHAR8* strAPN, const _CHAR8* strPdpAddr);
_VOID WaveC_WmAtGprs_QualityOfService(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strParam);
_VOID WaveC_WmAtGprs_RequestGprsService(_CHAR8* ATCmd, _UINT32 *size);
/*GSM0707 AT command*/
_VOID WaveC_WmAt0707_GetCurrentAcm(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_ResetAcm(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strPasswd);
_VOID WaveC_WmAt0707_GetAcmMax(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SetAcmMax(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strAcmax, const _CHAR8* strPasswd);
_VOID WaveC_WmAt0707_GetCurCallMeter(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_CallForward(_CHAR8* ATCmd, _UINT32 *size, const RIL_CCFC_FWDCALLS *cfData);
_VOID WaveC_WmAt0707_InquireCallFwd(_CHAR8* ATCmd, _UINT32 *size, RIL_CCFC_REAS reason, RIL_DATA_CLASS Class);
_VOID WaveC_WmAt0707_InquireClock(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SetClock(_CHAR8* ATCmd, _UINT32 *size, const RIL_SYSTIME *SysTime);
_VOID WaveC_WmAt0707_SetCallWait(_CHAR8* ATCmd, _UINT32 *size, _BOOL bURCEnable, _BOOL bEnable, RIL_DATA_CLASS eClass);
_VOID WaveC_WmAt0707_InquireCallWait(_CHAR8* ATCmd, _UINT32 *size, RIL_DATA_CLASS eClass);
_VOID WaveC_WmAt0707_GetExtendedError(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SetFunctionality(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strFun);
_VOID WaveC_WmAt0707_SetQSIMVOL(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strMod);
_VOID M35_WmAt0707_SetRingBack(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn);
_VOID UC15_WmAt0707_SetVBatt(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strEn);
_VOID UC15_WmAt0707_SetQFun(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strFun);
_VOID WaveC_WmAt0707_GetFunctionality(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_GetQSIMVOL(_CHAR8* ATCmd, _UINT32* size);
_VOID C202_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size);
_VOID MC323_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size);
_VOID M10_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size);
_VOID L810_WmAt0707_Reset(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_SetQMoStat(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strEn);
_VOID WaveC_WmAt0707_SetQCCINFOEn(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strEn);
_VOID WaveC_WmAt0707_SetQURCCFG(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_SetQJamDet(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strEn);
_VOID WaveC_WmAt0707_HoldAndMultiparty(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* sValue);
_VOID WaveC_WmAt0707_InquireSubscriber(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32 *size);
_VOID C202_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32 *size);
_VOID MC323_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32 *size);
_VOID CM300_WmAt0707_ListCurCalls(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_FacilityLock(_CHAR8* ATCmd, _UINT32 *size, RIL_FACILITY fac, RIL_CLCK_MODE mode, const _CHAR8* strPasswd, RIL_DATA_CLASS eClass);
_VOID WaveC_WmAt0707_CallingLineId(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strValue);
_VOID WaveC_WmAT0707_CheckCLIP(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAT0707_CheckCLIR(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAT0707_SetCLIRActive(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAT0707_SetCLIRDisable(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAT0707_DailCLIR(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strValue);
_VOID WaveC_WmAT0707_CheckCOLR(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID M10_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID UC15_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID CM300_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID MC323_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID C202_WmAt0707_SetSpeakerVol(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID M10_WmAt0707_SetFreqBand(_CHAR8* ATCmd, _UINT32* size, _SHORT16 bandindex);
_VOID WaveC_WmAt0705_SetQSFRParam(_CHAR8* ATCmd, _UINT32* size, const _SHORT16 sfrindex);
_VOID WaveC_WmAt0707_ReportMobileError(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strValue);
_VOID WaveC_WmAt0707_SetTimeZoneReporting(_CHAR8* ATCmd, _UINT32 *size, _BOOL enable);
_VOID WaveC_WmAt0707_EnterMultiplex(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strMode);
_VOID WaveC_WmAt0707_ReadOperatorNames(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_InquireBSInfo(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_InquireAvaiOperator(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SelectOperator(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strMode, const _CHAR8* strFormat, const _CHAR8* strOper);
_VOID WaveC_WmAt0707_SetEngMode(_CHAR8* ATCmd, _UINT32* size, const _UINT32 mode, const _UINT32 dump);
_VOID WaveC_WmAt0707_ReadPbEntries(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 begin, _SHORT16 end);
_VOID MC323_WmAt0707_ReadPbEntries(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 begin, _SHORT16 end);
_VOID WaveC_WmAt0707_InquirePbStorage(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SetPbStorage(_CHAR8* ATCmd, _UINT32 *size, RIL_PB_STORAGE storage);
_VOID WaveC_WmAt0707_InquirePbSpace(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_WritePbEntry(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 index, const _CHAR8* strNumber, _USHORT16 *strText);
_VOID C202_WmAt0707_WritePbEntry(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 index, const _CHAR8* strNumber, _USHORT16 *strText);
_VOID MC323_WmAt0707_WritePbEntry(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 index, const _CHAR8* strNumber, _USHORT16 *strText);
_VOID WaveC_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32 *size);
_VOID C202_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32 *size);
_VOID MC323_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_EnterPIN(_CHAR8* ATCmd, _UINT32 *size, const _CHAR8* strPIN, const _CHAR8* strNewPIN);
_VOID WaveC_WmAt0707_ChangePassword(_CHAR8* ATCmd, _UINT32 *size, RIL_FACILITY fac, _CHAR8* strOldPwd, _CHAR8* strNewPwd);
_VOID WaveC_WmAt0707_NetworkRegistration(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strValue);
_VOID WaveC_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue);
_VOID FIBOCOM_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue);
_VOID CM300_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue);
_VOID MC600_WmAt0707_InquireNetworkRegStatus(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strValue);
_VOID WaveC_WmAt0707_SetCharacterSet(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strChset);
_VOID Quectel_WmAt0707_InquireCallStatus(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_InquireSignalQuality(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_InquireNetType(_CHAR8* ATCmd, _UINT32* size);  //weikk 2016.3.31,AT XREG

_VOID C202_WmAt0707_InquireSignalQuality(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_SetSSNote(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strCSSI, _CHAR8* strCSSU);
_VOID WaveC_WmAt0707_SendUSSD(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strN, _CHAR8* strStr, _CHAR8* strDcs);
_VOID WaveC_WmAt0707_SendUssdData(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strUssdString);
_VOID WaveC_WmAt0707_SendSIMData(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strSimData);
_VOID WaveC_WmAt0707_SendATOrigin(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strATString);
_VOID WaveC_WmAt0707_CancelUssdData(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAt0707_DialDTMF(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strDtmfStr);
_VOID MC323_WmAt0707_DialDTMF(_CHAR8* ATCmd, _UINT32* size, _CHAR8* strDtmfStr, _INT32 CallIndex);
_VOID C202_WmAt0707_SetDTMFPrd(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strDtmfStr);
_VOID CM300_WmAt0707_DialDTMF(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strDtmfStr);
_VOID WaveC_WmAt0707_SetMuxMode(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strPara);
/*extended AT command*/
_VOID WaveC_WmAtExt_SetCancel(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_SetAlarm(_CHAR8* ATCmd, _UINT32 *size, RIL_CALA_ALARM *alarmData);
_VOID WaveC_WmAtExt_FieldTestReport(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strMode);
_VOID WaveC_WmAtExt_ReadSMSUnchgStat(_CHAR8* ATCmd, _UINT32 *size, _USHORT16 index);
_VOID WaveC_WmAtExt_SwitchOffModule(_CHAR8* ATCmd, _UINT32 *size);

_VOID WaveC_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);    
_VOID M10_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID UC15_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID CM300_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID MC323_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);
_VOID C202_WmAtExt_SetMicGainLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 volume);

_VOID WaveC_WmAtExt_SetAudioPath(_CHAR8* ATCmd, _UINT32 *size, RIL_AUDIO_PATH_TYPE path);
_VOID M10_WmAtExt_SetAudioPath(_CHAR8* ATCmd, _UINT32 *size, RIL_AUDIO_PATH_TYPE path);
_VOID UC15_WmAtExt_SetAudioMod(_CHAR8* ATCmd, _UINT32 *size, RIL_AUDIO_MOD_TYPE path);
_VOID S5320_WmAtExt_SetVoiceHangupCtrl(_CHAR8* ATCmd, _UINT32* size, _BOOL bEnable);
_VOID MC323_WmAtExt_SetAudioPath(_CHAR8* ATCmd, _UINT32 *size, RIL_AUDIO_PATH_TYPE path);
_VOID WaveC_WmAtExt_InquireHeadsetStatus(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_InquireHeadsetBtnStatus(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_InquireBatteryLevel(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_InquirePinCount(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_InquirePukCount(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_DumpBroadCast(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_SetRingerSoundLevel(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 level);
_VOID WaveC_WmAtExt_SetAudioOutput(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strAudioParam);
_VOID WaveC_WmAtExt_SetSmsFullUrc(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strValue);
_VOID WaveC_WmAtExt_QueryModuleState(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_InquireCOLP(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_SetColpStatus(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strValue);
_VOID WaveC_WmAtExt_CheckLatestURCEvent(_CHAR8* ATCmd, _UINT32 *size,_CHAR8* strIndex);
_VOID WaveC_WmAtExt_ControlURCIndication(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strValue);
_VOID WaveC_WmAtExt_ControlModuleDriver(_CHAR8* ATCmd, _UINT32 *size, _UCHAR8 mode);
_VOID WaveC_WmAtExt_QueryURCIndication(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_MobileEquipmentEvent(_CHAR8* ATCmd, _UINT32 *size,_CHAR8* strValue);
_VOID WaveC_WmAtExt_SendAT(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_PwrCtrl(_CHAR8* ATCmd, _UINT32 *size);
_VOID UC15_WmAtExt_PwrCtrl(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_SetMSStorageType(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strService);
_VOID WaveC_WmAtExt_SetExtSMSStorageStatus(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strService);
_VOID WaveC_WmAtExt_SetModuleSleep(_CHAR8* ATCmd, _UINT32 *size,_UCHAR8 enable);
_VOID WaveC_WmAtExt_GetModemVersionNum(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_SetChargingCurrent(_CHAR8* ATCmd, _UINT32 *size, _CHAR8* strStep);
/*used for ulc2*/
_VOID WaveC_WmAtExt_ConfigURCMode(_CHAR8* ATCmd, _UINT32 *size, RIL_URC_MODE mode);
_VOID WaveC_WmAtExt_XDRVURCControl(_CHAR8* ATCmd, _UINT32 *size, RIL_sXDRV_TYPE *xdrvType);
_VOID WaveC_WmAtExt_XDRVAudioControl(_CHAR8* ATCmd, _UINT32 *size,RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType, _CHAR8* strValue);
_VOID WaveC_WmAtExt_SetTraceOutput(_CHAR8* ATCmd, _UINT32 *size,_BOOL bOnOff);
_VOID WaveC_WmAtExt_GetModemExcLog(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_XDRVControlNewSMSStatus(_CHAR8* ATCmd, _UINT32 *size, _BOOL enable);
_VOID WaveC_WmAtExt_SetSignalStrengthReport(_CHAR8* ATCmd, _UINT32 *size, _BOOL enable);
_VOID WaveC_WmAtExt_ReportCallStat(_CHAR8* ATCmd, _UINT32 *size);
_VOID WaveC_WmAtExt_XDRVControlAudioLoopTest(_CHAR8* ATCmd, _UINT32 *size, _BOOL enable);
/*end for ulc2*/
/*stk AT command*/
_VOID WaveC_STK_SetProfile(_CHAR8* ATCmd, _UINT32 *size);
_VOID M35_STK_SetAutoRsp(_CHAR8* ATCmd, _UINT32* size);
_VOID M35_STK_SetTR(_CHAR8* ATCmd, _UINT32* size);

_VOID WaveC_STK_SelectMainMenu(_CHAR8* ATCmd, _UINT32 *size, _SHORT16 index);
_VOID WaveC_STK_Response(_CHAR8* ATCmd, _UINT32 *size, RIL_RES_PAR *par);
_VOID WaveC_WmAt0705_SetBatteryParam(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SetToneDetParam(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SetDAIParam(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SelectVoiceGain(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SetSideTone(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SetQindi(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SetQiDnsip(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID M10_WmAt0705_SetSideTone(_CHAR8* ATCmd, _UINT32* size, const _CHAR8* strParam);
_VOID WaveC_WmAt0705_SaveConfiguration(_CHAR8* ATCmd, _UINT32* size);

/* ext    */
_VOID M10_WmAtV25_InquireCCID(_CHAR8* ATCmd, _UINT32* size);
_VOID C202_WmAtV25_InquireCCID(_CHAR8* ATCmd, _UINT32* size);

_VOID M10_WmAtV25_InquireIMSI(_CHAR8* ATCmd, _UINT32* size);

_VOID CM300_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size);
_VOID C202_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size);
_VOID MC323_WmAt0707_InquireCurOperator(_CHAR8* ATCmd, _UINT32* size);
_VOID WaveC_WmAt0707_InquireInitStatus(_CHAR8* ATCmd, _UINT32* size);
_VOID CM300_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size);
_VOID MC600_WmAt0707_InquirePinStatus(_CHAR8* ATCmd, _UINT32* size);

#endif

