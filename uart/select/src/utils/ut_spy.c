
#ifdef __VXWORKS_OS__
#include <ioLib.h>
#include <spyLib.h>

#include <eos.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utils.h>
#include <ut_spy.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_SPY_MAX_TASKS    32

#ifndef STDOUT
#define STDOUT 1
#endif

typedef struct tagstUtilsSpyTimeSlot
{
    char valid;
    char usage;
    unsigned char ticks;
}ST_UT_SPY_TS;

typedef struct tagstUtilsSpyTask
{
    int tid;
    int name[16];

    int nSlots;     /* total time slot count */
    int nValids;     /* the valid time slot count */
    ST_UT_SPY_TS* tss;
}ST_UT_SPY_TASK;

typedef struct tagstUtilsSpy
{
    int bInitialized;
    int bRunning;
    int bShutdown;
    int bReport;
    
    int period;     /* the tiem slot period in ms */
    int nSlots;     /* the total time slot count */
    int iCurSlot;   /* the current time slot */
    
    int iTaskEnd;
    int nTasks;
    ST_UT_SPY_TASK tasks[UT_SPY_MAX_TASKS];

    int soRx;
    int soTx;
    
    int tTaskMain;
}ST_UT_SPY;

static ST_UT_SPY g_stUtSpy = {0};

static int utSpyCreateSocks(ST_UT_SPY* pSpy, int rxPort)
{
    struct sockaddr_in in_addr;
    int soLsn;

    memset(&in_addr, 0, sizeof(in_addr));
    
    in_addr.sin_family = AF_INET;
    in_addr.sin_addr.s_addr = htonl(0x7f000001);
    in_addr.sin_port = 0;
    
    pSpy->soTx = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pSpy->soTx == -1)
    {
        return -1;
    }

    in_addr.sin_port = htons(rxPort);
    
    soLsn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (soLsn == -1)
    {
        close(pSpy->soTx);
        return -2;
    }

    if (bind(soLsn, (struct sockaddr*)&in_addr, sizeof(in_addr)))
    {
        close(soLsn);
        close(pSpy->soTx);
        return -3;
    }

    if (listen(soLsn, 1))
    {
        close(soLsn);
        close(pSpy->soTx);
        return -4;
    }
    
    if (connect(pSpy->soTx, (struct sockaddr*)&in_addr, sizeof(in_addr)))
    {
        close(soLsn);
        close(pSpy->soTx);
        return -5;
    }

    if ((pSpy->soRx = accept(soLsn, NULL, 0)) < 0)
    {
        close(soLsn);
        close(pSpy->soTx);
        return -6;
    }

    close(soLsn);

    return 0;
}

ST_UT_SPY_TASK* utSpyFindTask(ST_UT_SPY* pSpy, int tid, const char* name)
{
    int i;

    if (tid != EOS_ERROR)
    {
        for (i = 1; i < UT_SPY_MAX_TASKS; i++)
        {
            if (tid == pSpy->tasks[i].tid)
            {
                return &pSpy->tasks[i];
            }
        }
    }
    else if (name != NULL)
    {
        for (i = 0; i < UT_SPY_MAX_TASKS; i++)
        {
            if (!strcmp(name, pSpy->tasks[i].name))
            {
                return &pSpy->tasks[i];
            }
        }
    }

    return NULL;
}

ST_UT_SPY_TASK* utSpyAddTask(ST_UT_SPY* pSpy, int tid, const char* name)
{
    ST_UT_SPY_TASK* pTask;
    int i;

    for (i = 0; i < UT_SPY_MAX_TASKS; i++)
    {
        if ((EOS_ERROR == pSpy->tasks[i].tid) 
            && ('\0' == pSpy->tasks[i].name[0]))
        {
            break;
        }
    }

    if (i == UT_SPY_MAX_TASKS)
    {
        return NULL;
    }

    pTask = &pSpy->tasks[i];
    
    if (NULL == pTask->tss)
    {
        pTask->tss = (ST_UT_SPY_TS*)UT_MALLOC(sizeof(ST_UT_SPY_TS)*pSpy->nSlots);
        if (NULL == pTask->tss)
        {
            return NULL;
        }
    }

    memset(pTask->tss, 0, sizeof(ST_UT_SPY_TS)*pSpy->nSlots);
    
    pTask->tid = tid;
    pTask->nValids = 0;
    pTask->nSlots = pSpy->nSlots;
    
    strncpy(pTask->name, name, sizeof(pTask->name));
    pTask->name[sizeof(pTask->name) - 1] = '\0';

    return pTask;
}

static void utSpyUpdateTaskActivity(ST_UT_SPY* pSpy, int tid, const char* name, int usage, int ticks)
{
    ST_UT_SPY_TASK* pTask;
    ST_UT_SPY_TS* pts;

    pTask = utSpyFindTask(pSpy, tid, name);
    if (NULL == pTask)
    {
        pTask = utSpyAddTask(pSpy, tid, name);
        if (NULL == pTask)
        {
            /* task is not enough */
            return;
        }
    }

    pts = &pTask->tss[(pSpy->iCurSlot + 1) % pSpy->nSlots];

    pTask->nValids -= pts->valid;

    pts->valid = 1;
    pts->usage = usage;
    pts->ticks = ticks;

    pTask->nValids++;

    return;
}

static void utSpyInitTaskActivity(ST_UT_SPY* pSpy)
{
    ST_UT_SPY_TASK* pTask;
    ST_UT_SPY_TS* pts;

    int i;

    for (i = 0; i < UT_SPY_MAX_TASKS; i++)
    {
        if (pSpy->tasks[i].tid != EOS_ERROR || pSpy->tasks[i].name[0] != '\0')
        {
            return &pSpy->tasks[i];
        }
    }

    return;
}

static void utSpyRecvReport(ST_UT_SPY* pSpy)
{
    char buf[4096];
    int len;

    while (1)
    {
        len = recv(pSpy->soRx, buf, sizeof(buf) - 1, 0);
        if (len > 0)
        {
            char* ptr = buf;

            buf[len] = 0;        

            while (ptr != NULL)
            {
                char name[32];
                int tid = -1;
                int usage = -1;
                int ticks = -1;

                name[0] = '\0';
                
                sscanf(ptr, " %31s %*s %x %*s %*s (%*d) %d%% (%d)", 
                    name, &tid, &usage, &ticks);
                if (tid >= 0 && usage >= 0 && ticks >= 0)
                {
                    /* get the cpu usage of one task */
                    utSpyUpdateTaskActivity(pSpy, tid, name, usage, ticks);
                }
                else if (!strcmp(name, "IDLE"))
                {
                    /* add the idle task */
                    sscanf(ptr, " %*s %*s (%*d) %d%% (%d)", &usage, &ticks);

                    utSpyUpdateTaskActivity(pSpy, EOS_ERROR, "IDLE", usage, ticks);
                }
                else if (!strcmp(name, "TOTAL"))
                {
                    /* the end, return */
                    pSpy->iCurSlot = (pSpy->iCurSlot + 1) % pSpy->nSlots;
                    return;
                }

                /* find next task */
                ptr = strchr(ptr+1, '\n');
            }
        }
    }
}

static void utSpyMain(ST_UT_SPY* pSpy)
{
    int iOldStdOut = ioTaskStdGet(0, STDOUT);
    int iDelayTicks = pSpy->period * sysClkRateGet() / 1000;

    pSpy->bRunning = 1;
    pSpy->bReport = 1;

    spyClkStart();
        
    while (!pSpy->bShutdown)
    {
        taskDelay(iDelayTicks);

        if (pSpy->bReport)
        {
            /* send the output of spyReport() to the RX socket*/
            ioTaskStdSet(0, STDOUT, pSpy->soTx);
            spyReport();
            ioTaskStdSet(0, STDOUT, iOldStdOut);

            /* receive report information from the TX socket */
            utSpyRecvReport(pSpy);
        }
    }

    spyClkStop();
    
    pSpy->bRunning = 0;
}

int ut_spy_init(int period, int time_slots)
{
    ST_UT_SPY* pSpy = &g_stUtSpy;
    int i = 0;

    if (pSpy->bInitialized)
    {
        return 0;
    }

    memset(pSpy, 0, sizeof(ST_UT_SPY));

    pSpy->period = period;
    pSpy->nSlots = time_slots;

    for (i = 0; i < UT_SPY_MAX_TASKS; i++)
    {
        pSpy->tasks[i].tid = EOS_ERROR;
    }

    utSpyAddTask(pSpy, EOS_ERROR, "IDLE");

    /* create the sock fd to send/recv the report information */
    if (utSpyCreateSocks(pSpy, 65432))
    {
        printf("create utils socket failed!\r\n");
        return -1;
    }

    pSpy->tTaskMain = taskSpawn("tUtSpyMain", 10, 0, 16*1024, 
                                utSpyMain, (_INT32) pSpy, 
                                0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (EOS_ERROR == pSpy->tTaskMain)
    {
        printf("create utils spy task failed!\r\n");
        return -2;
    }

    pSpy->bInitialized = 1;

    return 0;
}

int ut_spy_start(void)
{
    ST_UT_SPY* pSpy = &g_stUtSpy;

    pSpy->bReport = 1;

    return 0;
}

int ut_spy_stop(void)
{
    ST_UT_SPY* pSpy = &g_stUtSpy;

    pSpy->bReport = 0;

    return 0;
}

int ut_spy_get_cpu(int* usages, int time_slots)
{
    return 0;
}

int ut_spy_report(char* buf, int size)
{
    return 0;
}

#if defined(__cplusplus)
}
#endif

#endif

