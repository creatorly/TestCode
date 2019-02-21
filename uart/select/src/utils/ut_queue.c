
/******************************************************************************
 * Description: ut_queue.c, queue
 * Author: pengjian
 * Histroy: 1.00, 2006.06.09 created
 * CopyRight: Ejoin Tech. Ltd. Co, 2006.6~2006
******************************************************************************/

#include "eos.h"
#include "ut_debug.h"
#include "ut_mem.h"

#include "ut_queue.h"
#include "ut_queue.inc"

#if defined(__cplusplus)
extern "C" {
#endif

_UINT32 UT_QueueConstruct(H_UT_QUEUE* phQue, EN_UT_QUEUE_MODE enMode)
{
    ST_UT_QUEUE* pstQue = NULL;    

    if ((NULL == phQue) || (enMode >= EN_UT_QUEUE_MODE_BUTT))
    {
        UT_ERROR(EOS_ERROR, "invalid parameter passed!\r\n");
        return EOS_ERROR;
    }

    *phQue = NULL;
    
    pstQue = (ST_UT_QUEUE*)UT_MALLOC(sizeof(ST_UT_QUEUE));
    if (NULL == pstQue)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return EOS_ERROR;
    }

    memset(pstQue, 0, sizeof(ST_UT_QUEUE));
    pstQue->enMode = enMode;
    
    eos_lst_init(&pstQue->lsEles);
    eos_lst_init(&pstQue->lsFreeNodes);

    *phQue = pstQue;

    return EOS_OK;
}

_UINT32 UT_QueueDestruct(H_UT_QUEUE hQue)
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;    

    if (NULL == hQue)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    eos_lst_uninit(&pstQue->lsEles, (EOS_LSTFREE_FUNC)utQueNodeFree);
    eos_lst_uninit(&pstQue->lsFreeNodes, (EOS_LSTFREE_FUNC)utQueNodeFree);
    
    memset(pstQue, 0, sizeof(ST_UT_QUEUE));
    UT_FREE(pstQue);

    return EOS_OK;
}

#ifdef UT_QUE_DEBUG
_UINT32 UT_QueuePushEx(H_UT_QUEUE hQue, _VOID* data, const _CHAR8* file, _INT32 line)
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;    
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;

    pstQueEnt = (ST_UT_QUEUE_NODE*) eos_lst_get(&pstQue->lsFreeNodes);
    if (NULL == pstQueEnt)
    {
    #ifdef UT_MEM_DEBUG
        pstQueEnt = (ST_UT_QUEUE_NODE*) UT_MemAlloc(sizeof(ST_UT_QUEUE_NODE), file, line);
    #else
        pstQueEnt = (ST_UT_QUEUE_NODE*) UT_MALLOC(sizeof(ST_UT_QUEUE_NODE));
    #endif
        if (NULL == pstQueEnt)
        {
            UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
            return EOS_ERROR;
        }
    }

    memset(pstQueEnt, 0, sizeof(ST_UT_QUEUE_NODE));
    pstQueEnt->data = data;
    eos_lst_add(&pstQue->lsEles, &pstQueEnt->node);
    
    return EOS_OK;
}

#else
_UINT32 UT_QueuePush(H_UT_QUEUE hQue, _VOID* data)
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;    
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;

    pstQueEnt = (ST_UT_QUEUE_NODE*) eos_lst_get(&pstQue->lsFreeNodes);
    if (NULL == pstQueEnt)
    {
        pstQueEnt = (ST_UT_QUEUE_NODE*) UT_MALLOC(sizeof(ST_UT_QUEUE_NODE));
        if (NULL == pstQueEnt)
        {
            UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
            return EOS_ERROR;
        }
    }

    pstQueEnt->data = data;
    eos_lst_add(&pstQue->lsEles, &pstQueEnt->node);
    
    return EOS_OK;
}
#endif

_VOID* UT_QueuePop(H_UT_QUEUE hQue)
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;   
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;
    _VOID* data = NULL;
    
    if (EN_UT_QUEUE_MODE_FIFO == pstQue->enMode)
    {
        pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_first(&pstQue->lsEles);
    }
    else
    {
        pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_last(&pstQue->lsEles);
    }

    if (pstQueEnt != NULL)
    {
        data = pstQueEnt->data;
        
        eos_lst_del(&pstQue->lsEles, &pstQueEnt->node);
        eos_lst_add(&pstQue->lsFreeNodes, &pstQueEnt->node);
    }
    
    return data;
}

_INT32 UT_QueueFind(H_UT_QUEUE hQue, _VOID* data)
{
    _INT32 i = 0;
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;   
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;

    UT_ASSERT(hQue);

    pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_first(&pstQue->lsEles);
    for (; NULL!=pstQueEnt; i++)
    {
        if (data == pstQueEnt->data)
        {
            return i;
        }
        
        pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_next(&pstQueEnt->node);
    }

    return -1;
}

_VOID* UT_QueueGet(H_UT_QUEUE hQue, _UINT32 uiPos)
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;   
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;
    _BOOL bForward = EOS_FALSE;
    _UINT32 uiSize = 0;

    UT_ASSERT(hQue);

    uiSize = eos_lst_count(&pstQue->lsEles);
    UT_CONDCHK(uiPos < uiSize, NULL, "beyond the queue size!\r\n");

    bForward = (EN_UT_QUEUE_MODE_FIFO == pstQue->enMode);
    if (uiPos > uiSize/2)
    {
        bForward = (EN_UT_QUEUE_MODE_LIFO == pstQue->enMode);
        uiPos = uiSize - 1 - uiPos;
    }

    if (EOS_TRUE == bForward)
    {
        pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_first(&pstQue->lsEles);
        while (uiPos--)
        {
            pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_next(&pstQueEnt->node);
        }
    }
    else
    {
        pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_last(&pstQue->lsEles);
        while (uiPos--)
        {
            pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_prev(&pstQueEnt->node);
        }
    }
    
    return pstQueEnt->data;
}

_VOID* UT_QueuePopIf(H_UT_QUEUE hQue, _VOID* data, 
    _BOOL (*fpCmp)(_VOID* ele, _VOID* data))
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;
    ST_UT_QUEUE_NODE* pstNode = NULL;
    
    UT_ASSERT(hQue);

    if (NULL == fpCmp)
    {
        return UT_QueuePop(pstQue);
    }

    if (EN_UT_QUEUE_MODE_FIFO == pstQue->enMode)
    {
        pstNode = (ST_UT_QUEUE_NODE*)eos_lst_first(&pstQue->lsEles);

        while (pstNode != NULL)
        {
            if (fpCmp(pstNode->data, data))
            {
                _VOID* ptr = pstNode->data;
                
                eos_lst_del(&pstQue->lsEles, &pstNode->node);
                eos_lst_add(&pstQue->lsFreeNodes, &pstNode->node);

                return ptr;
            }
            
            pstNode = (ST_UT_QUEUE_NODE*)eos_lst_next(&pstNode->node);
        }
    }
    else
    {
        pstNode = (ST_UT_QUEUE_NODE*)eos_lst_last(&pstQue->lsEles);

        while (pstNode != NULL)
        {
            if (fpCmp(pstNode->data, data))
            {
                _VOID* ptr = pstNode->data;
                
                eos_lst_del(&pstQue->lsEles, &pstNode->node);
                eos_lst_add(&pstQue->lsFreeNodes, &pstNode->node);

                return ptr;
            }

            pstNode = (ST_UT_QUEUE_NODE*)eos_lst_prev(&pstNode->node);
        }
    }

    return NULL;
}

_VOID* UT_QueueFindIf(H_UT_QUEUE hQue, _VOID* data, 
    _BOOL (*fpCmp)(_VOID* ele, _VOID* data))
{
    ST_UT_QUEUE_NODE* pstNode = NULL;
    
    UT_ASSERT(hQue);

    pstNode = (ST_UT_QUEUE_NODE*)eos_lst_first(&((ST_UT_QUEUE*)hQue)->lsEles);
    while (pstNode)
    {
        if (fpCmp(pstNode->data, data))
        {
            return pstNode->data;
        }

        pstNode = (ST_UT_QUEUE_NODE*)eos_lst_next(&pstNode->node);
    }

    return NULL;
}

_UINT32 UT_QueueInsertIf(H_UT_QUEUE hQue, _VOID* data, 
    _BOOL (*fpCmp)(_VOID* ele, _VOID* data))
{
    _NODE* pstNode = NULL;
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;    
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;

    if (NULL == hQue)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstQueEnt = (ST_UT_QUEUE_NODE*)UT_MALLOC(sizeof(ST_UT_QUEUE_NODE));
    if (NULL == pstQueEnt)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return EOS_ERROR;
    }

    memset(pstQueEnt, 0, sizeof(ST_UT_QUEUE_NODE));
    pstQueEnt->data = data;

    pstNode = eos_lst_first(&pstQue->lsEles);
    while (pstNode)
    {
        if (fpCmp(((ST_UT_QUEUE_NODE*)pstNode)->data, data))
        {
            eos_lst_insert(&pstQue->lsEles, pstNode->pstPrev, &pstQueEnt->node);
            return EOS_OK;
        }

        pstNode = eos_lst_next(pstNode);
    }

    eos_lst_add(&pstQue->lsEles, &pstQueEnt->node);
    return EOS_OK;
}

_UINT32 UT_QueueSize(H_UT_QUEUE hQue)
{
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;   
    
    if (NULL == hQue)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    return eos_lst_count(&pstQue->lsEles);
}

_VOID UT_QueueShow(H_UT_QUEUE hQue, EOS_INFO_PRINTF fpPrint, _VOID* data)
{
    _UINT32 i = 0;
    ST_UT_QUEUE* pstQue = (ST_UT_QUEUE*)hQue;
    ST_UT_QUEUE_NODE* pstQueEnt = NULL;
    _CHAR8 acInfo[128];
    
    if (NULL == hQue)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return;
    }
    
    if (NULL == fpPrint)
    {
        fpPrint = EOS_DEFAULT_INFO_PRINT_FUNC;
        data = NULL;
    }

    sprintf(acInfo, "queue statistics: size(%d)\r\n", eos_lst_count(&pstQue->lsEles));
    fpPrint(data, acInfo);
    
    pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_first(&pstQue->lsEles);
    while (NULL != pstQueEnt)
    {
        sprintf(acInfo, "0x%08x ", (_UINT32)pstQueEnt->data);
        fpPrint(data, acInfo);
        if (0 == (++i & 3))
        {
            fpPrint(data, "\r\n");
        }

        pstQueEnt = (ST_UT_QUEUE_NODE*)eos_lst_next(&pstQueEnt->node);
    }

    if (i & 3)
    {
        fpPrint(data, "\r\n");
    }

    return;
}

_UINT32 utQueNodeFree(ST_UT_QUEUE_NODE* pstQueNode)
{
    memset(pstQueNode, 0, sizeof(ST_UT_QUEUE_NODE));
    UT_FREE(pstQueNode);

    return EOS_OK;
}

#if defined(__cplusplus)
}
#endif

