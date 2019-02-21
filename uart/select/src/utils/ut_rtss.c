
/******************************************************************************
 * Description: ut_rtss.c, runtime statistics set
 * Author: pengjian
 * Histroy: 1.00, 2006.06.06 created
 * CopyRight: Ejoin Tech. Ltd. Co, 2006.6~2006
******************************************************************************/

#include "eos.h"
#include "ut_debug.h"
#include "ut_mem.h"

#include "ut_rtstat.h"
#include "ut_rtss.h"
#include "ut_rtss.inc"

#if defined(__cplusplus)
extern "C" {
#endif

_UINT32 UT_RTSS_Construct(H_UT_RTSS* phRtss)
{
    static _UINT32 uiRtssCnt = 0;
    ST_UT_RTSS* pstRtss = NULL;    
    _CHAR8 acSemName[16];
        
    if (NULL == phRtss)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    *phRtss = NULL;
    
    pstRtss = (ST_UT_RTSS*)UT_MALLOC(sizeof(ST_UT_RTSS));
    if (NULL == pstRtss)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return EOS_ERROR;
    }

    memset(pstRtss, 0, sizeof(ST_UT_RTSS));

    sprintf(acSemName, "mRtss%d", uiRtssCnt++);
    eos_sem_create(acSemName, &pstRtss->m);
    
    UtRtssSemP(pstRtss->m);
    eos_lst_init(&pstRtss->lsRtss);
    UtRtssSemV(pstRtss->m);
    
    *phRtss = pstRtss;

    return EOS_OK;
}

_UINT32 UT_RTSS_Destruct(H_UT_RTSS hRtss)
{
    ST_UT_RTSS* pstRtss = (ST_UT_RTSS*)hRtss;    

    if (NULL == hRtss)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    UtRtssSemP(pstRtss->m);
    eos_lst_uninit(&pstRtss->lsRtss, (EOS_LSTFREE_FUNC)utRtssNodeFree);
    UtRtssSemV(pstRtss->m);

    memset(pstRtss, 0, sizeof(ST_UT_RTSS));

    return EOS_OK;
}

_VOID* UT_RTSS_Add(H_UT_RTSS hRtss, 
                    const _CHAR8* pcFile,
                    _UINT32 uiLine,
                    _VOID* fpPrint,
                    _VOID* data)
{
    ST_UT_RTSS* pstRtss = (ST_UT_RTSS*)hRtss;    
    ST_UT_RTSS_NODE* pstRtssNode = NULL;
    H_UT_RTS hRts = NULL;
    
    if (NULL == hRtss)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return NULL;
    }

    UTRTS_Construct(&hRts, pcFile, uiLine, fpPrint, data);
    if (NULL == hRts)
    {
        return NULL;
    }
    
    pstRtssNode = (ST_UT_RTSS_NODE*)UT_MALLOC(sizeof(ST_UT_RTSS_NODE));
    if (NULL == pstRtssNode)
    {
        UTRTS_Destruct(hRts);
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return NULL;
    }

    memset(pstRtssNode, 0, sizeof(ST_UT_RTSS_NODE));
    pstRtssNode->hRts = hRts;

    UtRtssSemP(pstRtss->m);
    eos_lst_add(&pstRtss->lsRtss, &pstRtssNode->node);
    UtRtssSemV(pstRtss->m);

    return hRts;
}

_UINT32 UT_RTSS_Del(H_UT_RTSS hRtss, _VOID* hRts)
{
    ST_UT_RTSS* pstRtss = (ST_UT_RTSS*)hRtss;    
    ST_UT_RTSS_NODE* pstRtssNode = NULL;
    
    if ((NULL == hRtss) || (NULL == hRts))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    UtRtssSemP(pstRtss->m);
    
    pstRtssNode = (ST_UT_RTSS_NODE*)eos_lst_search(&pstRtss->lsRtss, hRts, utRtssNodeEqu);
    if (NULL == pstRtssNode)
    {
        UtRtssSemV(pstRtss->m);
        UT_ERROR(EOS_ERROR, "not existed rtss node!\r\n");
        return EOS_ERROR;
    }

    eos_lst_del(&pstRtss->lsRtss, &pstRtssNode->node);
    
    UtRtssSemV(pstRtss->m);

    UTRTS_Destruct(pstRtssNode->hRts);
    memset(pstRtssNode, 0, sizeof(ST_UT_RTSS_NODE));
    UT_FREE(pstRtssNode);

    return EOS_OK;
}

_VOID UT_RTSS_Show(H_UT_RTSS hRtss, FP_UT_RTS_PRINTF fpPrint, _VOID* data)
{
    ST_UT_RTSS* pstRtss = (ST_UT_RTSS*)hRtss;   
    ST_UT_RTSS_NODE* pstNode = NULL;
    
    if (NULL == hRtss)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return;
    }

    UtRtssSemP(pstRtss->m);
    
    pstNode = (ST_UT_RTSS_NODE*)eos_lst_first(&pstRtss->lsRtss);
    while (NULL != pstNode)
    {
        UTRTS_Show(pstNode->hRts, fpPrint, data);
        pstNode = (ST_UT_RTSS_NODE*)eos_lst_next(&pstNode->node);
    }

    UtRtssSemV(pstRtss->m);
}

_UINT32 utRtssNodeFree(ST_UT_RTSS_NODE* pstRtssNode)
{
    UTRTS_Destruct(pstRtssNode->hRts);
    memset(pstRtssNode, 0, sizeof(ST_UT_RTSS_NODE));
    UT_FREE(pstRtssNode);

    return EOS_OK;
}

_BOOL utRtssNodeEqu(_NODE * pNode, const _VOID* pKey)
{
    return (((ST_UT_RTSS_NODE*)pNode)->hRts == pKey);
}

#if defined(__cplusplus)
}
#endif

