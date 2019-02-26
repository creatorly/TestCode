/******************************************************************************
                Copyright (c) 2005-2005 Ejoin-Tech Inc.
                
Filename   :    ccm_msgcomm.h
Description:    define the message and the interface to access it
Revision:       1.0
Author:         pengjian
Date:           2005.10.29
******************************************************************************/

#ifndef CCM_MSGCOMM_H
#define CCM_MSGCOMM_H

#ifdef __cplusplus
extern "C" {
#endif

/********************* OS HEADFILE DEFINITION ********************************/

#define OS_MemCpy(dst, src, len)    memcpy((_UCHAR8*)(dst), (_UCHAR8*)(src), len)
#define OS_MemSet(dst, val, len)    memset(dst, val , len)
#define OS_MemCmp(dst, val, len)    memcmp(dst, val , len)

#define OS_StrLen(str)              strlen((_CHAR8*)(str))
#define OS_StrCpy(dst, src)         strcpy((_CHAR8*)(dst), (_CHAR8*)(src))
#define OS_StrCmp(dst, src)         strcmp((_CHAR8*)(dst), (_CHAR8*)(src))
#define OS_StrNcmp(dst, src, n)     strncmp((_CHAR8*)(dst), (_CHAR8*)(src), n)

#define OS_Atoi(str)                atoi(str)
#define OS_Iota(i, str, radix)      itoa(i, str, radix)

/********************* MACRO DEFINITION **************************************/

#define CCM_SYNC_FLAG           0xaaaa5555
#define CCM_IE_REQUEST          0xffffffff
#define CCM_INVALID_VALUE       0xffffffff

#define CCM_MSG_GETSYNC(msg)    ((msg)->ulSync)
#define CCM_MSG_GETSENDER(msg)  ((msg)->ulMsgSender)
#define CCM_MSG_GETRECVER(msg)  ((msg)->ulMsgRecver)
#define CCM_MSG_GETCALLREV(msg) ((msg)->ulCallRev)
#define CCM_MSG_GETTYPE(msg)    ((msg)->ulMsgType)
#define CCM_MSG_GETLEN(msg)     ((msg)->ulMsgLen)
#define CCM_MSG_GETDATA(msg)    ((msg)->data)
#define CCM_MSG_GETEND(msg)     ((_CHAR8*)(msg) + (msg)->ulMsgLen)

#define CCM_MSG_SETSYNC(msg, sync)          ((msg)->ulSync = (sync))
#define CCM_MSG_SETSENDER(msg, sender)      ((msg)->ulMsgSender = (sender))
#define CCM_MSG_SETRECVER(msg, recver)      ((msg)->ulMsgRecver = (recver))
#define CCM_MSG_SETTRANSID(msg, transid)    ((msg)->ulTransId = (transid))
#define CCM_MSG_SETCALLREV(msg, callrev)    ((msg)->ulCallRev = (callrev))
#define CCM_MSG_SETTYPE(msg, type)          ((msg)->ulMsgType = (type))
#define CCM_MSG_SETLEN(msg, len)            ((msg)->ulMsgLen = (len))

#define CCM_MSG_IEBEGIN(msg)    ((ST_CCM_IE*)CCM_MSG_GETDATA(msg))
#define CCM_MSG_IEEND(msg)      ((ST_CCM_IE*)CCM_MSG_GETEND(msg))

#define CCM_MSG_CONSTRUCTEX(msg, sync, sender, recver, callrev, type, len) \
{\
    (msg)->ulSync = (sync); \
    (msg)->ulMsgSender = (sender); \
    (msg)->ulMsgRecver = (recver); \
    (msg)->ulCallRev = (callrev); \
    (msg)->ulMsgType = (type); \
    (msg)->ulMsgLen = (len); \
}

#define CCM_MSG_CONSTRUCT(msg, sender, recver, callrev, type) \
            CCM_MSG_CONSTRUCTEX(msg, CCM_SYNC_FLAG, sender, recver, \
                                callrev, type, sizeof(ST_CCM_MSG))
#define CCM_MSG_DESTRUCT(msg)

#define CCM_LEN_ALIGN(len)  (((len) + 3) & (~3))

#define CCM_IE_GETTYPE(ie)  ((ie)->ulType)
#define CCM_IE_GETLEN(ie)   ((ie)->ulLen)
#define CCM_IE_GETDATA(ie)  ((ie)->data)
#define CCM_IE_GETSIZE(ie)  (sizeof(ST_CCM_IE) + CCM_LEN_ALIGN((ie)->ulLen))

#define CCM_IE_GETTDATA(ie, T)  (*(T*)CCM_IE_GETDATA(ie))
#define CCM_IE_GET_INT8(ie)     CCM_IE_GETTDATA(ie, _CHAR8)
#define CCM_IE_GET_INT16(ie)    CCM_IE_GETTDATA(ie, _SHORT16)
#define CCM_IE_GET_INT32(ie)    CCM_IE_GETTDATA(ie, _INT32)
#define CCM_IE_GET_UINT8(ie)    CCM_IE_GETTDATA(ie, _UCHAR8)
#define CCM_IE_GET_UINT16(ie)   CCM_IE_GETTDATA(ie, _USHORT16)
#define CCM_IE_GET_UINT32(ie)   CCM_IE_GETTDATA(ie, _UINT32)

/* get the 4-byte alignment address of next ie, 
   CCM_LEN_ALIGN(-1) = 0, so don't need to tell if len = CCM_IE_REQUEST */
#define CCM_IE_NEXT(ie)     \
    ((ST_CCM_IE*)((ie)->data + CCM_LEN_ALIGN((ie)->ulLen)))

#define CCM_IE_SETTYPE(ie, type)    ((ie)->ulType = (type))
#define CCM_IE_SETLEN(ie, len)      ((ie)->ulLen = (len))

#define CCM_IEEX_CONSTRUCT(ieex, type, len, val) \
{ \
    (ieex)->ulType = (type); \
    (ieex)->ulLen = (len); \
    (ieex)->pvData = (val); \
}

#define CCM_IEEX_INIT_INT(ieex, type, val) \
            CCM_IEEX_CONSTRUCT(ieex, type, sizeof(val), &val)

#define CCM_IEEX_INIT_STR(ieex, type, sval) \
            CCM_IEEX_CONSTRUCT(ieex, type, strlen(sval)+1, sval)

/********************* STRUCTION DEFINITION **********************************/
typedef enum
{
    EN_CCM_MSGTYPE_SETUP = 0,
    EN_CCM_MSGTYPE_SETUP_ACK,
    EN_CCM_MSGTYPE_INFOMATION,
    EN_CCM_MSGTYPE_CALLPROCEEDING,
    EN_CCM_MSGTYPE_ALERTING,
    EN_CCM_MSGTYPE_CONNECT,
    EN_CCM_MSGTYPE_CONNECT_ACK,
    EN_CCM_MSGTYPE_RELEASE,
    EN_CCM_MSGTYPE_RELEASE_ACK,
    EN_CCM_MSGTYPE_REGISTER,
    EN_CCM_MSGTYPE_REGISTER_ACK,
    EN_CCM_MSGTYPE_UNREGISTER,
    EN_CCM_MSGTYPE_UNREGISTER_ACK,

    EN_CCM_MSGTYPE_HB,
    EN_CCM_MSGTYPE_HB_ACK,

    EN_CCM_MSGTYPE_OAM,
    
    EN_CCM_MSGTYPE_BUTT
}EN_CCM_MSGTYPE;

typedef enum
{
    EN_CCM_PROTO_FXS  = 0x00010000,
    EN_CCM_PROTO_POTS  = 0x00020000,
    EN_CCM_PROTO_MGCP = 0x00040000,
    EN_CCM_PROTO_H248 = 0x00080000,
    EN_CCM_PROTO_H323 = 0x00100000,
    EN_CCM_PROTO_SIP  = 0x00200000,
    EN_CCM_PROTO_ECCP = 0x00400000,
    EN_CCM_PROTO_PRI  = 0x00800000,
    EN_CCM_PROTO_NO1  = 0x01000000,
    EN_CCM_PROTO_CCM  = 0x02000000,    
    EN_CCM_PROTO_SLIC = 0x04000000,
    EN_CCM_PROTO_DSP  = 0x08000000,
    
    EN_CCM_PROTO_NULL = 0
    
}EN_CCM_PROTO;
    
typedef enum
{
    EN_CCM_IENAME_ERRCODE = 0,
    EN_CCM_IENAME_CALLID,
    EN_CCM_IENAME_ONHOOK,
    EN_CCM_IENAME_BUSYTONE_DETECT = EN_CCM_IENAME_ONHOOK,
    EN_CCM_IENAME_OFFHOOK,
    EN_CCM_IENAME_RING_DETECT = EN_CCM_IENAME_OFFHOOK,
    EN_CCM_IENAME_DTMF_VALUE,
    EN_CCM_IENAME_DTMF_USER_INPUT = EN_CCM_IENAME_DTMF_VALUE,
    EN_CCM_IENAME_MEDIA_PLT,
    EN_CCM_IENAME_MEDIA_CODEC,
    EN_CCM_IENAME_MEDIA_IP,
    EN_CCM_IENAME_IP = EN_CCM_IENAME_MEDIA_IP,
    EN_CCM_IENAME_MEDIA_PORT,
    EN_CCM_IENAME_EPPORT = EN_CCM_IENAME_MEDIA_PORT,
    EN_CCM_IENAME_MEDIA_CHANNEL_OPEN,
    EN_CCM_IENAME_MEDIA_CHANNEL_CLOSE,
    EN_CCM_IENAME_MEDIA_FAX_START,
    EN_CCM_IENAME_MEDIA_FAX_STOP,
    EN_CCM_IENAME_CALLING_NUMBER,
    EN_CCM_IENAME_CALLED_NUMBER,
    EN_CCM_IENAME_DISCONNECT_CAUSE,
    EN_CCM_IENAME_BILL_START,
    EN_CCM_IENAME_BILL_STOP,
    EN_CCM_IENAME_EPNAME,
    EN_CCM_IENAME_DIALTONE,
    EN_CCM_IENAME_RINGBACK,
    EN_CCM_IENAME_PKGLIST,
    EN_CCM_IENAME_CONN_MODE,
    EN_CCM_IENAME_RATE,
    EN_CCM_IENAME_CHANNO,
    EN_CCM_IENAME_DTMF_END,
    EN_CCM_IENAME_FAXSTART,
    EN_CCM_IENAME_FAXSTOP,
    EN_CCM_IENAME_REM_RB,
    EN_CCM_IENAME_DTMF_DURATION,
    EN_CCM_IENAME_LINEIN,
    EN_CCM_IENAME_LINEOUT,
    EN_CCM_IENAME_CALL_ONHOLD,
    EN_CCM_IENAME_CALL_OFFHOLD,
    EN_CCM_IENAME_DTMF_ADJUST_LEN,
    EN_CCM_IENAME_PASSWORD,
    EN_CCM_IENAME_TALKING,
    EN_CCM_IENAME_T38_MAXBITRATE,
    EN_CCM_IENAME_MEDIA_PTIME,
    EN_CCM_IENAME_FLASH_HOOK,
    EN_CCM_IENAME_RFC2833_PLTYPE,
    EN_CCM_IENAME_CALLER_ID,

    EN_CCM_IENAME_CMD,
    EN_CCM_IENAME_CMD_PARINT,
    EN_CCM_IENAME_CMD_PARSTR,   /* extension parameters */
    EN_CCM_IENAME_CMD_HANDLER,
    EN_CCM_IENAME_CMD_DATA,
    /* add new ie to here */

    EN_CCM_IENAME_CALLER_ADDR,
    EN_CCM_IENAME_CALLED_ADDR,

#if ACOM_HW_HAS_MOBILE    
    EN_CCM_IENAME_SMS,      /* encoded in UTF-8 */
    EN_CCM_IENAME_SMS_RES,  /* 0: success */
    EN_CCM_IENAME_SMS_DRPT, /* SMS Deliver Report */
    EN_CCM_IENAME_SMS_DST,  /* SMS destination number */
#endif

    EN_CCM_IENAME_ALERTING,
    EN_CCM_IENAME_NO_MANIPULATION,

    EN_CCM_IENAME_BUTT
}EN_CCM_IENAME;

typedef enum
{
    EN_CCM_MSG_OK = 0,
    EN_CCM_MSG_ERRNO_NORESP,

    EN_CCM_MSG_ERROR = 0xffffffff
}EN_CCM_MSG_ERRNO;

#define CCM_MSG_CONNMODE_INACTIVE "inactive"
#define CCM_MSG_CONNMODE_RECVONLY "recvonly"
#define CCM_MSG_CONNMODE_SENDONLY "sendonly"
#define CCM_MSG_CONNMODE_SENDRECV "sendrecv"

typedef enum
{
    EN_CCM_IEPROMPT_DIALTONE = 0,
    EN_CCM_IEPROMPT_PORT_USERPROMPT,
    EN_CCM_IEPROMPT_PORT_PASSWORD,
    EN_CCM_IEPROMPT_USERPROMPT,
    EN_CCM_IEPROMPT_PASSWORD
}EN_CCM_IEPROMPT;

typedef struct tagstCcmMsg
{
    _UINT32 ulSync;
    _UINT32 ulMsgSender;
    _UINT32 ulMsgRecver;
    _UINT32 ulCallRev;
    _UINT32 ulMsgType;
    _UINT32 ulMsgLen;
    _CHAR8  data[0];
}ST_CCM_MSG;

typedef struct tagstCcmInfoElem
{
    _UINT32 ulType;
    _UINT32 ulLen;
    _CHAR8  data[0];
}ST_CCM_IE;

typedef struct tagstCcmInfoElemEx
{
    _UINT32 ulType;
    _UINT32 ulLen;
    const _VOID*  pvData;
}ST_CCM_IEEX;

typedef _VOID* (*FP_CCM_MSG_MALLOC)(_UINT32 uiSize);

/********************* FUNCTION DEFINITION ***********************************/
_UINT32 CCM_MsgConstruct(ST_CCM_MSG** ppstMsg, EN_CCM_MSGTYPE enMsgType);

ST_CCM_IE* CCM_GetIe(ST_CCM_MSG* pstCcmMsg, _UINT32 enIeType);
ST_CCM_IE* CCM_GetIeByIdx(ST_CCM_MSG* pstCcmMsg, _UINT32 ulIndex);
_UINT32  CCM_AddIeCopy(ST_CCM_MSG* pstCcmMsg, ST_CCM_IE* pstCcmIe);
_UINT32  CCM_AddIe(ST_CCM_MSG* pstCcmMsg, _UINT32 enIeType, 
                   _UINT32 ulIeLen, const _VOID* pvData);
#define CCM_MsgHaveIe(msg, iename) \
    (NULL != CCM_GetIe(msg, iename))

const _CHAR8* CCM_GetMsgtypeName(EN_CCM_MSGTYPE msgtype);
const _CHAR8* CCM_GetIetypeName(EN_CCM_MSGTYPE msgtype);

ST_CCM_MSG* CCM_GetMsgCopy(ST_CCM_MSG* msg, FP_CCM_MSG_MALLOC fpMalloc);

_UINT32  CCM_AddIntIe(ST_CCM_MSG* pstMsg, 
                     _UINT32 enIeType, 
                     _UINT32 uiIeVal);

#define CCM_AddStrIe(msg, iename, strie) \
    CCM_AddIe(msg, iename, strlen(strie)+1, strie)

_UINT32 CCM_GetIeInt(ST_CCM_MSG* msg, _UINT32 uiIeName);
_CHAR8* CCM_GetIeStr(ST_CCM_MSG* msg, _UINT32 uiIeName);

_UINT32 CCM_GetErrcode(ST_CCM_MSG* msg);

EN_CCM_PROTO CCM_GetProtoID(_CHAR8* pcProto);
const _CHAR8* CCM_GetProtoName(EN_CCM_PROTO uiPorto);

/****************************************************************************/
typedef _VOID* H_CCM_MSGPROC;
typedef _UINT32 (*FP_CCM_MSG_PROC)(_VOID* msg, _VOID* node, _VOID* data);

_UINT32 CCM_MsgProcConstruct(H_CCM_MSGPROC* phProc,
                            _UINT32 uiStateNum, 
                            _UINT32 uiMsgTypeNum,
                             FP_CCM_MSG_PROC fpDefProc,
                            _VOID* data);

_UINT32 CCM_MsgProcDestruct(H_CCM_MSGPROC hProc);

_UINT32 CCM_MsgProcRegister(H_CCM_MSGPROC hProc,
                           _UINT32 uiState, 
                           _UINT32 uiMsgType,
                            FP_CCM_MSG_PROC fpProc);

_UINT32 CCM_MsgProcTrig(H_CCM_MSGPROC hProc,
                       _UINT32 uiState, 
                       _UINT32 uiMsgType, 
                       _VOID* msg,
                       _VOID* node);

#ifdef __cplusplus
}
#endif

#endif
