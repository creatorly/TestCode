
#ifndef WM_STRUCT_H
#define WM_STRUCT_H

#include "gbl_macrodef.h"

#define NEIG_ITEM_NUM            7        /**< parse neighbour cell para num*/ 
#define SERV_ITEM_NUM            8       /**< parse server cell para num*/ 
#define NEIG_CELL_NUM            6       /**< neighbour cell number*/ 
#define MAX_CALL_NUM            8        /**< maximum parts for conference call*/ 
#define    MAX_CLASS_TYPE_NUM        10        /**< maximum number of class type*/
#define MAX_NET_NUM                 300        /**< maximum network number that anyka need */ 
#define ALL_NET_NUM                 800        /**< all network operator names number*/  
#define MAX_TELE_LEN            50        /**< maximum telephone number length*/
#define TELNUM_TYPE_LEN            3
#define MAX_NETNAME_LEN            50        /**< maximum network name length*/
#define MAX_BSINFO_LEN            128        
#define MAX_PDU_LEN               480            /**< maximum length of SMS PDU*/
#define MAX_PDU_LEN_CDMA        480            /**< maximum length of CDMA SMS PDU*/
#define INITIALYEAR                2000
#define MAX_NAME_LEN            16        /**< maximum name length*/
#define MAX_NETID_LEN           16        /**< maximum network ID length*/
#define MAX_VOLUME_LEVEL        7
#define MAX_NETCELL_LEN            8      /**< maximum cell data length*/
#define MAX_LAC_LEN                4
#define MAX_CID_LEN                4
#define MAX_CENADDRESS_LEN        20
#define MAX_SMSSTORAGE_LEN        3

#define MAX_MENU_ITEM            64
#define MAX_FILE_ITEM            128
#define MIN_STK_STRING            16
#define SMALL_STK_STRING        32
#define MIDDLE_STK_STRING        64
#define LARGE_STK_STRING        128
#define MAX_STK_STRING            255
#define MAX_STK_DATA            2048


typedef _CHAR8    T_STR_PDU[MAX_PDU_LEN+1];            /**< type definition for SMS PDU string*/
typedef _CHAR8    T_STR_NETID[MAX_NETID_LEN+1];        /**< type definition for net ID string */
typedef _CHAR8    T_STR_NETNAME[MAX_NETNAME_LEN+1];    /**< type definition for net name string */
typedef _CHAR8    T_STR_CELLLEN[MAX_NETCELL_LEN+1];    /**< type definition for parse cell*/
typedef _CHAR8    T_STR_NUMBER[64];            /**< type definition for SMS PDU string*/

typedef _CHAR8    T_STR_BSINFO[MAX_BSINFO_LEN+1];    /**< type definition for net name string */

typedef enum {
    eRIL_CME_ERROR = 0,
    eRIL_CMS_ERROR,
    eRIL_EXT_ERROR,
    eRIL_EXT_SS_ERROR,
    eRIL_EMS_DEFINE
} RIL_eERR_TYPE;

typedef struct {
    RIL_eERR_TYPE    repType;
    _SHORT16        repID;        /** if repType is etEXTENDED_ERROR, repID instead of location ID */
    _SHORT16        reason;        /** if repType isn't etEXTENDED_ERROR, reason is reserved */
} RIL_ERR_REPORT;
/** error type enum*/
typedef enum {
    eRIL_MODULE_TIME_OUT,
    eRIL_FAILURE
}RIL_ANYKA_ERR_ID;
/** PIN status definition*/
typedef enum {
    RIL_READY = 0,
    RIL_SIM_PIN, RIL_SIM_PUK,
    RIL_SIM_PIN2, RIL_SIM_PUK2, 
    RIL_PH_SIM_PIN, RIL_PH_SIM_PUK, 
    RIL_PH_FSIM_PIN, RIL_PH_FSIM_PUK,
    RIL_PH_NET_PIN, RIL_PH_NET_PUK,
    RIL_PH_NS_PIN, RIL_PH_NS_PUK,
    RIL_PH_SP_PIN, RIL_PH_SP_PUK,
    RIL_PH_C_PIN, RIL_PH_C_PUK,

    RIL_MAX_CPIN_STATUS_NUM
} RIL_CPIN_STATUS;

typedef enum {
    RIL_INITSTART = 0,
    RIL_AT_READY,
    RIL_PB_READY, 
    RIL_SMS_READY, 

    RIL_MAX_INIT_STATUS_NUM
} RIL_INIT_STATUS;


/** Pin PIN2 PUK PUK2 remain counts*/
typedef enum {
    PIN_REMAIN_COUNT = 0,
    PIN2_REMAIN_COUNT,
    PUK_REMAIN_COUNT, 
    PUK2_REMAIN_COUNT
} PIN_SECURITY_COUNT;

/** The data types definition*/
typedef enum {
    RIL_dcDEFAULT=-1,
    RIL_dcNULL   = 0,
    RIL_dcVOICE  = 1,
    RIL_dcDATA   = 2,
    RIL_dcFAX    = 4,
    RIL_dcSMS     = 8,
    RIL_dcDCS     = 16,
    RIL_dcDCA     = 32,
    RIL_dcDPA    = 64,
    RIL_dcDPADA  = 128
} RIL_DATA_CLASS;

/** system time structure */
typedef struct {
    _USHORT16   year;           /* 4 byte: 1-9999 */
    _UCHAR8    month;          /* 1-12 */
    _UCHAR8    day;            /* 1-31 */
    _UCHAR8    hour;           /* 0-23 */
    _UCHAR8    minute;         /* 0-59 */
    _UCHAR8    second;         /* 0-59 */
    _USHORT16   milli_second;   /* 0-999 */     
    _UCHAR8    week;           /* 0-6,  0: monday, 6: sunday*/
} RIL_SYSTIME;

/** Alarm struct definition*/
typedef struct
{
    RIL_SYSTIME    time;
    _UCHAR8        repeat; /**0: none; 1: daily; 2: weekly; 3: monthly*/
    _UCHAR8        power; /**0: None; 1: alarm power off; 2: alarm power on*/
    _UCHAR8        valid; /**0: invalid.*/
    _UCHAR8        id;        /**0-3, when delete the id of alarm set*/
} RIL_CALA_ALARM;

/** FAC definition*/
typedef enum {
    RIL_SC = 0,        /**<lock SIM card*/
    RIL_PS,            /**<lock phone to SIM card*/
    RIL_FD,            /**<fixed dialing*/
    RIL_PF,            /**<*/
    RIL_PN,            /**<*/
    RIL_PU,            /**<*/
    RIL_PP,            /**<*/
    RIL_PC,            /**<*/
    RIL_P2,            /**<PIN 2*/
    RIL_AO,            /**<bar all outgoing calls*/
    RIL_OI,            /**<bar outgoing international calls*/
    RIL_OX,            /**<bar outgoing international calls except to home country*/
    RIL_AI,            /**<bar all incoming calls*/
    RIL_IR,            /**<bar incoming calls when roaming outside the home country*/
    RIL_AB,            /**<all barring services*/
    RIL_AG,            /**<all outgoing barring service*/
    RIL_AC,            /**<all incoming barring service*/
    RIL_MAX_ATD_FAC_NUM
} RIL_FACILITY;

/**clck mode definition*/
typedef enum {
    RIL_CLCK_Unlock = 0,
    RIL_CLCK_Lock,
    RIL_CLCK_Query
} RIL_CLCK_MODE;

/** Lock struct definition*/
typedef struct {
    RIL_FACILITY    Fac;            /** only used in ^SCLCK */
    RIL_CLCK_MODE    mode;
    _UCHAR8            Class;            /** class of lock */
} RIL_CLCK_LOCK;

/** CREG parse definition*/
typedef enum {
    RIL_NOTREGIST = 0,  /**no network*/
    RIL_REGISTHOME,
    RIL_NOTREGSEARCH,
    RIL_REGISTDENY,
    RIL_UNKNOWNNET,    /* for mc323, this is roam    */
    RIL_REGISROAM,
    RIL_MAX_NET_STAT_NUM
} RIL_NET_STAT;

typedef struct {
    RIL_NET_STAT    netstat;
    _UINT32 lac;         
    _UCHAR8 cellid[12];         
 } RIL_NET_STAT_WITH_CID;


/** parse result */
typedef enum {
    RIL_prOK = 0,
    RIL_prCONNECT,
    RIL_prNOCARRIER,
    RIL_prERROR,
    RIL_prCMEERROR,
    RIL_prCMSERROR,
    RIL_prGERROR,
    RIL_prWMRESET,
    RIL_prNULL,
    RIL_prUNKNOWN,
    RIL_prUNKNOWN_WM,
    RIL_prTIMEOUT
} RIL_PARSE_RES;    

/** MODULE_STATE*/        
typedef enum
{
    RIL_MODULE_NORMAL = 0,        /** 正常开机*/    
    RIL_MODULE_GHOST,            /** 关机充电：表示用户在关机的情况下，插入充电器，此时只是显示充电动画通过AT查询电池电量，其他的操作是建议进行的，尤其是查询时间在此时是不能进行的！！！*/    
    RIL_MODULE_LOWPOWER,        /** 低电压开机：电池电量低，应该立刻关机或者提示用户电量低；*/    
    RIL_MODULE_POWERDOWN,        /** 正常关机*/    
    RIL_MODULE_CHARGEPOWERDOWN, /** 在充电的情况下关机：表示手机在正常工作的情况下，进入充电模式，显示充电动画；*/    
    RIL_MODULE_NORMALCHARGE,    /** 开机的情况下充电*/    
    RIL_MODULE_ALARM,            /** 闹钟开机*/    
    RIL_MODULE_GHOSTALARM,        /** 在充电的情况下闹钟关机：系统由充电转到开机*/    
    RIL_MODULE_INVALID            /** 无效状态*/    
} RIL_MODULE_STATE;

/** Battery Status*/
typedef enum {
    RIL_NOCHARGE = 0,
    RIL_CHARGING,
    RIL_CHARGEFULL
} RIL_BatteryStatus;

/** SIM phone book space definition*/
typedef struct
{
    _USHORT16    MaxID;                     /**< max location ID of the SIM card */
    _USHORT16    MinID;                    /**< min location ID of the SIM card */
    _UCHAR8    NameLength;                /**< limit length of name */
    _UCHAR8    TeleLength;                /**< limit length of telephone number */
} RIL_SIMPB_SPACE;

typedef struct
{
    _UCHAR8    strType[3];
    _SHORT16    UsedNnm;            /** has used num of sim */
    _SHORT16    MaxNum;                /**  maxinum of sim */
} RIL_SIMPB_MEMORY;    

/** telephone type definition*/
typedef enum {
    RIL_TYPENULL = -1,
    RIL_INTER,
    RIL_OTHERS
} RIL_TELE_TYPE;

/** Call type definition*/
typedef enum {
    RIL_MOCALL,
    RIL_MTCALL
} RIL_TELE_ORIGNATE;

/** Call status definition*/
typedef enum {
    RIL_ACTIVE = 0,
    RIL_HOLD,
    RIL_DIALING,
    RIL_ALERTING,
    RIL_INCOMING,
    RIL_WAITING,
    RIL_CALL_STATUS_NUM
} RIL_CALL_STATUS;

typedef enum {
    MODULE_READY = 0,
    MODULE_UNKNOWN = 2,
    MODULE_RING = 3,
    MODULE_CALLING = 4,
    MODULE_CALL_STATUS_NUM
} MOD_CALL_STATUS;

/** Call mode definition*/
typedef enum {
    RIL_VOICECALL,
    RIL_DATACALL,
    RIL_FAXCALL,
    RIL_UNKNOWN
} RIL_CALL_MODE;

/** Call party definition*/
typedef enum {
    RIL_NINPARTY,
    RIL_INPARTY
} RIL_CALL_PARTY;

/** Current call list definition*/
typedef struct {
    _UINT32                hCallHandle; /**< Call handle to this call */
    _SHORT16                idx;        /**< index of the entry for CHLD from 0 */
    RIL_TELE_ORIGNATE    dir;        /**< 0. MO call 1.MT call */
    RIL_CALL_STATUS        stat;        /**< 0. active 1.hold 2. dialing (MO call) */
                                    /**< 3. alerting (MO call) 4.incoming (MT call) 5. waiting (MT call) */
    RIL_CALL_MODE        mode;        /**< 0. voice call 1. data call 2. fax call 9. unknown */
    RIL_CALL_PARTY        mpty;        /**< 0. call not in call parties 1. call in call parties */
    T_STR_TELE            number;        /**< call number */
    RIL_TELE_TYPE        type;        /**< 145 international 129 others */
} RIL_CLCC_ENTRY;

/** GSM MODE */            
typedef enum{
    RIL_MODE_DISENABLE,    
    RIL_MODE_ENABLE,    
    RIL_MODE_INQUERY,
    RIL_MODE_REGISTER,
    RIL_MODE_ERASE,
    RIL_MODE_INVALID_MODE
}RIL_GSM_MODE;

/** Call forward reason */
typedef enum{
    RIL_CCFC_ANY = 0,
    RIL_CCFC_BUSY,
    RIL_CCFC_NOREPLY,
    RIL_CCFC_NOREACH,
    RIL_CCFC_ALLCALL,
    RIL_CCFC_ALLCOND
}RIL_CCFC_REAS;

/** QUERY status definition*/
typedef enum {
    RIL_STATUS_NACTIVE = 0,
    RIL_STATUS_ACTIVE
} RIL_QUERY_STATUS;

/** for forward structure */
typedef struct
{
    RIL_CCFC_REAS    Reason;
    RIL_GSM_MODE    Mode;
    RIL_DATA_CLASS    Class;
    T_STR_TELE        Number;
    RIL_TELE_TYPE    Type;
    RIL_QUERY_STATUS Status;
    _SHORT16            Time;
} RIL_CCFC_FWDCALLS;

/** CCWA definition*/
typedef struct {
    RIL_DATA_CLASS        Class;
    RIL_QUERY_STATUS    status;
    T_STR_TELE            telenumber;
} RIL_CCWA_LIST;

/** call progress*/
typedef struct {
    _SHORT16 cin;
    _SHORT16 status;    
} RIL_CPROGRESS;

/** CPMS definition*/
typedef struct {
    _SHORT16 usedx;
    _SHORT16 totalx;    
} RIL_CPMS;

/** SMS mode definition*/
typedef enum {
    RIL_RECUNREAD,
    RIL_RECREAD,
    RIL_STOUNSENT,
    RIL_STOSENT,
    RIL_ALL
} RIL_SMS_MODE;

/** SMS entry definition*/
typedef struct {
    _SHORT16        idx;        /**< SMS index in the SMS storage */
    RIL_SMS_MODE    stat;    
    _CHAR8             SCA[32];            
    _CHAR8             TPA[32];            
    _CHAR8             TP_DCS;            
    _CHAR8             TP_SCTS[32];    
    
    T_STR_NAME    name;
    _USHORT16        len;
    T_STR_PDU  strPDU;
} RIL_SMS_ENTRY;

/** SMS center address definition*/
typedef struct {
    T_STR_TELE        sca;
    RIL_TELE_TYPE    tosca;
} RIL_SMS_CENADDRESS;

/** COPN name definition*/
typedef struct {
    T_STR_NETID        numericn;    /* operator in numeric form    */
    T_STR_NETNAME    alphan;        /* operator in alphanumeric form */
} RIL_COPN_NAME;

/** URC status definition*/
typedef enum {
    RIL_DISAURC,
    RIL_ENABURCNOT,
    RIL_ENABURCREPORT
} RIL_URC_STAT;

/** Network mode definition*/
typedef enum {
    RIL_AUTO,
    RIL_MANU,
    RIL_MANUDEREGISTER,
    RIL_SETFORMAT,
    RIL_SETDIGITAL,
    RIL_COMBIN
} RIL_NET_MODE;

/** COPS current name definition*/
typedef struct {
    RIL_NET_MODE        mode;                    /* 0. Automatic  1.manual  2. manual deregister */
                                            /* 3. for setting format   4. combination of manual and automatic */
    _SHORT16            format;                    /* 0. alphanumeric format  2. numeric */
    _SHORT16           netattr;                  /*0. GSM, 1 GSM compact, 2 UTRAN*/
    T_STR_NETNAME    operat;                    /* operator, attention: Although there are two kinds of styles for the operator, */
                                            /*  numeric and long format according the ''format'', here we suppose all show in long alphanumeric mode, please sending "at+cops=3,2" to set the format before using */
} RIL_COPS_CURRENTNAME;

/** Base Station Info definition*/
typedef struct {
    _BOOL            bGetNegborBSInfo;
    _UINT32            CurrBSId;
    T_STR_BSINFO    BSInfo;
} RIL_BS_INFO;

/** Network status definition*/
typedef enum {
    RIL_UNKNOWNSTAT = 0,
    RIL_AVALIABLE,
    RIL_CURRENREGIST,
    RIL_FORBIDDEN
} RIL_NETWORK_STAT;

/** COPS network definition*/
typedef struct {
    RIL_NETWORK_STAT    stat;        /** operator status */
    T_STR_NETID    operatn;            /**  operator name as numerica mode */
    T_STR_NETNAME    operata;        /** operator name as alphanumerica mode */
} RIL_COPS_NETWORK;

typedef struct {
    _UINT32        mode;
    T_STR_200    strID;
    T_STR_20    strDCS;
} RIL_CBS_SETTING;

/** SIM phone book storage definition*/
typedef enum {
    RIL_sSIM = 0,                /**< SIM PhoneBook */
    RIL_sME,                    /**< ME PhoneBook */
    RIL_sFD,                    /**< SIM fxidialing PhoneBook */
    RIL_sLD,                    /**< Last Dialing PhoneBook */
    RIL_sMC,                    /**< List of Missed calls */
    RIL_sRC,                    /**< List of Recieved calls */
    RIL_sON,                    /**< Own Number */
    RIL_sDC,                    /**< ME dialed calls list */
    RIL_sLA,                    /**< Last Number All list */
    RIL_sBN,                    /**< SIM barred dialled number */
    RIL_sSD,                    /**< SIM service mailbox */
    RIL_sVM                     /**< SIM voice mailbox */
} RIL_PB_STORAGE;

/** Message storage definition*/
typedef enum {
    RIL_MSG_MEM_SM = 0,                /**< SIM message storage */
    RIL_MSG_MEM_ME,                    /**< ME message storage */
    RIL_MSG_MEM_MT,                    /**< MT=ME+SIM, SIM preferred */
    RIL_MSG_MEM_BM                    /**< broadcast message storage */
} RIL_MSG_STORAGE;

/** SIM phone book entry definition*/
typedef struct 
{
    _USHORT16        index;            /**< The index of this entry in the storage */
    T_WSTR_NAME    Name;            /**< The name of the entry, In unicode format */
    T_STR_TELE    Tele;
}RIL_PB_SIM_ENTRY;

/** query battery level  definition*/
typedef struct
{
    _SHORT16        battery;
    _SHORT16        level;
}RIL_BAT_LEVEL;

/** STK command definition*/
typedef enum{
    RIL_STK_Invalid                =    00,
    RIL_STK_Refresh                =    01,
    RIL_STK_SetupEventList        =    05,
    RIL_STK_SetupCall            =    16,
    RIL_STK_SendSS                =    17,
    RIL_STK_SendUSSD            =    18,
    RIL_STK_SendSM                =    19,
    RIL_STK_SendDTMF            =    20,
    RIL_STK_LaunchBrowser        =    21,
    RIL_STK_PlayTone            =    32,
    RIL_STK_DisplayText            =    33,
    RIL_STK_GetInkey            =    34,
    RIL_STK_GetInput            =    35,
    RIL_STK_SelectItem            =    36,        /** <entry main item */
    RIL_STK_SetupMenu            =    37,
    RIL_STK_LanguageSetting        =    38,
    RIL_STK_SetupIdleModeText    =    40,
    RIL_STK_LanguageNotification=    53,
/*extern*/
    RIL_STK_MenuSelection        =    99,        /** <main menu selection*/
    RIL_STK_EndProactive        =    81,
    RIL_STK_STTONE                =    111,
    RIL_STK_RunAT                =    55,
    RIL_STK_OpenChannel            =    44,
    RIL_STK_CloseChannel        =    41,
    RIL_STK_ReceiveData            =    42,
    RIL_STK_SendData            =    43,
}RIL_STK_CMD;

/** STK response mode definition*/
typedef enum{
    RIL_SIMInitAndFullFileChange = 00,
    RIL_FileChange,
    RIL_SIMInitAndFileChange,
    RIL_SIMInit,
    RIL_SIMReset
}RIL_STK_REFRESHMODE;

/** STK data coding scheme  definition*/
typedef enum{
    RIL_GSM7bit = 0,
    RIL_Data8bit = 4,
    RIL_UCS2 = 8
}RIL_STK_ALPHABET;

/** STK priority definition*/
typedef enum{
    RIL_NormalPriority = 0,
    RIL_HighPriority
}RIL_STK_PRIORITY;

/** STK clear definition*/
typedef enum{
    RIL_ClearAfterDelay = 0,
    RIL_UserClearsMessage
}RIL_STK_CLEAR;

/** STK display text definition*/
typedef enum{
    RIL_DisplayIconOnly = 0,
    RIL_DisplayWithOther
}RIL_STK_DISPLAY;

/** STK display text response definition*/
typedef enum{
    RIL_NormalReponse = 0,
    RIL_ImmediateResponse
}RIL_STK_DISPLAYTEXTRESPONSE;

/** STK inkey response definition*/
typedef enum{
    RIL_ikDigits = 0 ,
    RIL_ikSMS ,
    RIL_ikUCS2 , 
    RIL_ikYesNo
}RIL_STK_INKEYRESPONSE;

/** STK input response definition*/
typedef enum{
    RIL_DigitsFromSMSUnpacked = 1,
    RIL_DigitsFromSMSpacked = 2 , 
    RIL_DigitsFromUCS2 = 3 ,
    RIL_SMSUnpacked = 4 ,
    RIL_SMSPacked = 5 , 
    RIL_ipUCS2 = 6,
    RIL_SMSUnpackedUCS2 = 7 ,
    RIL_SMSPackedUCS2 = 8 
}RIL_STK_INPUTRESPONSE;

/** STK echo definition*/
typedef enum{
    RIL_ECHO = 0 ,
    RIL_NoECHO
}RIL_STK_ECHO;

/** STK help information definition*/
typedef enum{
    RIL_HelpInfo = 0,
    RIL_NoHelpInfo
}RIL_STK_HELPINFO;

/** STK tone definition*/
typedef enum{
    RIL_Dial = 1,
    RIL_CalledSubscriberBusy = 2,
    RIL_Congestion = 3,
    RIL_RadioPathAcknowledge = 4,
    RIL_CallDropped = 5,
    RIL_Error = 6,
    RIL_CallWaiting = 7,
    RIL_RingingTone = 8,
    RIL_GeneralBeep = 16,
    RIL_PositiveAck = 17,
    RIL_NegativeAck = 18
}RIL_STK_TONE;

/** STK menu selection definition*/
typedef enum{
    RIL_DisableSelection = 0,
    RIL_SoftKeySelection = 1
}RIL_STK_MENUSELECTION;

/** STK menu remove definition*/
typedef enum{
    RIL_DoNotRemove = 0,
    RIL_RemoveCurrent = 1
}RIL_STK_MENUREMOVE;

/** STK Launch browser definition*/
typedef enum{
    RIL_NotMakeConnection = 0,
    RIL_MakeConnection = 1,
    RIL_UseExisting = 2,
    RIL_LaunchNew = 3,
    RIL_LaunchNewUseSecureSession = 4
}RIL_STK_COMQUAL;

/** STK bearer definition*/
typedef enum{
    RIL_bSMS = 0,
    RIL_bCSD,
    RIL_bUSSD,
    RIL_bGPRS
}RIL_STK_BEARER;

/** STK event list definition*/
typedef enum{
    RIL_UserActivity = 0,
    RIL_IdleScreenAvailable = 6,
    RIL_LanguageSelection = 8,
    RIL_BrowserTermination = 9,
    RIL_RemoveExisting = 0xff
}RIL_STK_EVENTLIST;

/*--------------- response code ------------------*/

/** STK display text result definition*/
typedef enum{
    RIL_DisplayTextOK = 0,
    RIL_TerminateProactive,
    RIL_UserClearedMessage,
    RIL_ScreenIsBusy,
    RIL_BackwardMove,
    RIL_NoResponseFromUser,
    RIL_CallIsBusy
}RIL_STK_DISPLAYTEXT_RES;

/** STK get inkey result definition*/
typedef enum{
    RIL_GetInkeyOK = 0,
    RIL_ikTerminateProactive,
    RIL_ikHelpInformation,
    RIL_ikBackwardMove,
    RIL_ikNoResponseFromUser
}RIL_STK_GETINKEY_RES;

/** STK get input result definition*/
typedef enum{
    RIL_GetInputOK = 0,
    RIL_ipTerminateProactive,
    RIL_ipHelpInformation,
    RIL_ipBackwardMove,
    RIL_ipNoResponseFromUser
}RIL_STK_GETINPUT_RES;

/** STK play tone result definition*/
typedef enum{
    RIL_PlayToneOK = 0,
    RIL_ptTerminateProactive,
    RIL_ToneNotPlayed,
    RIL_ToneNotSupported
}RIL_STK_PLAYTONE_RES;

/** STK setup menu result definition*/
typedef enum{
    RIL_SetupMenuOK = 0,
    RIL_ChosenItem,
    RIL_HelpInformation,
    RIL_Problem
}RIL_STK_SETUPMENU_RES;

/** STK select item result definition*/
typedef enum{
    RIL_SelectItemOK = 0,
    RIL_siTerminateProactive,
    RIL_siHelpInformation,
    RIL_siBackwardMove,
    RIL_NoResponse
}RIL_STK_SELECTITEM_RES;

/** STK get setup-call result definition*/
typedef enum{
    RIL_AcceptedCall = 0,
    RIL_RejectedCall,
    RIL_ClearedCall
}RIL_STK_SETUPCALL;

/** STK setup idle mode text result definition*/
typedef enum{
    RIL_SetupIdleModeTextOK = 0,
    RIL_SetupIdleModeTextProblem
}RIL_STK_SETUPIDLEMODETEXT_RES;

/** STK send DTMF result definition*/
typedef enum{
    RIL_sNotAccepted = 0,
    RIL_sRequired
}RIL_STK_SENDDTMF_RES;

/** STK launch browser result definition*/
typedef enum{
    RIL_Successfully = 0,
    RIL_PartialComp,
    RIL_MissingInfo,
    RIL_UserRejected,
    RIL_LaunchError,
    RIL_BearerUnavailable,
    RIL_BrowserUnavailable,
    RIL_MECannotProcess,
    RIL_NetworkCannotProcess,
    RIL_BeyondMEsCapabilities
}RIL_STK_LAUNCHBROWSER_RES;

/** STK open channel result definition*/
typedef enum{
    RIL_NotAccepted = 0,
    RIL_Required
}RIL_STK_OPENCHANNEL_RES;

/** STK setup event list result definition*/
typedef enum{
    RIL_PerformOK = 0,
    RIL_CannotPerform
}RIL_STK_SETUPEVENTLIST_RES;

/*---------------- for +STUD -------------------*/

/** STUD refresh definition*/
typedef struct{
    RIL_STK_REFRESHMODE refMode;
    _UCHAR8 numFiles;
    _CHAR8 fileList[MAX_FILE_ITEM][LARGE_STK_STRING];
}RIL_STUD_REFRESH;

/** STUD send ss definition*/
typedef struct{
    _USHORT16             alphaId[MAX_STK_DATA];
    _UCHAR8             iconId;
    RIL_STK_DISPLAY     dispMode;
}RIL_STUD_SENDSS;

/** STUD send ussd definition*/
typedef struct{
    _UCHAR8 dcs;
    _USHORT16 text[MAX_STK_STRING];
    _USHORT16 alphaId[MAX_STK_DATA];
    _UCHAR8 iconId;
    _UCHAR8 refNum;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_SENDUSSD;

/** STUD send sm definition*/
typedef struct{
    _USHORT16 alphaId[MAX_STK_DATA];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_SENDSM;

/** STUD setup call definition*/
typedef struct{
    _USHORT16 alphaId[LARGE_STK_STRING];
    _CHAR8 dialstring[LARGE_STK_STRING];
    _CHAR8 cps[LARGE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_SETUPCALL;

/** STUD run AT definition*/
typedef struct{
    _CHAR8 alphaId[MAX_STK_DATA];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_RUNAT;

/** STUD language notification definition*/
typedef struct{
    _UINT32 languageCode;/* at ISO 639 [12] */
}RIL_STUD_LANGUAGENOTIFICATION;

/** STUD close channel definition*/
typedef struct{
    _CHAR8 alphaId[MAX_STK_DATA];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_CLOSECHANNEL;

/** STUD receive definition*/
typedef struct{
    _UINT32 length;
    _CHAR8 alphaId[MAX_STK_DATA];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_RECEIVE;

/** STUD send definition*/
typedef struct{
    _UINT32 length;
    _CHAR8 data[MAX_STK_DATA];
    _CHAR8 alphaId[LARGE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STUD_SEND;

/*-------------- for STGC ----------------*/

/** STGC display text definition*/
typedef struct{
    RIL_STK_ALPHABET dcs;
    _USHORT16 text[MAX_STK_STRING];
    RIL_STK_PRIORITY priority;
    RIL_STK_CLEAR clear;
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
    RIL_STK_DISPLAYTEXTRESPONSE response;
}RIL_STGC_DISPLAYTEXT;

/** STGC get inkey definition*/
typedef struct{
    RIL_STK_ALPHABET dcs;
    _USHORT16 text[MAX_STK_STRING];
    RIL_STK_INKEYRESPONSE response;
    RIL_STK_HELPINFO helpInfo;
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_GETINKEY;

/** STGC getinput definition*/
typedef struct{
    RIL_STK_ALPHABET dcs;
    _USHORT16 text[MAX_STK_STRING];
    RIL_STK_INPUTRESPONSE response;
    RIL_STK_ECHO echo;
    RIL_STK_HELPINFO helpInfo;
    _UCHAR8 minLength;
    _UCHAR8 maxLength;
    RIL_STK_ALPHABET defaultDcs;
    _USHORT16 defaultText[MAX_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_GETINPUT;

/** STGC play tone definition*/
typedef struct{
    _USHORT16 alphaId[MAX_STK_STRING];
    RIL_STK_TONE tone;
    _UINT32 duration;
}RIL_STGC_PLAYTONE;

/** STGC menu item definition*/
typedef struct{
    _UCHAR8 itemId;
    _USHORT16 itemText[MIDDLE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
    _UCHAR8 nai;
}RIL_STGC_MENUITEM;

/** STGC setup menu definition*/
typedef struct{
    _UCHAR8 numItems;
    RIL_STK_MENUSELECTION selection;
    RIL_STK_HELPINFO helpInfo;
    RIL_STK_MENUREMOVE removeMenu;
    _USHORT16 alphaId[MAX_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
    RIL_STGC_MENUITEM items[MAX_MENU_ITEM];
}RIL_STGC_SETUPMENU;

/** STGC select item definition*/
typedef struct{
    _UCHAR8 numItems;
    RIL_STK_MENUSELECTION selection;
    RIL_STK_HELPINFO helpInfo;
    _USHORT16 alphaId[MAX_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
    RIL_STGC_MENUITEM items[MAX_MENU_ITEM];
}RIL_STGC_SELECTITEM;

/** STGC get acknowledgement definition*/
typedef struct{
    _CHAR8 alphaId[LARGE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_GETACKNOWLEDGEMENT;

/** STGC setup idle mode text definition*/
typedef struct{
    RIL_STK_ALPHABET dcs;
    _USHORT16 text[MAX_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_SETUPIDLEMODETEXT;

/** STGC send DTMF definition*/
typedef struct{
    _USHORT16 alphaId[LARGE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_SENDDTMF;

/** STGC launch browser definition*/
typedef struct{
    RIL_STK_COMQUAL comQual;
    _CHAR8 url[MAX_STK_STRING];
    _CHAR8 browserId[LARGE_STK_STRING];
    RIL_STK_BEARER bearer;
    _UCHAR8 numFiles;
    _CHAR8 provFiles[MAX_FILE_ITEM][LARGE_STK_STRING];
    RIL_STK_ALPHABET dcs;
    _CHAR8 gateway[LARGE_STK_STRING];
    _USHORT16 alphaId[LARGE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_LAUNCHBROWSER;

/** STGC open channel definition*/
typedef struct{
    _CHAR8 alphaId[LARGE_STK_STRING];
    _UCHAR8 iconId;
    RIL_STK_DISPLAY dispMode;
}RIL_STGC_OPENCHANNEL;

/** STGC setup event list definition*/
typedef struct{
    _UCHAR8 eventNum;
    RIL_STK_EVENTLIST eventList[5];
}RIL_STGC_SETUPEVENTLIST;

/** STK command data definition*/
typedef union{
    RIL_STUD_REFRESH                u_refresh;
    RIL_STUD_SENDSS                    u_send_ss;
    RIL_STUD_SENDUSSD                u_send_ussd;
    RIL_STUD_SENDSM                    u_send_sm;
    RIL_STUD_SETUPCALL                u_setup_call;
    RIL_STUD_RUNAT                    u_run_at;
    RIL_STUD_LANGUAGENOTIFICATION    u_language_notification;
    RIL_STUD_CLOSECHANNEL            u_close_channel;
    RIL_STUD_RECEIVE                u_receive;
    RIL_STUD_SEND                    u_send;

    RIL_STGC_DISPLAYTEXT            u_display_text;
    RIL_STGC_GETINKEY                u_get_inkey;
    RIL_STGC_GETINPUT                u_get_input;
    RIL_STGC_PLAYTONE                u_play_tone;
    RIL_STGC_SETUPMENU                u_setup_menu;
    RIL_STGC_SELECTITEM                u_select_item;
    RIL_STGC_GETACKNOWLEDGEMENT        u_get_acknowledgement;
    RIL_STGC_SETUPIDLEMODETEXT        u_setup_idle_text;
    RIL_STGC_SENDDTMF                u_send_dtmf;
    RIL_STGC_LAUNCHBROWSER            u_launch_browser;
    RIL_STGC_OPENCHANNEL            u_open_channel;
    RIL_STGC_SETUPEVENTLIST            u_setup_event_list;
}RIL_STK_UNION;

/** STK response definition*/
typedef union{
    RIL_STK_DISPLAYTEXT_RES r_display_text;
    RIL_STK_GETINKEY_RES r_get_inkey;
    RIL_STK_GETINPUT_RES r_get_input;
    RIL_STK_PLAYTONE_RES r_play_tone;
    RIL_STK_SETUPMENU_RES r_setup_menu;
    RIL_STK_SELECTITEM_RES r_select_item;
    RIL_STK_SETUPCALL r_setupcall;
    RIL_STK_SETUPIDLEMODETEXT_RES r_setup_idle_text;
    RIL_STK_SENDDTMF_RES r_send_dtmf;
    RIL_STK_LAUNCHBROWSER_RES r_launch_browser;
    RIL_STK_OPENCHANNEL_RES r_open_channel;
    RIL_STK_SETUPEVENTLIST_RES r_setup_event_list;
}RIL_RES_UNION;

/** STK response definition*/
typedef struct{
    RIL_STK_CMD    cmdId;
    RIL_RES_UNION    res;
    RIL_STK_UNION   para;
    _CHAR8        data[MAX_STK_STRING*4+4];
    _SHORT16        content[MAX_STK_STRING*4+4];
    _SHORT16        inkeycontent[10];
    _USHORT16       select;
}RIL_RES_PAR;

/** CLIP URC status definition*/
typedef enum {
    RIL_CLIP_Disable = 0,
    RIL_CLIP_Enable
} RIL_CLIP_URCSTA;

/** CLIP mode definition*/
typedef enum {
    RIL_CLIP_NActive = 0,
    RIL_CLIP_Active,
    RIL_CLIP_Unknw
} RIL_CLIP_MODE;

/** CLIP definition*/
typedef struct {
    RIL_CLIP_MODE    m;        /* 0 CLIP not provisioned; 1 CLIP provisioned; 2 unknown */
    RIL_CLIP_URCSTA    n;        /* 0 Suppress Unsolicted Result Codes; 1 display */
} RIL_GSM_CLIP;

/** CLIR mode definition*/
typedef enum {
    RIL_CLIR_MUnknw = 0,
    RIL_CLIR_Invctn,
    RIL_CLIR_Spprssn
} RIL_CLIR_MODE;

/** CLIR status definition*/
typedef enum {
    RIL_CLIR_NPrvsn = 0,
    RIL_CLIR_Prvsn,
    RIL_CLIR_SUnknw,
    RIL_CLIR_TmpRstrct,
    RIL_CLIR_TmpAllw
} RIL_CLIR_STAT;
    
/** CLIR definition*/
typedef struct {
    RIL_CLIR_MODE    n;        /**  0 Presentation indicator is used according to the subscription of the CLIR service  */
                            /**  1 CLIR invocation; 2 CLIR suppression */
    RIL_CLIR_STAT    m;        /**   0 CLIR not provisioned; 1 CLIR provisioned in permanent mode */
                            /**  3 Unknown    4 CLIR in temporary mode presentation restricted */
                            /**   5 CLIR in temporary mode presentation allowed */
} RIL_GSM_CLIR;

/** USSD status definition*/
typedef enum {
    RIL_USSDOFF = 0,
    RIL_USSDON
} RIL_USSD_STATE;

/** COL definition*/
typedef enum {
    RIL_COL_NActive = 0,
    RIL_COL_Active,
    RIL_COL_Unkown                /**accord with SIM300*/ 
} RIL_COL_MODE;
/** COLP result code definition*/                            
typedef enum {
    RIL_DISABLE = 0,
    RIL_ENABLE
} RIL_COLP_SET;
/** COLP parse result definition*/  
typedef struct {
    RIL_COLP_SET colpn;
    RIL_COL_MODE colpm;
}RIL_COLP_INQUIRY;

/** USSD URC definition*/
typedef struct {
    _CHAR8    type;
    _CHAR8    *str;
    _SHORT16    dcs;
    _CHAR8    channel;            /** if type == 1:  current channel need to be locked immediately */
} RIL_USSD_URC;

/** GPIO control definition*/
typedef struct{
    _UCHAR8 ident;
    _UCHAR8 status;
}RIL_GPIO_CONTROL;

/** SMS full definition*/
typedef enum {
    RIL_SM_NOTFULL = 0,
    RIL_SM_FULL,
    RIL_SM_FULL_NEWSM
} RIL_SMS_FULL;

/**cell test parse definition */
typedef struct{
    T_STR_CELLLEN   neigCell[NEIG_CELL_NUM][NEIG_ITEM_NUM];
    T_STR_CELLLEN    servCell[SERV_ITEM_NUM];
    _USHORT16            neigCellNumber;
    _USHORT16            servCellNUmber;
}RIL_CELL_TEST;

/** URC event type difinition */
typedef enum 
{
    WM_TYPE_CALL        = 0x00000001,    /**< call */
    WM_TYPE_MSG            = 0x00000002,   /**< sms,cds,cbm */
    WM_TYPE_STK            = 0x00000004,    /**< STK */
    WM_TYPE_NETWORK        = 0x00000008,    /**< NETWORK */
    WM_TYPE_BATTERY        = 0x00000010,    /**< BATTERY */
    WM_TYPE_MODEM        = 0x00000020,    /**< MODEM */
    WM_TYPE_USSD        = 0x00000040,    /**< USSD */
    WM_TYPE_XDRV        = 0x00000080,    /**< hardware,XDRV */
    WM_TYPE_SIM            = 0x00000100    /**< SIM card */
}RIL_URC_TYPE;

enum
{
    RIL_RTN_OK = 0x0,
    RIL_RTN_INVALID_HANDLE = 0x1000000,
    RIL_RTN_WM_NOTREADY = 0x1000001
};

/** URC event ID */
typedef enum 
{  
    RIL_NOTIFY_RING = 0,                /**< 新来电通知.该消息的参数是一个T_U32类型的值.表示通话句柄*/
    RIL_NOTIFY_CALLWAITING,                /**< 新waiting call通知，该消息的参数是一个T_U32类型的值.表示通话句柄*/
    RIL_NOTIFY_CALLINLINE,                /**< CLIP通知，该消息的参数是一个RIL_URC_CLIP结构体*/
    RIL_NOTIFY_CALLDIALING,                /**< 新dialing call通知.该消息的参数是一个T_U32类型的值.表示dialing call的句柄*/
    RIL_NOTIFY_DISCONNECT,                /**< 有通话连接断开.内部消息*/
    RIL_NOTIFY_CONNECT,                    /**< 有通话接通.内部消息*/
    RIL_NOTIFY_NOANSWER,                /**< 内部消息*/
    RIL_NOTIFY_BUSY,
    RIL_NOTIFY_NODIALTONE,    
    RIL_NOTIFY_CALLSTATECHG,            /**< 通话状态有改变,该消息的参数是RIL_CALLSTATECHG_REASON,表示状态改变的原因*/
    RIL_NOTIFY_MESSAGE,                    /**< 新短信事件，该消息的参数是一个RIL_URC_MSG结构体*/
    RIL_NOTIFY_BCMESSAGE,                /**< 新小区广播事件，该消息的参数是一个RIL_URC_MSG结构体*/
    RIL_NOTIFY_STATUSMESSAGE,            /**< 新状态报告事件，该消息的参数是一个RIL_URC_MSG结构体*/
    RIL_NOTIFY_MESSAGE_IN_SIM,            /**< 新短信事件，该消息的参数是一个_USHORT16类型的值，表示该短信在SIM中的存储位置*/
    RIL_NOTIFY_STK_Refresh,                /**< STK Refresh消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_EventList,            /**< STK EventList消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SetupCall,            /**< STK SetupCall消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SendSS,                /**< STK SendSS消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SendUSSD,            /**< STK SendUSSD消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SendSM,                /**< STK SendSM消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SendDTMF,            /**< STK SendDTMF消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_LaunchBrowser,        /**< STK LaunchBrowser消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_PlayTone,            /**< STK PlayTone消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_DisplayText,            /**< STK DisplayText消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_GetInkey,            /**< STK GetInkey消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_GetInput,            /**< STK GetInput消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SelectItem,            /**< STK SelectItem消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SetupMenu,            /**< STK SetupMenu消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_LanguageSet,            /**< STK LanguageSet消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SetupIdleModeText,    /**< STK SetupIdleModeText消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_LanguageNotification,/**< STK LanguageNotification消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_MenuSelection,        /**< STK MenuSelection消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_EndProactive,        /**< STK EndProactive消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_STTONE,                /**< STK STTONE消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_RunAT,                /**< STK RunAT消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_OpenChannel,            /**< STK OpenChannel消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_CloseChannel,        /**< STK CloseChannel消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_ReceiveData,            /**< STK ReceiveData消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_STK_SendData,            /**< STK SendData消息，该消息的参数是一个RIL_STK_PAR结构体*/
    RIL_NOTIFY_REGSTATUSCHANGED,        /**< 网络状态更新消息，该消息的参数是RIL_NET_STAT变量指针*/
    RIL_NOTIFY_SIMREADY,                /**< SIM卡初始化完成消息*/

    RIL_NOTIFY_TIMEZONEREPORT,            /**< 基站时区时间上报消息, 该消息得到一个系统类型时间*/
    RIL_NOTIFY_CALLPROGRESS,            /**< 标识通话的阶段*/
    RIL_NOTIFY_CSSU,                    /**< CSSU*/
    RIL_NOTIFY_MEMO_OCCUR,                /**< alarm*/
    RIL_NOTIFY_MODULE_RESET,            /**< module reset, 该消息的参数是RIL_EXCEPTION_TYPE变量*/
    RIL_NOTIFY_POWERON,                    /**< turn on phone*/
    RIL_NOTIFY_POWEROFF,                /**< turn off phone*/
    RIL_NOTIFY_SUPSERVICEDATA,            /**< CUSD*/
    RIL_NOTIFY_SIGNALUPDATE,            /**< 信号量有改变*/
    RIL_NOTIFY_CALLOUTLINE,                /**< COLP*/
    RIL_NOTIFY_HEADSET,                    /**< headset event*/
    RIL_NOTIFY_HEADSETBUTTON,            /**< headset button event*/
    RIL_NOTIFY_BATTERYLOW,                /**< battery low event*/
    RIL_NOTIFY_BATTERYSHUTDOWN,            /**< battery shutdown event*/
    RIL_NOTIFY_CALLSTATE_ALERTING,        /**< 标识拨号的阶段: 回铃音*/
} RIL_URC_ID;

/** STK command data definition*/
typedef struct{
    RIL_URC_ID                    cmdId;
    RIL_STK_UNION                    para;
    _CHAR8                        cmdQualifier[4];
    _CHAR8                        cmdNumber[4];
}RIL_STK_PAR;

/**sms,cds,cbm parse*/
typedef struct
{
    _SHORT16     length;    
    T_STR_PDU    Pdudata;
    T_STR_NUMBER Number;
} RIL_URC_MSG;

/**cmti parse*/
typedef struct
{
    _USHORT16 index;
} RIL_URC_CMTI;

/** Clip URC status definition*/
typedef enum {
    RIL_CLIValid,
    RIL_CLIOHold,
    RIL_CLINotValid
} RIL_URC_CLIP_STATE;
/**  Clip URC definition*/
typedef struct {
    T_STR_TELE        telenumber;        
    RIL_TELE_TYPE    type;
    RIL_URC_CLIP_STATE    cli;
} RIL_URC_CLIP;
/**  COLp URC definition*/
typedef struct {
    T_STR_TELE        telenumber;        
    RIL_TELE_TYPE    type;
    RIL_URC_CLIP_STATE    cli;
} RIL_URC_COLP;
typedef enum {
    RIL_NAME_HEADSET = 0,
    RIL_NAME_HOOKKEY,
    RIL_NAME_POWERONREASON,
    RIL_NAME_BATTERYLOW,
    RIL_NAME_BATTERYSHUTDOWN,
    RIL_NAME_BETTERYVOLTAGE,
    RIL_NAME_BATTERYTEMPER,
    RIL_NAME_ENVIRTEMPER,
    RIL_NAME_STATICEEP,
    RIL_NAME_CHIPID,
    RIL_NAME_BATTERYIDVOL,
    RIL_NAME_MAX_NUM
} RIL_XDRV_DRIVER_NAME;

typedef enum {
    RIL_TYPE_SETURCRPT = 0,
    RIL_TYPE_INQUIRE,
    RIL_DRV_MAX_TYPE
} RIL_XDRV_CONTROL_TYPE;

typedef struct{
    RIL_XDRV_DRIVER_NAME devName;
    RIL_XDRV_CONTROL_TYPE ctrType;
    _BOOL ctrEnable;
    _CHAR8* strValue1;
    _CHAR8* strValue2;    
} RIL_sXDRV_TYPE;

typedef enum {
    RIL_AUDIO_MASTER_VOLUME = 0,
    RIL_AUDIO_SOURCE_VOLUME,
    RIL_AUDIO_CTL_MAX_NUM
} RIL_XDRV_AUDIO_CTRL_TYPE;

typedef enum {
    RIL_PATH_PHONE_NORMAL = 0,
    RIL_PATH_HEADSET,
    RIL_PATH_BLUETOOTH,
    RIL_PATH_HANDFREE,
    RIL_PATH_ENGINEER_TEST,
    RIL_PATH_MAX
} RIL_AUDIO_PATH_TYPE;

typedef enum {
    RIL_AUDMOD_HANDSET = 0,
    RIL_AUDMOD_HEADSET,
    RIL_AUDMOD_MAX
} RIL_AUDIO_MOD_TYPE;

typedef enum {
    RIL_URC_NORMAL = 0,
    RIL_URC_DATACONN,
    RIL_URC_BUSY,
    RIL_URC_SLEEP
} RIL_URC_MODE;

typedef enum {
    RIL_STORENEWMSG_SIMFIRST = 0,
    RIL_STORENEWMSG_MEFIRST,
    RIL_STORENEWMSG_MEFIRST_RPT,
} RIL_STORENEWMSG_MODE;

typedef enum {
    RIL_EXCEPTION_SOFTWARE = 0,
    RIL_EXCEPTION_HARDWARE
} RIL_EXCEPTION_TYPE;

typedef enum {
    CALLSTATECHG_MANUAL = 0,
    CALLSTATECHG_RING,
    CALLSTATECHG_CCWA,
    CALLSTATECHG_NOCARRIER,
    CALLSTATECHG_NOANSWER,
    CALLSTATECHG_NODIALTONE,
    CALLSTATECHG_BUSY,
    CALLSTATECHG_CONNECT
} RIL_CALLSTATECHG_REASON;

#define MAX_SUPPORT_CHANNELS    12

#endif//#ifndef WM_STRUCT_H

