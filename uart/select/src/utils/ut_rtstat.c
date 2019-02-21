
/******************************************************************************
 * Description: ut_rtstat.c, runtime statistics
 * Author: pengjian
 * Histroy: 1.00, 2006.06.06 created
 * CopyRight: Ejoin Tech. Ltd. Co, 2006.6~2006
******************************************************************************/

#include "eos.h"
#include "ut_debug.h"
#include "ut_queue.h"
#include "ut_mem.h"

#include "ut_rtstat.h"
#include "ut_rtstat.inc"

#if defined(__cplusplus)
extern "C" {
#endif

_UINT32 UTRTS_Construct(H_UT_RTS* phRts, 
                        const _CHAR8* pcFile,
                        _UINT32 uiLine,
                        FP_UT_RTS_PRINTF fpPrint,
                        _VOID* data)
{
    ST_UT_RTS* pstRts = NULL;    

    if (NULL == phRts)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    *phRts = NULL;
    
    pstRts = UT_TALLOC(ST_UT_RTS);
    if (NULL == pstRts)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return EOS_ERROR;
    }

    memset(pstRts, 0, sizeof(ST_UT_RTS));

#ifdef ANDROID
    pstRts->pcFile = utRtsRemovePath(pcFile);
#else
    pstRts->pcFile = pcFile;
#endif
    pstRts->uiLine = uiLine;

    pstRts->fpPrint = fpPrint ? fpPrint : (FP_UT_RTS_PRINTF) fprintf;
    pstRts->data = fpPrint ? data : (_VOID*)stdout;

    eos_lst_init(&pstRts->lsRtsEnts);
    eos_lst_init(&pstRts->lsEndEnts);
    eos_lst_init(&pstRts->lsEntQue);
    
    pstRts->mtx = semMCreate(0);
    if (NULL == pstRts->mtx)
    {
        UTRTS_Destruct(pstRts);
        
        UT_ERROR(EOS_ERROR, "UTRTS_Construct: create mutex failed!\r\n");
        return EOS_ERROR;
    }

    *phRts = pstRts;

    return EOS_OK;
}

_UINT32 UTRTS_Destruct(H_UT_RTS hRts)
{
    if (hRts != NULL)
    {
        ST_UT_RTS* pstRts = (ST_UT_RTS*)hRts;    

        eos_lst_uninit(&pstRts->lsEntQue, (EOS_LSTFREE_FUNC) utRtsQueDestroy);
        eos_lst_uninit(&pstRts->lsEndEnts, (EOS_LSTFREE_FUNC) utRtsNodeDestroy);
        eos_lst_uninit(&pstRts->lsRtsEnts, (EOS_LSTFREE_FUNC) utRtsNodeDestroy);

        if (pstRts->mtx != NULL)
        {
            semDelete(pstRts->mtx);
        }
        
        UT_FREE(hRts);
    }

    return EOS_OK;
}

_UINT32 UTRTS_Begin(H_UT_RTS hRts, const _CHAR8* file, _INT32 line)
{
    ST_UT_RTS* pstRts = (ST_UT_RTS*)hRts;    
    ST_UT_RTS_NODE* pstRtsEnt;
    ST_UT_RTS_QUE* pQue;
    
    if (NULL == pstRts)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

#ifdef ANDROID
    file = utRtsRemovePath(file);
#endif

    pstRtsEnt = utRtsFindNode(pstRts, file, line);
    if (NULL == pstRtsEnt)
    {
        pstRtsEnt = utRtsNodeCreate(file, line);
        if (NULL == pstRtsEnt)
        {
            return EOS_ERROR;
        }

        utRtsAddNode(pstRts, pstRtsEnt);
    }

    pQue = utRtsFindQue(pstRts);
    if (NULL == pQue)
    {
        pQue = utRtsQueCreate();
        if (NULL == pQue)
        {
            return EOS_ERROR;
        }

        utRtsAddQue(pstRts, pQue);
    }

    if (EN_UT_RTS_STATE_IDLE == pstRtsEnt->enState)
    {
        UT_QueuePush(pQue->que, pstRtsEnt);
        
        pstRtsEnt->enState = EN_UT_RTS_STATE_WORKING;
    }
    else /* if (EN_UT_RTS_STATE_WORKING == pstRtsEnt->enState) */
    {
        UT_QueuePopIf(pQue->que, pstRtsEnt, (_BOOL (*)(_VOID*, _VOID*))utRtsNodeEqu);
        UT_QueuePush(pQue->que, pstRtsEnt);

        if (!pstRtsEnt->bReBeginPrompt)     /* only prompt once */
        {
            pstRtsEnt->bReBeginPrompt = EOS_TRUE;
            
            UT_RTS_PRINTF(pstRts, "%s:%d is working! last_begin = %d, now = %ld\r\n", 
                pstRts->pcFile, line, pstRtsEnt->iBeginTicks, tickGet());
        }
    }

    pstRtsEnt->iBeginTicks = tickGet();

    return EOS_OK;
}

_UINT32 UTRTS_End(H_UT_RTS hRts, const _CHAR8* file, _INT32 line)
{
    _UINT32 uiTicks = tickGet();
    
    ST_UT_RTS* pstRts = (ST_UT_RTS*)hRts;   
    ST_UT_RTS_NODE* pstRtsEnt;
    ST_UT_RTS_NODE* pstEndEnt;
    ST_UT_RTS_QUE* pQue;
    
    if (NULL == pstRts)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pQue = utRtsFindQue(pstRts);
    if (NULL == pQue)
    {
        UT_ERROR(EOS_ERROR, "queue not found!\r\n");
        return EOS_ERROR;
    }

    pstRtsEnt = UT_QueuePop(pQue->que);
    if (NULL == pstRtsEnt)
    {
        UT_ERROR(EOS_ERROR, "UTRTS_Begin() not invoked!\r\n");
        return EOS_ERROR;
    }

#ifdef ANDROID
    file = utRtsRemovePath(file);
#endif

    pstEndEnt = utRtsFindEndNode(pstRts, file, pstRtsEnt->uiBeginLine, line);
    if (NULL == pstEndEnt)
    {
        pstEndEnt = utRtsNodeCreate(file, pstRtsEnt->uiBeginLine);
        if (NULL == pstEndEnt)
        {
            return EOS_ERROR;
        }

        pstEndEnt->uiEndLine = line;
        
        utRtsAddEndNode(pstRts, pstEndEnt);
    }

    pstEndEnt->iBeginTicks = pstRtsEnt->iBeginTicks;
    pstEndEnt->iLastTicks = uiTicks - pstEndEnt->iBeginTicks;
    pstEndEnt->iTotTicks += pstEndEnt->iLastTicks;
    pstEndEnt->count++;
    
    if (pstEndEnt->iLastTicks > pstEndEnt->iMaxTicks)
    {
        pstEndEnt->iMaxTicks = pstEndEnt->iLastTicks;
        
        UT_RTS_PRINTF(pstRts, "%s:%d-%d taken %d ticks!\r\n", 
            pstRts->pcFile, pstEndEnt->uiBeginLine, pstEndEnt->uiEndLine, pstEndEnt->iMaxTicks);
    }

    pstRtsEnt->enState = EN_UT_RTS_STATE_IDLE;

    return EOS_OK;
}

_VOID UTRTS_Show(H_UT_RTS hRts, FP_UT_RTS_PRINTF fpPrint, const _VOID* data)
{
    ST_UT_RTS* pstRts = (ST_UT_RTS*)hRts;
    _NODE* pn;
    
    if (NULL == hRts)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return;
    }
    
    if (NULL == fpPrint)
    {
        fpPrint = pstRts->fpPrint;
        data = pstRts->data;
    }

    fpPrint(data, "%s:%d\r\n", pstRts->pcFile, pstRts->uiLine);

    semTake(pstRts->mtx, WAIT_FOREVER);

    pn = eos_lst_first(&pstRts->lsRtsEnts);
    while (pn != NULL)
    {
        ST_UT_RTS_NODE* pstRtsEnt = (ST_UT_RTS_NODE*)pn;

        pn = eos_lst_next(pn);
        
        if (EN_UT_RTS_STATE_WORKING == pstRtsEnt->enState)
        {
            fpPrint(data, "\t%s:%d-%d RUNNING, last=%d\r\n", 
                pstRtsEnt->pcFile, pstRtsEnt->uiBeginLine, pstRtsEnt->uiEndLine, pstRtsEnt->iLastTicks);
        }
    }

    fpPrint(data, "\r\n");

    pn = eos_lst_first(&pstRts->lsEndEnts);
    while (pn != NULL)
    {
        ST_UT_RTS_NODE* pstRtsEnt = (ST_UT_RTS_NODE*)pn;

        pn = eos_lst_next(pn);

        fpPrint(data, "\t%s:%d-%d: tot=%d, avg=%d, max=%d\r\n", 
            pstRtsEnt->pcFile, pstRtsEnt->uiBeginLine, pstRtsEnt->uiEndLine, 
            pstRtsEnt->iTotTicks, pstRtsEnt->iTotTicks/pstRtsEnt->count, pstRtsEnt->iMaxTicks);
    }

    semGive(pstRts->mtx);

    fpPrint(data, "\r\n");
}

ST_UT_RTS_NODE* utRtsFindNode(ST_UT_RTS* pstRts, const _CHAR8* file, _INT32 line)
{
    _INT32 tid = taskIdSelf();
    ST_UT_RTS_NODE* prn = NULL;

    semTake(pstRts->mtx, WAIT_FOREVER);

    prn = (ST_UT_RTS_NODE*)eos_lst_first(&pstRts->lsRtsEnts);
    while (prn != NULL)
    {
        if (tid == prn->tid && file == prn->pcFile && line == prn->uiBeginLine)
        {
            break;
        }

        prn = (ST_UT_RTS_NODE*)eos_lst_next(&prn->node);
    }

    semGive(pstRts->mtx);

    return prn;
}

ST_UT_RTS_NODE* utRtsFindEndNode(ST_UT_RTS* pstRts, const _CHAR8* file, _UINT32 line, _UINT32 end_line)
{
    _INT32 tid = taskIdSelf();
    ST_UT_RTS_NODE* prn = NULL;

    semTake(pstRts->mtx, WAIT_FOREVER);

    prn = (ST_UT_RTS_NODE*)eos_lst_first(&pstRts->lsEndEnts);
    while (prn != NULL)
    {
        if (tid == prn->tid && file == prn->pcFile 
            && line == prn->uiBeginLine && end_line == prn->uiEndLine)
        {
            break;
        }

        prn = (ST_UT_RTS_NODE*)eos_lst_next(&prn->node);
    }

    semGive(pstRts->mtx);

    return prn;
}

ST_UT_RTS_QUE* utRtsFindQue(ST_UT_RTS* pstRts)
{
    _INT32 tid = taskIdSelf();
    ST_UT_RTS_QUE* pQue = NULL;

    semTake(pstRts->mtx, WAIT_FOREVER);

    pQue = (ST_UT_RTS_QUE*)eos_lst_first(&pstRts->lsEntQue);
    while (pQue != NULL)
    {
        if (tid == pQue->tid)
        {
            break;
        }

        pQue = (ST_UT_RTS_QUE*)eos_lst_next(&pQue->node);
    }

    semGive(pstRts->mtx);

    return pQue;
}

_VOID utRtsAddNode(ST_UT_RTS* pstRts, ST_UT_RTS_NODE* prn)
{
    semTake(pstRts->mtx, WAIT_FOREVER);

    eos_lst_add(&pstRts->lsRtsEnts, &prn->node);

    semGive(pstRts->mtx);
}

_VOID utRtsAddEndNode(ST_UT_RTS* pstRts, ST_UT_RTS_NODE* prn)
{
    semTake(pstRts->mtx, WAIT_FOREVER);

    eos_lst_add(&pstRts->lsEndEnts, &prn->node);

    semGive(pstRts->mtx);
}

_VOID utRtsAddQue(ST_UT_RTS* pstRts, ST_UT_RTS_QUE* pQue)
{
    semTake(pstRts->mtx, WAIT_FOREVER);

    eos_lst_add(&pstRts->lsEntQue, &pQue->node);

    semGive(pstRts->mtx);
}

ST_UT_RTS_NODE* utRtsNodeCreate(const _CHAR8* file, _INT32 line)
{
    ST_UT_RTS_NODE* prn;
    
    prn = UT_TALLOC(ST_UT_RTS_NODE);
    if (NULL == prn)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return NULL;
    }
    
    memset(prn, 0, sizeof(ST_UT_RTS_NODE));

    prn->tid = taskIdSelf();
    prn->pcFile = file;
    prn->uiBeginLine = line;

    return prn;
}

_VOID utRtsNodeDestroy(ST_UT_RTS_NODE* pRtsNode)
{
    UT_FREE(pRtsNode);
}

ST_UT_RTS_QUE* utRtsQueCreate(_VOID)
{
    ST_UT_RTS_QUE* pQue;
    
    pQue = UT_TALLOC(ST_UT_RTS_QUE);
    if (NULL == pQue)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return NULL;
    }

    pQue->tid = taskIdSelf();

    if (UT_QueueConstruct(&pQue->que, EN_UT_QUEUE_MODE_LIFO))
    {
        UT_FREE(pQue);

        UT_ERROR(EOS_ERROR, "utRtsQueCreate: create queue failed!\r\n");
        return NULL;
    }

    return pQue;
}

_VOID utRtsQueDestroy(ST_UT_RTS_QUE* pQue)
{
    if (pQue->que != NULL)
    {
        UT_QueueDestruct(pQue->que);
    }

    UT_FREE(pQue->que);
}

_BOOL utRtsNodeEqu(ST_UT_RTS_NODE* pNode1, ST_UT_RTS_NODE* pNode2)
{
    return ((pNode1->pcFile == pNode2->pcFile) && (pNode1->uiBeginLine == pNode2->uiBeginLine));
}

#ifdef ANDROID

const _CHAR8* utRtsRemovePath(const _CHAR8* filename)
{
    const _CHAR8* tmp = eos_strrchr(filename, '\\');
    
    if (NULL == tmp)
    {
        tmp = eos_strrchr(filename, '/');
    }

    return tmp ? (tmp + 1) : filename;
}

#endif

#if defined(__cplusplus)
}
#endif

