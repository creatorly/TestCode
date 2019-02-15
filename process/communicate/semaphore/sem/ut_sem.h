#ifndef __UT_SEM_H
#define __UT_SEM_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstUtSem* SEM_ID;

#define    UT_WAIT_FOREVER         -1
#define    UT_NO_WAIT              0

#ifndef WAIT_FOREVER
#define WAIT_FOREVER    UT_WAIT_FOREVER

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

