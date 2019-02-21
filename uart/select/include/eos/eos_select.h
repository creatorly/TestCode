#ifndef __EOS_SELECT_H
#define __EOS_SELECT_H


#if defined(__cplusplus)
extern "C" {
#endif

typedef _USHORT16 eosSelectEvents;

typedef enum
{
    EOS_SELECT_READ = 0,
    EOS_SELECT_WRITE,
    EOS_SELECT_CONNECT,
    EOS_SELECT_ACCEPT,
    EOS_SELECT_CLOSE,

    EOS_SELECT_EVENT_MAX,
}EN_EOS_SELECT_EVENT;

#define eosSelectRead        EOS_SELECT_READ
#define eosSelectWrite       EOS_SELECT_WRITE
#define eosSelectAccept      EOS_SELECT_ACCEPT
#define eosSelectConnect     EOS_SELECT_CONNECT
#define eosSelectClose       EOS_SELECT_CLOSE

/* Set easy to use 64 bit equivalents */
#define EOS_TIME64_NSECPERUSEC (1000)
#define EOS_TIME64_NSECPERMSEC (1000000)
#define EOS_TIME64_NSECPERSEC (1000000000)

#define EOS64Multiply( a , b ) (a*b)
#define EOS64Modulu( a , b )   (a%b)
#define EOS64Divide( a , b )   (a/b)



typedef struct EOSSelectEngineInternal eosSelectEngine;

typedef struct eosSelectFdInternal eosSelectFd;


typedef _VOID  (*eosSelectCb)(
        eosSelectEngine*  selectEngine,
        eosSelectFd*      fd,
        eosSelectEvents   selectEvent,
        EOSBool           error);

struct eosSelectFdInternal
{
    eosSocket        fd; /* File descriptor for this event */
    eosSelectCb      callback; /* Callback function - should be type casted when used */
    eosSelectEvents  events; /* Events we're waiting on - needed for translation */
    _USHORT16        usPads; /* Events we're waiting on - needed for translation */
    eosSelectFd*     nextInBucket; /* Next file descriptor in the same bucket */
};

struct EOSSelectEngineInternal
{
    _UINT32        maxFd; /* Maximum number of fd's supported */
    _UINT32        maxFdInSelect; /* Maximum fd set in the fdset's of the select() */
    fd_set          rdSet; /* Read fd's we're currently waiting for */
    fd_set          wrSet; /* Write fd's we're currently waiting for */
    fd_set          tmpRdSet; /* Read fd's given to select() */
    fd_set          tmpWrSet; /* Write fd's given to select() */
    EOSBool        waiting; /* RV_TRUE if we're blocked in a select() call */

    eosSelectFd      preemptionFd;
    eosSocket        preemptionSocket; /* Socket used to preempt a select() call of the thread */
    eosAddress       localAddress; /* Address we're listening on for preemption */


    /* Bucket-hash parameters */
    _UINT32        maxHashSize; /* Size of the bucket hash to use */
    eosSelectFd**   hash; /* Bucket hash to use for the file descriptors */
    EOS_SEM_ID     hashLock; /* Lock to use for bucket hash access synchronization */

    EOS_SEM_ID     lock; /* General lock. Used for updating the list of file descriptors we're
                             currently interested in */
};

_INT32 eos_select_init();
_INT32 eos_select_end();
_INT32 eos_select_construct(eosSelectEngine* selectEngine,_UINT32 maxHashSize);
_INT32 eos_select_destruct(eosSelectEngine* selectEngine);
_INT32 eos_fd_construct(eosSelectFd*  fd,eosSocket* s);
_INT32 eos_fd_destruct(eosSelectFd* fd);
_INT32 eos_select_add(eosSelectEngine* selectEngine,eosSelectFd* fd,eosSelectEvents selectEvents,eosSelectCb eventsCb);
_INT32 eos_select_remove(eosSelectEngine* selectEngine,eosSelectFd* fd);
_INT32 eos_select_update(eosSelectEngine* selectEngine,
    eosSelectFd*          fd,
    eosSelectEvents       selectEvents,
    eosSelectCb           eventsCb);
_INT32 eos_select_wait_and_block(eosSelectEngine* selectEngine,_ULLONG64 nsecTimeout);
eosSocket* eos_fd_get_socket(eosSelectFd* fd);
eosSelectEvents eos_select_get_events(eosSelectFd* fd);
eosSelectFd* eos_select_find_fd(eosSelectEngine* selectEngine,eosSocket* s);
__CONST__ _CHAR8* eos_fd_get_event_str(eosSelectEvents ev);
_INT32 eos_select_stop_waiting(eosSelectEngine* selectEngine);

#ifdef __cplusplus
}
#endif
#endif




















