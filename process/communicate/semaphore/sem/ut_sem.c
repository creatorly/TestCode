#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <time.h>
#include "ut_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UT_SEM_TYPE_MUTEX = 0,
    UT_SEM_TYPE_BINARY,
    UT_SEM_TYPE_COUNT,
}EN_UT_SEM_TYPE;

typedef struct tagstUtSemM
{
    pthread_mutex_t mutex;
}ut_sem_m_t;

typedef struct tagstUtSemC
{
    sem_t sem;
}ut_sem_c_t;

typedef struct tagstUtSemB
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}ut_sem_b_t;

typedef struct tagstUtSem
{
    EN_UT_SEM_TYPE type;

    union
    {
        ut_sem_m_t m;
        ut_sem_c_t c;
        ut_sem_b_t b;
    }u;
}ut_sem_t;

void utSemGetTimeSpec(struct timespec* ts, int offset_ms)
{
    struct timeval now;
    
    gettimeofday(&now, NULL);

    offset_ms = (offset_ms > 1) ? (offset_ms - 1) : 1;
    
    ts->tv_sec = now.tv_sec + offset_ms/1000;
    ts->tv_nsec = (now.tv_usec + (offset_ms%1000)*1000) * 1000;
    
    if (ts->tv_nsec >= 1000*1000*1000)
    {
        ts->tv_sec++;
        ts->tv_nsec -= 1000*1000*1000;
    }
}

SEM_ID semMCreate(int options)
{
    ut_sem_t* pSem;

    pSem = (ut_sem_t*)malloc(sizeof(ut_sem_t));
    if (NULL == pSem)
    {
        return NULL;
    }

    pSem->type = UT_SEM_TYPE_MUTEX;

    if (pthread_mutex_init(&pSem->u.m.mutex, NULL))
    {
        free(pSem);
        return NULL;
    }

    return pSem;
}

SEM_ID semCCreate(int options, int initialCount)
{
    ut_sem_t* pSem;

    pSem = (ut_sem_t*)malloc(sizeof(ut_sem_t));
    if (NULL == pSem)
    {
        return NULL;
    }

    pSem->type = UT_SEM_TYPE_COUNT;

    if (sem_init(&pSem->u.c.sem, 0, initialCount))
    {
        free(pSem);
        return NULL;
    }

    return pSem;
}

SEM_ID semBCreate(int options, SEM_B_STATE initialState)
{
    ut_sem_t* pSem;

    pSem = (ut_sem_t*)malloc(sizeof(ut_sem_t));
    if (NULL == pSem)
    {
        return NULL;
    }

    pSem->type = UT_SEM_TYPE_BINARY;

    if (pthread_mutex_init(&pSem->u.b.mutex, NULL))
    {
        free(pSem);
        return NULL;
    }

    if (pthread_cond_init(&pSem->u.b.cond, NULL))
    {
        pthread_mutex_destroy(&pSem->u.b.mutex);
        free(pSem);
        return NULL;
    }

    return pSem;
}

int semDelete(SEM_ID semId)
{
    if (UT_SEM_TYPE_MUTEX == semId->type)
    {
        pthread_mutex_destroy(&semId->u.m.mutex);
    }
    else if (UT_SEM_TYPE_COUNT == semId->type)
    {
        sem_destroy(&semId->u.c.sem);
    }
    else
    {
        pthread_cond_destroy(&semId->u.b.cond);
        pthread_mutex_destroy(&semId->u.b.mutex);
    }

    free(semId);

    return 0;
}

/**
 * This routine performs the take operation on a specified semaphore.
 * @param semId:    semaphore ID to take 
 * @param timeout:  timeout in ticks
 *
 * @return: OK, or ERROR if the semaphore ID is invalid or the task timed out.
 */
int semTake(SEM_ID semId, int timeout)
{
    int ret;
    
    if (UT_SEM_TYPE_MUTEX == semId->type)
    {
        ret = pthread_mutex_lock(&semId->u.m.mutex);
    }
    else if (UT_SEM_TYPE_COUNT == semId->type)
    {
        if (timeout > 0)
        {
            struct timespec ts;
            
            utSemGetTimeSpec(&ts, timeout);

            ret = sem_timedwait(&semId->u.c.sem, &ts);
        }
        else if (timeout < 0)
        {
            ret = sem_wait(&semId->u.c.sem);
        }
        else
        {
            ret = sem_trywait(&semId->u.c.sem);
        }
    }
    else /* UT_SEM_TYPE_BINARY */
    {
        if (timeout >= 0)
        {
            struct timespec ts;
            
            utSemGetTimeSpec(&ts, timeout);

            pthread_mutex_lock(&semId->u.b.mutex);
            ret = pthread_cond_timedwait(&semId->u.b.cond, &semId->u.b.mutex, &ts);
            pthread_mutex_unlock(&semId->u.b.mutex);
        }
        else
        {
            pthread_mutex_lock(&semId->u.b.mutex);
            ret = pthread_cond_wait(&semId->u.b.cond, &semId->u.b.mutex);
            pthread_mutex_unlock(&semId->u.b.mutex);
        }
    }

    return ret;
}

int semGive(SEM_ID semId)
{
    int ret;
    
    if (UT_SEM_TYPE_MUTEX == semId->type)
    {
        ret = pthread_mutex_unlock(&semId->u.m.mutex);
    }
    else if (UT_SEM_TYPE_COUNT == semId->type)
    {
        ret = sem_post(&semId->u.c.sem);
    }
    else /* UT_SEM_TYPE_BINARY */
    {
        pthread_mutex_lock(&semId->u.b.mutex);
        ret = pthread_cond_signal(&semId->u.b.cond);
        pthread_mutex_unlock(&semId->u.b.mutex);
    }

    return ret;
}

int semClear(SEM_ID semId)
{
    int ret = 0;
    
    if (UT_SEM_TYPE_BINARY == semId->type)
    {
        struct timespec ts;
        
        utSemGetTimeSpec(&ts, 0);
        
        pthread_mutex_lock(&semId->u.b.mutex);
        ret = pthread_cond_timedwait(&semId->u.b.cond, &semId->u.b.mutex, &ts);
        pthread_mutex_unlock(&semId->u.b.mutex);
    }
    else if (UT_SEM_TYPE_COUNT == semId->type)
    {
        ret = sem_trywait(&semId->u.c.sem);
    }

    return ret;
}

#ifdef __cplusplus
}
#endif

