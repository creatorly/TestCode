#ifndef __EOS_SEM_H
#define __EOS_SEM_H


#if defined(__cplusplus)
extern "C" {
#endif

#if (EJOIN_OS_TYPE != EJOIN_OS_TYPE_VXWORKS)
typedef struct tagstEosSem* SEM_ID;
#endif

typedef SEM_ID  EOS_SEM_ID;

#define    EOS_WAIT_FOREVER         -1
#define    EOS_NO_WAIT              0

_INT32 eos_sem_init();
_INT32 eos_sem_create(__CONST__ _CHAR8 * pcName, EOS_SEM_ID* pSemID);
_INT32 eos_sem_delete(EOS_SEM_ID hSemID);
_INT32 eos_sem_p(EOS_SEM_ID hSemID,_UINT32 ulWaitTime);
_INT32 eos_sem_v(EOS_SEM_ID hSemID);
_INT32 eos_sem_show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

#define eos_sem_trace_p(semid, wt) \
    eos_printf("%s:%d: eos_sem_p ... ", __FILE__, __LINE__); \
    fflush(stdout); \
    eos_sem_p(semid, wt); \
    eos_printf("OK\r\n")
    
#define eos_sem_trace_v(semid) \
    eos_printf("%s:%d: eos_sem_v ... ", __FILE__, __LINE__); \
    fflush(stdout); \
    eos_sem_v(semid); \
    eos_printf("OK\r\n")

#if (EJOIN_OS_TYPE != EJOIN_OS_TYPE_VXWORKS)

#ifndef WAIT_FOREVER
#define WAIT_FOREVER    EOS_WAIT_FOREVER
#endif

/* only for port, not imeplement */
#define SEM_Q_FIFO              0x0
#define SEM_Q_PRIORITY          0x1
#define SEM_DELETE_SAFE         0x4
#define SEM_INVERSION_SAFE      0x8
#define SEM_EVENTSEND_ERR_NOTIFY    0x10

typedef enum        /* SEM_B_STATE */
{
    SEM_EMPTY,      /* 0: semaphore not available */
    SEM_FULL        /* 1: semaphore available */
} SEM_B_STATE;

SEM_ID semMCreate(int options);
SEM_ID semCCreate(int options, int initialCount);
SEM_ID semBCreate(int options, SEM_B_STATE initialState);

int semDelete(SEM_ID semId);

/**
 * This routine performs the take operation on a specified semaphore.
 * @param semId:    semaphore ID to take 
 * @param timeout:  timeout in ticks
 *
 * @return: OK, or ERROR if the semaphore ID is invalid or the task timed out.
 */
int semTake(SEM_ID semId, int timeout);

int semGive(SEM_ID semId);

/* only support binary or counting semphore */
int semClear(SEM_ID semId);

#endif

#if defined(__cplusplus)
}
#endif 
#endif

