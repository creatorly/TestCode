
#ifndef  WIRELESS_INTERFACE_INC_H
#define  WIRELESS_INTERFACE_INC_H

#include <utl_select.h>
#include <utl_timer.h>
#include "utils/ut_msgq.h"

#include "wireless_list.h"
#include "line.h"
#include "dataconn_manager.h"
#include "proxycom.h"
#include "proxy.h"
#include "wm_channel.h" 
#include "utl_httpc.h"
#include "wireless_common.h"
#include "wm_interface.h"

#define WIRELESS_MEM_32_CNT              (48*4)
#define WIRELESS_MEM_64_CNT              (24*4)
#define WIRELESS_MEM_128_CNT             (16*4)
#define WIRELESS_MEM_256_CNT             (16*4)
#define WIRELESS_MEM_512_CNT             (16*4)
#define WIRELESS_MEM_1024_CNT            (2*16)
#define WIRELESS_MEM_2048_CNT            (1*4)
#define WIRELESS_MEM_4096_CNT            (1*4)

#define WIRELESS_EXTCOMBUF_MAXSIZE       (4*1024)
#define WIRELESS_COMPLETEINFO_MAXSIZE    (1*1024)

#define WIRELESS_COM_BAUDRATE_INDEX       4 /* 9600*/  /* 2  38400 */  /* 1  57600    */  /* 0  115200 */
#define WIRELESS_COM_BAUDRATE_INDEX_M10   /*4 9600*/   2/* 38400 */    /* 1 57600    */  /* 0  115200 */

#define WIRELESS_ATDBG_MAX_MSGQS          16
#define WIRELESS_ATDBG_MAX_MSGLEN         256

#define WIRELESS_MODULE_DET_RESTART_MAX   4
#define WIRELESS_MODULE_DETECT_MAX        16
#define WIRELESS_SIM_DETECT_MAX           16
#define WIRELESS_INQ_SMSC_MAX             16
#define WIRELESS_PREPARE_INIT_TIME        10000 /* msecond */

#define WIRELESS_MODULE_DETECT_PERIOD     1000  /* msecond */
#define WIRELESS_SIM_DETECT_PERIOD        1000  /* msecond */

#define WIRELESS_SET_OPR_FORMAT_MAX       60
#define WIRELESS_SET_OPR_FORMAT_PERIOD    2000  /*msecond */

#define WIRELESS_OPR_INQUIRE_MAX          24
#define WIRELESS_OPR_INQUIRE_PERIOD       1000  /* msecond */

#define WIRELESS_SET_NETREG_PERIOD        3000  /* msecond */

#define WIRELESS_SET_NEWSMSSTOREMODE_MAX       64
#define WIRELESS_SET_NEWSMSSTOREMODE_PERIOD    2000  /* msecond */

#define WIRELESS_SEND_USSD_INQUIRE_MAX         3

#define WIRELESS_INQUIRE_NET_REG_PERIOD        5000  /* msecond */

#define WIRELESS_FUNCTION_ON_WAITING_TIME      60000 /* msecond */

#define WIRELESS_READ_IMEI_MAX                 4
#define WIRELESS_READ_IMEI_PERIOD              10000 /* msecond */
#define WIRELESS_READ_CCID_MAX                 6

#define WIRELESS_SIMLOST_RESTART_MAX           4
#define WIRELESS_OPSLOST_RESTART_MAX           8

#define WIRELESS_SIMSLOT_NUM_MAX               4

#define WIRELESS_CCIDINFO_MAX_SIZE             128

#define WRL_INIT_MODULE_IDX                    0
#define WRL_MAIN_MODULE_IDX                    1

#define WRL_GET_GPS_MAX_CNT                   (60*10)  

/* for wrl fsm */
#define WRL_FSM_SIM_DET_RETRY_MAX              24
#define WRL_FSM_SIM_DET_AIRON_CNT             (WRL_FSM_SIM_DET_RETRY_MAX + 1)

#define WRL_SMS_PACKAGE_MAX                    8

#define WRL_LOCSIM_CHN                         0
#define WRL_RMTSIM_CHN                         1
#define WRL_BAKSIM_CHN                         2

#define WRL_QUERY_DIALSTATE_PERIOD            (1000)

#define WRL_QUERY_HOLDPPP_TIME                (1000)

#if (ACOM_BOARD_TYPE == MT673X)
#define WRL_SIMDATA_NUM_CHECK_TIME            (10000)
#elif (ACOM_BOARD_TYPE == MDM9X07)
#define WRL_SIMDATA_NUM_CHECK_TIME            (3000)
#else
#define WRL_SIMDATA_NUM_CHECK_TIME            (5000)
#endif

#define WRL_DATADIAL_CHECK_TIME            (10000)


#define WRL_RMT_UIM_EVENT_MAX                   16

#if (ACOM_BOARD_TYPE == MDM9X07)
#define WRL_SIMDATA_NUM_LIMIT                  8   //apdu id
#else
#define WRL_SIMDATA_NUM_LIMIT                  128 //in bytes
#endif

#define WRL_APCLIENT_NUM_MAX     8

#if (ACOM_BOARD_TYPE == SC98XX)
#define WRL_LTE_SIM_REMOTE_SLOT  1
#define WRL_LTE_SIM_LOCAL_SLOT   0
#else
#define WRL_LTE_SIM_REMOTE_SLOT  0
#define WRL_LTE_SIM_LOCAL_SLOT   1
#endif

#define WRL_TCPIP_PROTO_HEADER_LEN              60      /* in bytes */

#if 1    
#define WirelessSemP(sem, timeout) \
do{ \
    /*printf("%s(%d): envoke function eos_sem_p!\r\n", __FILE__, __LINE__);*/ \
    eos_sem_p(sem, timeout); \
}while(0)

#define WirelessSemV(sem) \
do{ \
    /*printf("%s(%d): envoke function eos_sem_v!\r\n", __FILE__, __LINE__);*/ \
    eos_sem_v(sem); \
}while(0)
#else
#define WirelessSemP(sem, timeout)
#define WirelessSemV(sem)
#endif

typedef struct wirelesslistnode{
    _NODE          stnode;
    _UINT32        channelid;
    _UINT32        op_type;
    _UINT32        Value;
    _CHAR8*        pcStr;
    _CHAR8         acValue[1460];
}WIRELESSLISTNODE,*PWIRELESSLISTNODE;

typedef struct taglineManager{
    struct list_head    head;        /*line的链表头*/
    EOS_SEM_ID          mutex;        /*临界区,操作链表时,要互斥*/
    _BOOL               bInit;
} T_LINEMANAGER;

typedef struct tagdataconnparam{
    HLINE          hLine;    /*发起请求的line的句柄*/
    _UINT32        hATServer;
    _UINT32        customParam; /*如果请求是line的请求,记录下line的custom Parameter信息*/
    T_DATACALL_OPERATION operation;
}T_DATACONNPARAM;

enum
{
    EN_WIRELESS_ST_MODULE_WAITING_INIT = 0,
    EN_WIRELESS_ST_SET_BAUDRATE,          /* 1 */
    EN_WIRELESS_ST_SET_BAUDRATE_TRY,
    EN_WIRELESS_ST_MODULE_INIT,          
    EN_WIRELESS_ST_MODULE_DETECT,
    
    EN_WIRELESS_ST_IMEI_READ,
    EN_WIRELESS_ST_IMEI_READ_ERROR,
    EN_WIRELESS_ST_SIM_WAIT_INIT,
    EN_WIRELESS_ST_SIM_INIT,
    EN_WIRELESS_ST_MODULE_LOST,
     
    EN_WIRELESS_ST_SIM_LOST,                
    EN_WIRELESS_ST_SET_COPS_FORMAT,
    EN_WIRELESS_ST_SET_COPS_FORMAT_ERR,    
    EN_WIRELESS_ST_SET_NETWORK_REG,
    EN_WIRELESS_ST_OPR_INQUIRE,    
    
    EN_WIRELESS_ST_BLANCE_INQUIRE,
    EN_WIRELESS_ST_OPR_INQ_ERR,
    EN_WIRELESS_ST_SET_NEWSMSSTOREMODE,
    EN_WIRELESS_ST_NEWSMSSTOREMODE_ERR,
    EN_WIRELESS_ST_NORMAL,   /* 19 */                 
    EN_WIRELESS_ST_BUTT
 };

typedef enum
{
    EN_WRL_STATE_PWRON = 0,
    EN_WRL_STATE_IMEI,
    EN_WRL_STATE_SIM,
    EN_WRL_STATE_REGIST,
    EN_WRL_STATE_SWITCH,
    EN_WRL_STATE_PPPDIAL,
    EN_WRL_STATE_BUTT
}EN_WRL_STATE_INDEX;

typedef enum
{
    /* Power On State*/
    EN_WRL_SUBSTATE_PWRON_IDLE = 0,
    EN_WRL_SUBSTATE_PWRON_HWTRIG,         
    EN_WRL_SUBSTATE_PWRON_RESET,        
    EN_WRL_SUBSTATE_PWRON_SET_BAUDRATE,
    EN_WRL_SUBSTATE_PWRON_BAUDRATE_TRY,
    EN_WRL_SUBSTATE_PWRON_MODULE_DETECT,   //5
    
    /* IMEI State    */
    EN_WRL_SUBSTATE_IMEI_READ,    
    EN_WRL_SUBSTATE_IMEI_WRITE,
    EN_WRL_SUBSTATE_IMEI_AIRON,
    EN_WRL_SUBSTATE_IMEI_AIROFF,
    
    /* SIM State    */    
    EN_WRL_SUBSTATE_SIM_IDLE,   /* when sim slot is empty or read sim failed, be in this state    */  //10
    EN_WRL_SUBSTATE_SIM_CHECK_READY,
    EN_WRL_SUBSTATE_SIM_AIRON,
    EN_WRL_SUBSTATE_SIM_AIROFF,
    EN_WRL_SUBSTATE_SIM_UNREADY,

    /* Regist State    */    
    EN_WRL_SUBSTATE_REGIST_SET_SPECIFICOPR,     //15
    EN_WRL_SUBSTATE_REGIST_SET_OPRFORMAT,    
    EN_WRL_SUBSTATE_REGIST_CHECK_READY,
    EN_WRL_SUBSTATE_REGIST_GET_OPRINFO,
    EN_WRL_SUBSTATE_REGIST_REG_DENY,
    EN_WRL_SUBSTATE_REGIST_REG_UNKNOWNNET,   //20
    EN_WRL_SUBSTATE_REGIST_REG_NONETWORK,
    EN_WRL_SUBSTATE_REGIST_REG_OK_AND_CHECK_ALL,
    EN_WRL_SUBSTATE_REGIST_ALL_READY,

    /* Switch State    */    
    EN_WRL_SUBSTATE_SWITCH_IMEI_WRITE,
    EN_WRL_SUBSTATE_SWITCH_ON,                /* C202 don't has this status */  //25
    EN_WRL_SUBSTATE_SWITCH_ON_WAIT,         /* C202 don't has this status */
    EN_WRL_SUBSTATE_SWITCH_SWSIMSLOT,    
    EN_WRL_SUBSTATE_SWITCH_OFF,

    EN_WRL_SUBSTATE_PPP0DIAL_START,
    EN_WRL_SUBSTATE_PPP0DIAL_ONGOING,   //30
    EN_WRL_SUBSTATE_PPP0DIAL_OK,   //31
    EN_WRL_SUBSTATE_PPP0DIAL_FAIL_GIVEUP,

    EN_WRL_SUBSTATE_PPP1DIAL_START,
    EN_WRL_SUBSTATE_PPP1DIAL_ONGOING,
    EN_WRL_SUBSTATE_PPP1DIAL_OK,
    EN_WRL_SUBSTATE_PPP1DIAL_FAIL_GIVEUP,

    EN_WRL_SUBSTATE_BUTT
}EN_WRL_SUBSTATE_INDEX;

 typedef enum
 {
     EN_WRL_FSM_EVENT_ID_SWITCHSLOT = 0,
     EN_WRL_FSM_EVENT_ID_PWRON,
     EN_WRL_FSM_EVENT_ID_REGUNREADY,
 }EN_WRL_FSM_EVENT_ID_INDEX;

 typedef struct FSMMsgQ
 {
    _UINT32 ChannelID;
    _UINT32 EventType;
    EN_WRL_FSM_EVENT_ID_INDEX EventID;
    _CHAR8    Data[256];
 }ST_WRL_FSM_MSGQ;

enum
{
    EN_WIRELESS_ST_LED_NORMAL = 0,
    EN_WIRELESS_ST_LED_FLASHON,
    EN_WIRELESS_ST_LED_FLASHOFF,
    EN_WIRELESS_ST_LED_CYCLICON,
    EN_WIRELESS_ST_LED_CYCLICOFF,
    EN_WIRELESS_ST_LED_BUTT
 };

enum
{
    EN_WIRELESS_ST_FUNCTION_OFF_WAITING = 0,
    EN_WIRELESS_ST_FUNCTION_OFF,
    EN_WIRELESS_ST_FUNCTION_ON,
    EN_WIRELESS_ST_FUNCTION_NORMAL,
    EN_WIRELESS_ST_FUNCTION_BUTT
};

enum
{
    EN_WIRELESS_SIM_UNKNOWN = 0,
    EN_WIRELESS_SIM_QUERYING,
    EN_WIRELESS_SIM_NEW,
    EN_WIRELESS_SIM_OLD,
    EN_WIRELESS_SIM_BUTT
};

#define WRL_ATR_FIRST_CMD_ERROR   -100
#define WRL_RMTSIM_INVALID_CLASS   -200

typedef struct
{
    _UINT32    CurrentSendPkgID;
    _UINT32 TotalSendPkgCnt;
    _UCHAR8    SMSMR;
    _CHAR8  strPdu[WIRELESS_MAX_SMS_PKT_NUMBER][WIRELESS_MAX_SMS_PKT_PDU_LEN];       
    _BOOL    SendStatus[WIRELESS_MAX_SMS_PKT_NUMBER];
}WIRELESSSMSST, *PWIRELESSSMSST;

/* for SMS */
typedef struct {
    _CHAR8 SCA[16];            
    _CHAR8 TPA[16];            
    _CHAR8 TP_PID;            
    _CHAR8 TP_DCS;            
    _CHAR8 TP_SCTS[16];    
    _CHAR8 TP_UD[800];        
    _USHORT16 index;            
} SM_PARAM_GSM;

typedef struct {
    _CHAR8 SCA[32];            
    _CHAR8 TPA[32];            
    _CHAR8 TP_PID;            
    _CHAR8 TP_DCS;            
    _CHAR8 TP_SCTS[32];    
    _UINT32 uiCurrentLoc;
    _CHAR8 TP_UD[2048];
    _CHAR8 TP_SUB_UD[WRL_SMS_PACKAGE_MAX][512];
    _UINT32 TP_SUB_LEN[WRL_SMS_PACKAGE_MAX];
    _BOOL  bUDFilled[WRL_SMS_PACKAGE_MAX];
    _UINT32 uiTotalPktCnt;
    _UINT32 uiCurrentPktIndex;
    _USHORT16 index;            
}SM_SEND_PARAM_GSM;

typedef struct {
    unsigned char ifname[10];
    unsigned char ipaddr[20];
    unsigned char proto[10];
    unsigned char service[10];
    unsigned char apn[32];
    unsigned char num[32];
    unsigned char usr[32];
    unsigned char pwd[32];
    unsigned char device[20];
    unsigned char isParamAlter;  //0 = not right , 1 = is ok
}ST_PPPD_PARAM;

typedef struct {
    unsigned char isCardReady;
    unsigned char dialCmd;  //cmd :0->non,1->disconnect,2->connect. 
    unsigned char isDialing;
    unsigned char dialTime;
    unsigned char dialTimeMax;
    unsigned int  waitTime;
    _BOOL        pppok;
}ST_SIM_STATUS;

typedef struct wirelesschannelst
{
    _UCHAR8* pucExtComBuf;
    _UCHAR8* pucCompleteInfoBuf;

    _UINT32  uiChannelId; 
    _UINT32  uiExtComHead; 
    _UINT32  uiExtComTail;
    _UINT32  uiMaxComBuffLen;

    _UINT32  ModuleType; /*     M10: 0,2406: 1, C202: 2, MC323: 3, MC600: 4, S5320: 5, CM300: 6 */
    _UINT32  ModPwrOffDuration;   /* unit:ms*/
    _UINT32  SimSlot;
    _BOOL    bFirstSwitch;
    _BOOL    bEnterPinLock;
    
    _INT32   fUartHandle;

    _UINT32  uiUartWriteErrCnt;

    _INT32   ModuleDSIMIndex;
     
    _UINT32  SetBaudrateIndex;

    _BOOL    bReportReady;
    _BOOL    bIncomingRingRpt;
    _BOOL    bESPEnable;
    _BOOL    bReadyDialing;
        
    _INT32   Signal;
    _UINT32  CallStatus;
    /**used to store the result of the synchronize request*/
    _BOOL       bGetResult;
    T_MESSAGE   stReqReply;
    
    T_LINEMANAGER     stLineManager;
    T_DATACONNMANAGER *pstManager;
    HATSERVER         stHATServer;

    _UINT32                uiRespTimeoutCnt;    
    _UINT32                uiModJammingCnt;
    _UINT32                uiUnderVoltageWarningCnt;
    _UINT32                uiUnderVoltagePwroffCnt;
    _UINT32                uiOverVoltageWarningCnt;
    _UINT32                uiOverVoltagePwroffCnt;
    _UINT32                uiNoAlertCnt;
    _UINT32                uiToneDetectCnt;

    RIL_INIT_STATUS        ModInitStatus;
    _UINT32             enState;
    _UINT32             uiModuleDetectTick;
    _UINT32             uiSimDetectCnt;
    _UINT32             uiSimDetectRetryCnt;
    _UINT32             uiSIMDetectTick;
    _UINT32             uiInqSMSCCnt;

    /* State Machine    */
    EN_WRL_STATE_INDEX       enFSMSTATEIndex;
    EN_WRL_SUBSTATE_INDEX    enFSMSubSTATEIndex;
    EN_WRL_SUBSTATE_INDEX    enFSMPreSubSTATEIndex;

    _UINT32             NextSimSlot;

    /* a AT command is sent, wait response before the timer fired */
    _UINT32             tmr_wait_atrsp;

    _INT32              iDataState;

    /* for Module    */        
    _UINT32             uiModRegCnt;
    _UINT32             uiModResetCnt;
    _UINT32             uiModuleDetRestartCnt;
    _UINT32             uiModuleDetectCnt;
    _UINT32             uiModulePwrOffCnt;
    
    _UINT32             uiOprInquireCnt;
    _UINT32             uiOprInquireTick;
    _UINT32             uiSetNegRegTick;
    
    _UINT32             uiIMEIReadCnt;
    _UINT32             uiIMEIReadTick;
    
    _UINT32             uiSetCOPSFormatCnt;
    _UINT32             uiSetCOPSFormatTick;    
    _UINT32             uiSetNewSMSStoreModeCnt;
    _UINT32             uiSetNewSMSStoreModeTick;

    _UINT32             uiCCIDPBReadCnt;
    _UINT32             uiInquireNetRegStatusTick;

    _BOOL               bUpdateBSInfo;
    _UINT32             uiBSInqStartTick;
    _UINT32             uiInsertSIMTick;

    RIL_NET_STAT        NetRegStatus;

    _BOOL               bPPPOk;
    _BOOL               bWaitingToDial;

    ST_PPPD_PARAM       pppdParam;
    ST_SIM_STATUS       LoginSimStatus;

    _UINT32             uiLostStartTick;
    _BOOL               bDetNetLost;
    _BOOL               bRegUpdateWaiting;

    _UINT32             uiLastATCmdSendTick;
    _UINT32             uiSimLostRestartCnt;
    _UINT32             uiOprLostRestartCnt;
    
    _UINT32   uiFxoRingStartTicks;    
    _LIST     stCmdList;
    _UINT32   gb_hATServer;
    HRIL      gb_hCallRIL;
    HRIL      gb_hMsgRIL;
    HRIL      gb_hSIMRIL;
    HRIL      gb_hSTKRIL;

    EOS_TIMER_ID        BattQueryTimerID;
    EOS_TIMER_ID        BattTestTimerID;
    

    T_HCHNL   *pChnl;
    _UINT32   hCallHandle;

    _BOOL     bModuleExisted;
    _BOOL     bSIMExisted;
    _BOOL     bReadyReport;
        
    _UINT32   enFuctionStatus;
    _UINT32   uiFunctionOnTick;

    
    _BOOL     bSlotSIMExisted[WIRELESS_SIMSLOT_NUM_MAX];

    _UINT32   SlotSIMIsNew;
    _CHAR8    CCIDInfo[WIRELESS_SIMSLOT_NUM_MAX][WIRELESS_CCIDINFO_MAX_SIZE];

    _UINT32 uiAllCallQty;
    _UINT32 uiActiveQty;
    _UINT32 uiHoldQty;
    _UINT32 uiDialingQty;
    _UINT32 uiWaitingQty;
    _UINT32 uiIncomingQty;

    _BOOL   bTalkStart;
    _BOOL   bCallStart;
    _BOOL   bBusyStart;
    _UINT32 uiCallStartTick;

    _BOOL   bWaitATDResponse;
    _BOOL   bIgnoreATDResponse;
    _BOOL   bATDSendFakeResponse;
    _BOOL   bATDCLCCWaitOKResponse;
    
    /* For USSD Function    */
    _UINT32 uiSendUSSDStartTick;
    _UINT32 uiReadUSSDCnt;
    _INT32  iUssdBalanceLeft;
    _UINT32 uiUSSDRetryTime;
    _UINT32 uiUSSDZeroBalanceRetryTime;
    
    _BOOL   bIsOffHook;
    _UINT32 uiOnHookStartTick;
    
    _BOOL   bIsRinging;
    _BOOL   bStartChkRingOff;
    _UINT32 uiRingOffTick;
    EOS_SEM_ID     ComBufMutex;
    _UINT32 uiUSSDLedCyclicStatus;
    _UINT32 uiUSSDLedFlashStatus;

    T_ATCMD    stATCmd;

    _UINT32 OperatorID;
    _UINT32 OprtMCC;  //460 for china mainland
    _UINT32 OprtMNC;
    
    _BOOL   bModulePowerOff;
    _UINT32 uiModulePowerOffTick;
    _BOOL   bModuleNeedPowerOn;
    _BOOL   bModuleNeedHWPwrOn;
    _UINT32 uiModuleHWPwrOnTick;

    _VOID*  UrcParse_temp;
    _BOOL    recSmsPdu;
    _BOOL    recCdsPdu;
    _BOOL    recBcmPdu;

    _BOOL   bSimDataIgnoreOk;

    /* for no carrier detecting    and analyzing    */
    _UINT32 uiBarringDetectCnt;

      /* for statistics   */
    _UINT32    uiGetUSSDErr;        

    /* call index    */
    _INT32 iCallIndex;

    /*    Module Update    */
    _BOOL bModuleUpdating;
    
      /* SMS  */  
    WIRELESSSMSST stSMSSend;
    SM_SEND_PARAM_GSM SMSSendDst; 
    _CHAR8  strSMSFwdNumber[32];  
    _CHAR8  strSMSSendNumber[32];
    _CHAR8  strSMSC[64];
    
    _CHAR8  USSDCmdBuffer[480];
    _CHAR8  USSDBuff[512];    
    _CHAR8  USSDInvalidBalanceKeyStr[128];
    _CHAR8  USSDInvalidSimKeyStr[128];
    
}WIRELESSCHANNELST,*PWIRELESSCHANNELST;

typedef enum {
    NONE_CONNECT = 0, 
    CONNECT_OK, 
    CONNECT_FAIL,
    ALREAY_CONNECT,
    IP_INITIAL,
    IP_START,
    IP_CONFIG,
    IP_IND,
    IP_GPRSACT,
    IP_STATUS,
    TCP_CONNECTING,
    IP_CLOSE,
    PDP_DEACT
}CONNECT_STATE;

typedef enum {
    NONE_RCV = 0,
    RCV_OK,
    RCV_FAIL,
}RCV_STATE;

typedef enum {
    NONE_SEND = 0,
    SEND_OK,
    SEND_FAIL,
    TIMEOUT
}SEND_STATE;

typedef struct
{
    RCV_STATE  enRcvState;
    _UCHAR8    pData[1024];
    _INT32     pLen;
    _INT32     DataLen;
    _INT32     TimeOut;
    _BOOL      bQiRcvLock;
    _BOOL      bSemstate;        
}RCVDATA;

typedef struct
{
    CONNECT_STATE    enConnectState;
    CONNECT_STATE    bQiStatStatus;
    SEND_STATE       enSendState;
    _BOOL            bQiOpenStart; 
    _BOOL            bQiSackLock;
    _BOOL            bSemstate;
    _BOOL            bOpenReuse;
    _UCHAR8          pDstIp[128];
    _UCHAR8          pOldDstIp[128];
    _UINT32          DstPort;
    _UCHAR8*         pDstAddr;
    _UCHAR8          pData[512];
    _INT32           DataLen;
    _INT32           TimeOut;
}SENDDATA;

typedef struct wirelessrootst
{
    _INT32           iHardwareVer;
    _INT32           iSIMVer;
    _INT32           iMODVer;
    _INT32           iPCBVer;
    
    EOS_SEM_ID       mutex;
    
    EOS_SEM_ID       ATCmdLstMutex;        
    EOS_SEM_ID       RSPLstMutex;
    
    /*    Timer    */
    EOS_TIMER_ID     WirelessTimerID;
    /* 2018.08.01 added by pengjian */
    utl_select       sel;           /* select used to send/receive socket message */
            
    _UINT32          uiTickWatchDogTriger;

    _UINT32          uiMaxChannelNum;
    _UINT32          uiMaxModuleNum;           //for android,uiMaxModuleNum = uiMaxChannelNum - 1.
    _UINT32          uiStartChannel;
    _UINT32          uiEndChannel;
      /* for statistics   */
    _BOOL            bWirelessTaskRun;        
    _BOOL            bWirelessTaskRun1;        
    _BOOL            bWirelessTaskRun2;        
    _BOOL            bWirelessTaskRun3;        
    _BOOL            bWirelessTaskRun4;        
    _BOOL            bWirelessTaskRun5;        
    _BOOL            bWirelessTaskRun6;        
    _BOOL            bWirelessTaskRun7;        
    _BOOL            bWirelessTaskRun8;        
    _BOOL            bWirelessTaskRun9;      
    _BOOL            bWirelessTaskRun10;      
    _BOOL            bWirelessTaskRun11;      
    _BOOL            bWirelessTaskRun12;      
    _UINT32          uiWirelessMaxTick;
    _UINT32          uiMaxRcvCmdTick;
    _UINT32          uiMaxRcvCmdType;
    _UINT32          uiMaxComRcvTick;

    _UINT32          uiWirelessMaxTick1;
    _UINT32          uiWirelessMaxTick2;
    _UINT32          uiWirelessMaxTick3;
    _UINT32          uiWirelessMaxTick4;

    //_BOOL          bFxoInputRunAuto;   
    _BOOL            bFxoOutputRunAuto;
    _BOOL            bCIDEnable;
    
    _UINT32          uiFxoOutputAATicks;

    _BOOL            bHideNumber;
    _BOOL            bEbsEnable;
    _BOOL            bBillingEnable;
    _UINT32          ebsAccountType;
    
    _VOID*           hWireless;

    _BOOL            bNeedSaveNv;
    ST_UT_MSGQ*      FSMMsgq;
    ST_UT_MSGQ*      ATRspMsgq;
    
    SEM_ID           semATSendData;
    SEM_ID           semATRcvData;
    RCVDATA          rcvdata;
    SENDDATA         senddata;
    
    /*    channel    */
    PWIRELESSCHANNELST    stChannelSt;

    /* for AT debug mode    */
    ST_UT_MSGQ*      remMsgq;
    ST_UT_MSGQ*      locMsgq;
    _BOOL            bIsATDbgMode;
    _UINT32          uiDbgChannel;
    _UINT32          regetlocTimes;
    _UINT32          IsNewCard;
    /*    wireless func ptr    */
    _INT32          (*wirelessInitFunPtr)(_LONG32 param);
    _VOID           (*wirelessMainTask)(_VOID);
    _VOID           (*wirelessStat)(EOS_INFO_PRINTF fpPrint, _VOID* pData);
        
}WIRELESSROOTST,*PWIRELESSROOTST;

typedef struct
{
    T_ECALL_OPERATION    ID;
    _USHORT16            index;
}T_CALLCMD;

typedef _UINT32 (*CALLMANAGERCALLBACK)(_UINT32 ChannelId, HCALLMANAGER hCallManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

typedef struct tagcallparam{
    HLINE          hLine;    /*发起请求的line的句柄*/
    HCALL          hCall;  /*该请求涉及到的call句柄*/
    _UINT32        hATServer;
    _UINT32        requestOwner; /*该请求是line的请求还是call manager的请求; 1: line的请求; 0: call manager的请求*/
    _UINT32        customParam; /*如果请求是line的请求,记录下line的custom Parameter信息*/
    CALLMANAGERCALLBACK    pfCallback;         /*请求结果的回调函数*/
    _UINT32        event;
    T_CALLCMD      cmd[8];        /*Store all command the need for one request*/
    _USHORT16      cmdCount;    /*total command number.*/
    _USHORT16      cmdIndex;    /*current sending commend index for the command array.*/
}T_CALLPARAM;

typedef struct tagmsgmanager{
    T_MSGMNGSTATE    state;
}T_MSGMANAGER;

typedef _UINT32 (*MSGMANAGERCALLBACK)(HMSGMANAGER hMSGManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

typedef struct tagmsgparam{
    HLINE          hLine;    /*发起请求的line的句柄*/
    _UINT32        hATServer;
    _UINT32        requestOwner; /*该请求是line的请求还是call manager的请求; 1: line的请求; 0: call manager的请求*/
    _UINT32        customParam; /*如果请求是line的请求,记录下line的custom Parameter信息*/
    T_STR_PDU      pdu;
    T_STR_20       PduLen;
    _INT32         iPduLen;

    _UINT32        event;
    _VOID*         lpData;
    _UINT32        totalSize;
    _UINT32        usedSize;
    _UINT32        RetryTimes;
        
    MSGMANAGERCALLBACK    pfCallback;         /*请求结果的回调函数*/
}T_MSGPARAM;

typedef _UINT32 (*SIMMANAGERCALLBACK)(_UINT32 hManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);

typedef struct tagsimparam{
    HLINE          hLine;    /*发起请求的line的句柄*/
    _UINT32        hATServer;
    _UINT32        requestOwner; /*该请求是line的请求还是STK manager的请求; 1: line的请求; 0: manager的请求*/
    _UINT32        customParam; /*如果请求是line的请求,记录下line的custom Parameter信息*/
    _UINT32        info;         /*附加信息*/
    SIMMANAGERCALLBACK    pfCallback;         /*请求结果的回调函数*/
}T_SIMPARAM;


_UINT32 CallManager_DeInit_Ex(HCALLMANAGER hCallManager); /* Not Free hCallManager    */
                                  
_VOID WIRELESS_SetModInitStatus(_UINT32 ChannelID, RIL_INIT_STATUS initStatus);
_VOID WIRELESS_SetRegStatus(_UINT32 ChannelID, RIL_NET_STAT regStatus);
_VOID WIRELESS_SetSlotSIMIsNew(_UINT32 ChannelID, _UINT32 bSimIsNew, _BOOL bWakeup);
_VOID WIRELESS_SetFakeResponse(_UINT32 ChannelID, _BOOL bFakeResponse);

#endif

