/********************************************************************
 * @file svrcom.h
 * @brief the header files of server communication
 ********************************************************************/
#ifndef SERVERCOMMUNICATION_H
#define SERVERCOMMUNICATION_H

#include "wm_struct.h"

#define ATL_SOCKET unsigned long

typedef enum _svrcall{
    MakeCall = 0,
    ECALLREJECTCALL,
    ECALLANSWERCALL,
    ECALLHANGUPCALL,
    ECALLCANCELDIAL,
    ECALLSWAPCALL,
    ECALLCONFERENCE,
    ECALLENDSINGLE,
    ECALLENDACTIVE,
    ECALLENDHOLD,
    ECALLSPLITFROMCONFER,
    ECALLCURCALLLIST,
    ECALLGETCALLINFO,
    ECALLGETCALLQty,

    EQISENDDATA,

    ECALLSENDMSG,
    ECALLSENDMSG_MC323,
    ECALLSENDMSG_CDMA,
    ECALLSENDMSGBYTXT,
    ECALLREADMSG,
    ECALLREADMSGBYTXT,
    ECALLREADMSG_CDMA,    
    ECALLSAVEMSG,
    ECALLDELETEMSG,
    ECALLREADMSGUNCHGSTAT,
    INQUIRESMSSTORAGE,
    LISTSMS,
    INQUIREMSGCENTER,
    NEWMSGSTOREMODE,
    SETMSGCENTER,
    SETEXTSMSSTORAGESTATUS,
    
    ESTK_SETPROFILE,
    ESTK_SETAUTORSP,
    ESTK_SETTR,
    ESTK_SELECTMAINMENU,
    ESTK_RESPONSE,

    INQUIREGPRSSTATE,
    DATACONN_WITHGPRS,
    DATACONN_WITHCSD,
    DATACONN_RELEASE,

    ECALLOPENLINE,
    ECALLCLOSELINE,
    ECALLATTENTION,
    GETSLEEPSTATE,
    ECALLSETUP,
    ECALLSTOP,
    ECALLUPDATECHNL,

    
    INQUIRECLOCK,
    INQUIRECUROPERATOR,
    INQUIREBSINFO,
    SETCOMMANDECHO,
    GETPRODUCTID,
    SETRESULTPRESENTATION,
    SETRESULTFORMAT,
    SETMANUFDEFAULT,
    INQUIRECCID,
    INQUIREIMSI,
    INQUIREIMEI,
    SETIMEI,
    LOCKBS,
    SETBAUDRATE,
    INQUIREBAND,
    SETNETBAND,
    GETCURRENTACM,
    RESETACM,
    GETACMMAX,
    SETACMMAX,
    GETCURCALLMETER,
    SETCLOCK,
    GETEXTENDEDERROR,
    GETFUNCTIONALITY,
    GETSIMVOL,
    SETFUNCTIONALITY,
    SETSIMVOL,
    SETRINGBACK,
    SETVBATT,
    SETNWSCANMODE,
    SETOPENTCPUDP,
    INQUIREQISTAT,
    INQUIREQISACK,
    INQUIREQILOCIP,
    INQUIREQIRCVDATA,
    SETQICLOSE,
    INQUIREQIREGAPP, 
    SETQIREGAPP,
    SETQICSGP,
    SETQIACT,
    SETQIDEACT,
    SETQINDI,
    SETQIDNSIP,
    SETCNETSCAN,
    INQUIRECELLLOC,
    SETQFUN,
    SETQMOSTAT,
    SETQCCINFOEN,
    SETFSENDSIMDATA,
    SETQURCCFG,
    SETQJAMDET,    
    INQUIRESUBSCRIBER,
    SETSIMSLOT,
    SETSPEAKERVOL,
    SETFREQBAND,
    REPORTMOBILERROR,
    ENTERMULTIPLEX,
    READOPERATORNAMES,
    INQUIREAVAIOPERATOR,
    SELECTOPERATOR,
    SETENGMODE,
    INQUIREPINSTATUS,
    INQUIREINITSTATUS,
    ENTERPIN,
    CHANGEPASSWORD,
    NETWORKREGISTRATION,
    INQUIRENETREGSTATUS,
    INQUIREGNETREGSTATUS,
    SETCHARACTERSET,
    INQUIRESIGNALQUALITY,
    INQUIRENETTYPE, //weikk 2016.3.31 add XREG
    INQUIREADC,     //weikk 20161026 add ADC
    INQUIREMMAD, //weikk 20160713 add MMAD
    INQUIRESIDETONE,
    INQUIRECALLSTATUS,
    SETSSNOTE,
    SENDUSSD,
    SENDUSSDDATA,
    CANCELUSSDDATA,
    DIALDTMF,
    SETDTMFPRD,
    SETALARM,
    FIELDTESTREPORT,
    SWITCHOFFMODULE,
    SETMICGAINLEVEL,
    CONFIGUREHANDFREE,
    CONFIGUREAUDMOD,
    VOICEHANGUPCTRL,
    SETAUDIOMUTE,
    PLAYTONE,
    INQUIREHEADSETSTATUS,
    INQUIREHEAdSETBTNSTATUS,
    INQUIREBATTERYLEVEL,
    INQUIREPINCOUNT,
    INQUIREPUKCOUNT,
    
    SELECTBROADCAST,
    DUMPBROADCAST,

    SETRINGERSOUNDLEVEL,
    SETAUDIOOUTPUT,
    QUERYMODULESTATE,
    CHECKLATESTURCEVENT,
    CONTROLURCINDICATION,
    CONTROLMODULEDRIVER,
    QUERYURCINDICATION,
    MOBILEEQUIPMENTEVENT,
    SETMSSTORAGETYPE,
    SETMODULESLEEP,
    GETMODEMVERSIONNUM,
    SETCHARGINGCURRENT,
    SETINDICATIONCONTROL,
    XDRVURCCONTROL,
    XDRVAUDIOCONTROL,
    SETTRACEOUTPUT,
    GETMODEMEXCLOG,
    XDRVCONTROLNEWSMSSTATUS,
    XDRVCONTROLAUDIOLOOPTEST,
    
    CALLFORWARD,
    INQUIRECALLFWD,
    SETCALLWAIT,
    INQUIRECALLWAIT,
    FACILITYLOCK,
    INQUIREFACILITYLOCK,
    CALLINGLINEID,
    CHECKCLIP,
    CHECKCLIR,
    SETCLIRACTIVE,
    SETCLIRDISABLE,
    DAILCLIR,
    CHECKCOLR,
    INQUIRECOLP,
    SETCOLPSTATUS,

    READPBENTRIES,
    INQUIREPBSPACE,
    WRITEPBENTRY,
    DELETEPBENTRY,

    INITBEFOREPIN,
    INITAFTERPIN,


    IGNITEMODULE,
    RESETMODULE,
    WMEXCEPTIONCALLBACK,

    TONEDETCTRL,
    DAICTRL,    
    SETQSFR,
    BATTERYCTRL,
    SELECTVOICEGAIN,
    SIDETONEMODIFY,
    SAVECONFIGURE,

    PWRCTRL,
    SENDAT, /* kinson 2012-07-02    */ 
    SENDATORIGIN, /* kinson 2012-10-03    */ 
}E_SVRCALL;

typedef struct _makecall{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    const _UCHAR8* lpszAddress;
    _UINT32 dwType;
    _UINT32 dwOptions;
}T_MAKECALL;


typedef struct {
    E_SVRCALL type;
    _UINT32 hRIL;
    _UINT32 eventType;
    _UINT32 hATServer;
}T_OPENLINE;

typedef struct {
    E_SVRCALL type;
    _UINT32 hLine;
}T_CLOSELINE;


typedef struct {
    E_SVRCALL type;
    _UINT32 ChnlNum;
    _USHORT16 ChID[MAX_SUPPORT_CHANNELS];
    _UINT32 value;
}T_SETUP;


typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
}T_BASICPARAM;



typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _CHAR8* strValue;
}T_ONESTRPARAM;


typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _CHAR8* strValue1;
    _CHAR8* strValue2;
}T_TWOSTRPARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _CHAR8* strValue1;
    _CHAR8* strValue2;
    _CHAR8* strValue3;
}T_THREESTRPARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 value1;
    _UINT32 value2;
    _UINT32 value3;
}T_THREEU32PARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _SHORT16 value;
}T_ONES16PARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _USHORT16 value;
}T_ONEU16PARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 value;
}T_ONEU32PARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 value1;
    _UINT32 value2;
}T_TWOU32PARAM;

typedef struct
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UCHAR8  uValue;
}T_ONEU8PARAM;


typedef struct {
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 hCall;
}T_OPERATECALL;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    RIL_CALA_ALARM *alarmData;
}T_SETALARM;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    RIL_sXDRV_TYPE *xdrvType;
}T_XDRVURCCONTROL;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    RIL_XDRV_AUDIO_CTRL_TYPE audCtrlType;
    _CHAR8* strValue;
}T_XDRVAUDIOCONTROL;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    RIL_CCFC_FWDCALLS *cfData;
}T_CALLFORWARD;




typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _BOOL bURCEnable;
    _BOOL bEnable;
    RIL_DATA_CLASS eClass;
}T_CALLWAIT;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    RIL_DATA_CLASS eClass;
}T_INQUIRECALLWAIT;


typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 fac;
    _BOOL bLock;
    _CHAR8* strPasswd;
    RIL_DATA_CLASS eClass;
}T_FACILITYLOCK;


typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 storage;
    _SHORT16 begin;
    _SHORT16 end;
}T_READPBENTRIES;


typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;
    _UINT32 value;
    _SHORT16 index;
    _CHAR8* strNumber;
    _USHORT16 *strText;
}T_WRITEPBENTRY;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;

    const _UCHAR8* pPDU;
    _UINT32 PDULen;
    const _UCHAR8* pNumber;
    _UINT32 option;
}T_SENDMSG;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;

    const _UCHAR8* pPDU;
    _UINT32 option;
    _UCHAR8 read_flag;
    _SHORT16 PDUlen;
}T_SAVEMSG;

typedef struct 
{
    E_SVRCALL type;
    _UINT32 hRequest;
    _UINT32 hLine;

    _CHAR8* user;
    _CHAR8* dial;
    _CHAR8* pwd;
    _BOOL bMUX;
    _USHORT16  step;
}T_DATACONN;

_UINT32 SvrSend(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size);
_UINT32 SvrRecv(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size);

#endif
