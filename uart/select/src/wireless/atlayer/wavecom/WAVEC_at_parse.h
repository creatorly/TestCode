
#ifndef WAVEC_AT_PARSE_H
#define WAVEC_AT_PARSE_H

#include "eos.h"
#include "wm_struct.h"

#define ATCMD_RETURN            "\r"
#define AT_NEW_LINE                "\n"
#define AT_COMMA                ","

#define STK_STRING_COMMAND                    "+STKPRO: "
#define STK_STRING_OK                        "+STKPRO: OKAY"
#define STK_STRING_ERROR                    "+STKPRO: ERROR"
#define STK_PROACTIVE_END                    "+STKPRO: Proactive End"
#define STK_STRING_STKCNF                    "+STKCNF: "

/*for parse at cmd  */
#define AK_OK_POS_5                5
#define AK_ERROR_POS_15            15
#define AK_STRING_BEG            0    /* beginning of string */

#define AK_CLCK_SKIPCLCK_7      7   /* strlen(+clck:) = 7*/
#define AK_CLCK_SKIPATCLCK_9    9    /* strlen(AT+CLCK<CR><LF>) = 9 */

#define AK_CCFC_SKIPCCFC_7      7   /* strlen(+ccfc:) = 7 */
#define AK_CCFC_NACTIVE_0        0    /* forward off */
#define AK_CCFC_ACTIVE_1        1    /* forward on */

#define AK_CPBW_SKIPCPBW        7    /* strlen(+cpbw:) = 7, skip the string */
#define AK_CPBR_CPBRLEN_7        7

#define AK_CPBR_CPBRLEN_6        6


#define AK_CLCC_ATCLCCLEN_7        7
/*Kinson
#define AK_CLCC_ATCLCCLEN_7        0
*/


#define AK_CLCC_CLCCLEN_7        7
#define AK_CLCC_CLCCLEN_6        6


#define AK_CLCC_TYPELEN_3        3

#define AK_CMGR_ATCMGRLEN_7        7
#define AK_CMGR_CMGRLEN_7        7
#define AK_CMGR_CMGRLEN_CDMA_6    6

#define AK_CMGL_ATCMGLLEN_7        7
#define AK_CMGL_CMGLLEN_7        7

#define AK_CPMS_ATLEN_7            7
#define AK_CPMS_CPMSLEN_7        7

#define AK_CSCA_ATCSCALEN_7        7
#define AK_CSCA_CSCALEN_7        7

#define AK_COPS_ATCOPSLEN_7        7
#define AK_COPS_COPSLEN_7        7
#define AK_COPS_ATCOPSLEN_9        9

#define    AK_CLIP_LEN_7            7
#define AK_COLP_LEN_7            7
#define AK_COLR_LEN_7            7
#define AK_SCCFC_LEN_8            8
#define AK_CCWA_LEN_7            7
#define AK_CME_LEN_12            12
#define AK_CEER_LEN_7            7
#define AK_CUSD_LEN_7            7
#define AK_CREG_CREGLEN_7        7
#define AK_CPIN_SKIPCPIN_7        7
#define AK_CTZV_LEN_7            7    /*(+CTZV:) */ 

#define MAX_CUSD_LEN            380    
#define SET_CANCEL_SIZE            3     /*allot 3 space,cancel last  cmd*/
#define AT_CONTROL_Z            26
#define MAX_NETWORK_NUM         10
#define MODEM_CHIP_ID_LEN        20
/*end for parse at cmd  */
/*allot space for parse at cmd  */
#define AT_ERROR_LEN                20     /*include ERROR type and ERROR ID */
#define SEND_STANDARDAT_SIZE        100    /*allot 100 space, AT+... */
#define SAVE_SMSPDU_SIZE            100    /*allot 100 space, AT+CMGW */
#define INPUIRE_SMSSTORAGE_SIZE        100    /*allot 100 space, AT+CPMS= */
#define SELECT_BROADCAST_SIZE        100    /*allot 100 space, AT+CSCB= */
#define SWITCH_DATATOATMODE_SIZE    100    /*allot 100 space, +++ */
#define DEFINE_PDPCONTEXT_SIZE        100    /*allot 100 space, AT+CGDCONT= */
#define RESET_ACM_SIZE                100    /*allot 100 space, AT+CACM= */
#define SET_ACMMAX_SIZE                100    /*allot 100 space, AT+CAMM= */
#define SET_CALLFORWARD_SIZE        100    /*allot 100 space, AT+CCFC= */
#define SET_CLOCK_SIZE                100    /*allot 100 space, AT+CCLK= */
#define SET_CALLWAIT_SIZE            100    /*allot 100 space, AT+CCWA= */
#define SET_FACILITYLOCK_SIZE        100    /*allot 100 space, AT+CLCK= */
#define SET_DAILCLIR_SIZE            100    /*allot 100 space, ATD */
#define SET_SPEAKERVOL_SIZE            100    /*allot 100 space, AT+CLVL= */
#define SELECT_OPERATOR_SIZE        100    /*allot 100 space, AT+COPS= */
#define READ_PBENTRY_SIZE           100    /*allot 100 space, AT+CPBR= */
#define ENTER_PIN_SIZE              100    /*allot 100 space, AT+CPIN= */
#define CHANGE_PWD_SIZE             100    /*allot 100 space, AT+CPWD= */
#define SET_CHARACTERSET_SIZE       100    /*allot 100 space, AT+CSCS= */
#define SET_SSNOTE_SIZE             100    /*allot 100 space, AT+CSSN= */
#define SET_ALARM_SIZE              100    /*allot 100 space, AT+CALA= */
#define READ_SMSUNCHGSTAT_SIZE      100    /*allot 100 space, AT+CMGR= */
#define SET_MICGAINLEVEL_SIZE       100    /*allot 100 space, AT+CMIC= */
#define SET_RINGERSOUNDLEVEL_SIZE   100    /*allot 100 space, AT+CRSL */
#define CONTROL_MODULEDRI_SIZE      100    /*allot 100 space */
#define MAIN_MENUSELECT_SIZE        100    /*allot 100 space,AT+STKENV=211, */
#define SET_SMSCENTER_SIZE          200    /*allot 200 space, allot space for at+csca */
#define WRITE_PBENTRY_SIZE          200    /*allot 200 space, allot space for AT+CPBW= */
#define SEND_USSD_SIZE              200    /*allot 200 space, allot space for AT+CUSD= */
#define DAIL_CALL_SIZE              300    /*allot 300 space, ATD */
#define STK_RESPONSE_SIZE           1024    /*allot 1024 space, AT+STKTR= */
/*end allot space for parse at cmd  */
/* need for parse stk at cmd*/
#define SPIC_STRING_COMMAND_LEN                7
#define STK_STRING_COMMON_LEN                11

#define STK_STRING_REFRESH                    "+STKPRO: 01"
#define STGC_STRING_SETUPEVENTLIST            "+STKPRO: 05"
#define STK_STRING_SETUPCALL                "+STKPRO: 16"
#define STK_STRING_SENDSS                    "+STKPRO: 17"
#define STK_STRING_SENDUSSD                    "+STKPRO: 18"
#define STK_STRING_SENDSM                    "+STKPRO: 19"
#define STGC_STRING_SENDDTMF                "+STKPRO: 20"
#define STGC_STRING_LAUNCHBROWSER            "+STKPRO: 21"
#define STK_STRING_PLAYTONE                    "+STKPRO: 32"
#define STK_STRING_DISPLAYTEXT                "+STKPRO: 33"
#define STK_STRING_GETINKEY                    "+STKPRO: 34"
#define STK_STRING_GETINPUT                    "+STKPRO: 35"
#define STK_STRING_SELECTITEM                "+STKPRO: 36"
#define STK_STRING_SETUPMENU                "+STKPRO: 37"
#define STK_STRING_SETUPIDLEMODETEXT        "+STKPRO: 40"
#define STK_STRING_LANGUAGENOTIFICATION        "+STKPRO: 53"
/* end need for parse stk at cmd*/

 /* function declare*/
_VOID* WaveC_ParseOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_ParseQiRawDataOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID *M10_WmPsGsmCd_QuerySideTone(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_OriginATResponse(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_ParseDataConnect(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_ParseDataConnectCSD(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_ParseDataResult(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_ParseCMGSOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_InquireQiStat(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0705_InquireQiSack(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0705_InquireQiLocIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0705_InquireQiRcvData(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_ParseQISendOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* SIM800_WmPsV25_InquireCipAck(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* SIM800_WmPsV25_InquireCipRxget(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* SIM800_WmPsV25_SendCipData(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0707_InquireCellLoc(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* MC323_ParseHCMGSOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_ParseCMGWOkError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);

 /* parse GSM0707 AT command */
_VOID* WaveC_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireInitStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* C202_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC323_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* CM300_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC600_WmPs0707_InquirePinStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_SetPin(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* FIBOCOM_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* FIBOCOM_WmPs0707_InquireRegisterStatus_With_Cid(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* CM300_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC600_WmPs0707_InquireRegisterStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* C202_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* CM300_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC323_WmPs0707_ListCurCalls(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireClock(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireLock(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquirePbStorage(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquirePbSpace(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_ReadPbEntries(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* C202_WmPs0707_ReadPbEntries(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC323_WmPs0707_ReadPbEntries(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_ReadOperatorNames(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* UC15_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0707_InquireBSInfo(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* CM300_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* C202_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC323_WmPs0707_InquireCurOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0707_InquireAvaiOperator(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireSubscriber(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_GetExtendedError(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireSignalQuality(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_InquireNetType(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);//weikk 2016.3.31,add XREG
_VOID* WaveC_WmPs0707_InquireADC(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);//weikk 20160713, add adc

_VOID* Quectel_WmPs0707_InquireCallStatus(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* MC323_WmPs0707_InquireSignalQuality(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_GetCurrentAcm(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_GetACMMax(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0707_GetCurCallMeter(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
/*parse GSM0705 AT command*/
_VOID* WaveC_WmPs0705_InquireSmsStorage(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_ReadSMS(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* M35_WmPs0705_ReadSMS(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* MC323_WmPs0705_ReadSMS(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_ReadSMS_CDMA(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_ParseSimSms(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_InquireSmsCenter(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_CbsSetting(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_SaveSmsPduData(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPs0705_GetFun(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPs0705_GetSimVol(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);

/*parse GSM code AT command*/
_VOID* M10_WmPsGsmCd_QueryCCID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* C202_WmPsGsmCd_QueryCCID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* M10_WmPsGsmCd_QueryIMSI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPsGsmCd_QueryIMEI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* M10_WmPsGsmCd_QueryIMEI(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* C202_WmPsGsmCd_QueryESN(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* CM300_WmPsGsmCd_QueryESN(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32 *strNum, _UINT32 *strSize);
_VOID* WaveC_WmPsGsmCd_pCCWAProcess(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CallWait(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CallForward(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CheckCOLR(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CheckCOLP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CheckCLIR(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CheckCLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_CallBarring(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsGsmCd_QueryBand(RIL_PARSE_RES*result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);

/*parse data connection command*/
_VOID *WaveC_WmPsGprs_InquireGprsState(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);

/*parse specific AT command*/
_VOID* WaveC_WmPsExt_InquSMSFULL(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquirePinCount(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireBatteryLevel(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireBatteryVoltage(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireEnvirTemper(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireBattTemper(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireBattTemperADC(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireStaticEEP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_InquireChipID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);

_VOID* WaveC_WmPsExt_ParseModuleState(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_FieldTestReport(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_CheckLatestURCEvent(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_ControlURCIndication(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsV25_GetProductID(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID *WaveC_WmPsExt_QueryURCIndication(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);


/*parse urc AT  command*/
_VOID* WaveC_WmPsUrc_CREG(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* C202_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* CM300_WmPsUrc_CLIP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID *WaveC_WmPsUrc_COLP(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsUrc_USSD(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsUrc_CallProgress(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);

_VOID *WaveC_WmPsUrc_STK(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsExt_SignalStrengthURC(RIL_PARSE_RES* result, const _CHAR8* strResult, _UINT32* strNum, _UINT32* strSize);
_VOID* WaveC_WmPsUrc_TimeZoneReporting(RIL_PARSE_RES* result, const _CHAR8* strResult);

#endif




