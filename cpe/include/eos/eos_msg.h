#ifndef __EOS_MSG_H
#define __EOS_MSG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstEosMessageEng* EOS_MSG_ID;

typedef enum
{
    MSG_READ = EOS_SELECT_READ,
    MSG_WRITE = EOS_SELECT_WRITE,
    MSG_CONNECT = EOS_SELECT_CONNECT,
    MSG_ACCEPT = EOS_SELECT_ACCEPT,
    MSG_CLOSE = EOS_SELECT_CLOSE
} eosMsgEvents;    

/* young make change
typedef _VOID (*eosMsgCallback) (_INT32 iFd,eosMsgEvents sEvent,EOSBool bError);    */
typedef _VOID (*eosMsgCallback)(_INT32 iSock,eosMsgEvents sEvent,_VOID* iUserData);


_INT32 eos_msg_init();
_INT32 eos_msg_create(const _CHAR8 *pcName,EOS_MSG_ID*pstMsgId, eosMsgCallback pfCallBack,_VOID* iData);
_INT32 eos_msg_run(EOS_MSG_ID pstMsgId, _INT32 timeout);
_INT32 eos_msg_callon(const _CHAR8* cName,EOS_MSG_ID pstMsgId,_INT32 fd,eosMsgEvents sEvents,eosMsgCallback callbackFunc,_VOID* iData);
_INT32 eos_msg_send(EOS_MSG_ID pstMsgId,const _VOID* pBuf,EOSSize_t iLen,_VOID * pData);
_INT32 eos_msg_show(EOS_MSG_ID pstMsgId, EOS_INFO_PRINTF pPrintFunc,_VOID *pData);
_INT32 eos_msg_owntask_flag(EOS_MSG_ID pstMsgId,EOSBool** ppState);

_VOID eos_msg_debug(EOS_MSG_ID pstMsgId, _BOOL enabled);

#ifdef __cplusplus
}
#endif
#endif

