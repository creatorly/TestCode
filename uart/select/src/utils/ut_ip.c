#include "eos.h"
#include "eos_buf.h"
#include "utl_dns.h"
#include "ut_general.h"
#include "ut_mem.h"
#include "ut_ip.h"
#include "ut_debug.h"

#define UT_IP_SEM_NAME      "ut-ip-sem"

#define UT_IP_UPDATE_INTVL  (3600*sysClkRateGet())

typedef struct tagUtIpLst
{
    EOSBool    bInit;
    _LIST    stLst;
    EOS_SEM_ID    stSemId;
}UtIpLstSt;

typedef struct tagUtIp
{
    _NODE node;
    
    EOSBool bDeleted;
    
    EOSBool bValid;
    _UINT32 uiLastTick;
    _UINT32 uiIp;
    _UINT32 uiOldIp;
    
    _CHAR8 acDnsNa[UT_DEFAULT_STR_MAX];
    _CHAR8 acIpStr[UT_DEFAULT_STR_MAX];

    /* 2006.11.28 pengjian modified */
    _LIST lsUpdates;
    _BOOL bNullFuncAdded;
}UtIpSt;

typedef struct tagstUtIpCB
{
    _NODE node;

    _VOID (*fp)(_VOID);
    _VOID* data;

    FP_UTIP_RESOLVFAIL fpResolvFail;
}ST_UT_IP_CB;

typedef struct tagIpShow{
    EOS_INFO_PRINTF pPrintFunc;
    _VOID *pData;
}UtIpShowSt;

#define    EOS_CONTRUCT_STRUCT(_sptr,_sname)    \
do { \
    _sptr=(_sname*)UT_MALLOC(sizeof(_sname));    \
    if(_sptr==NULL)    \
        return EOS_ERROR;        \
    memset(_sptr,0,sizeof(_sname));    \
}while(0)

    
#define    EOS_DESTRUCT_STRUCT(_sptr) UT_FREE(_sptr)

#define UTIP_TRACE \
    printf("%s:%d\r\n", __FILE__, __LINE__)

#define UtIpSemP(semid, timeout) \
do{ \
    eos_sem_p(semid, timeout); \
}while(0)

#define UtIpSemV(semid) \
do{ \
    eos_sem_v(semid); \
}while(0)

_UINT32 utIpCbConsturct(ST_UT_IP_CB** ppstCb, _VOID* fpFunc, FP_UTIP_RESOLVFAIL fpFail, _VOID* data)
{
    ST_UT_IP_CB* pIpCb = NULL;

    UT_ASSERT(ppstCb);
    UT_ASSERT(fpFunc);

    *ppstCb = NULL;
    
    EOS_CONTRUCT_STRUCT(pIpCb, ST_UT_IP_CB);

    pIpCb->fpResolvFail = fpFail;
    pIpCb->fp = (_VOID (*)(_VOID))fpFunc;
    pIpCb->data = data;

    *ppstCb = pIpCb;
    
    return EOS_OK;
}

_UINT32 utIpCbDestruct(ST_UT_IP_CB* pstCb)
{
    UT_ASSERT(pstCb);

    memset(pstCb, 0, sizeof(ST_UT_IP_CB));
    EOS_DESTRUCT_STRUCT(pstCb);

    return EOS_OK;
}

/* pengjian added on 2006.06.06 */
_VOID utIpInvokeUpdateCB(UtIpSt* pstIp)
{
    ST_UT_IP_CB* pstIpCB = NULL;

    pstIpCB = (ST_UT_IP_CB*)eos_lst_first(&pstIp->lsUpdates);
    while (NULL != pstIpCB)
    {
        ((FP_UTIP_UPDATE)pstIpCB->fp)(pstIp->acDnsNa, 
            pstIp->acIpStr, pstIp->uiIp, pstIpCB->data);
        pstIpCB = (ST_UT_IP_CB*)eos_lst_next(&pstIpCB->node);
    }
}

/* pengjian added on 2006.06.06 */
_VOID utIpInvokeResolvFailCB(UtIpSt* pstIp)
{
    ST_UT_IP_CB* pstIpCB = NULL;

    pstIpCB = (ST_UT_IP_CB*)eos_lst_first(&pstIp->lsUpdates);
    while (pstIpCB != NULL)
    {
        if (pstIpCB->fpResolvFail != NULL)
        {
            pstIpCB->fpResolvFail(pstIp->acDnsNa, 1, pstIpCB->data);
        }

        pstIpCB = (ST_UT_IP_CB*)eos_lst_next(&pstIpCB->node);
    }
}

_INT32 utIpContruct(LPHIPLST lphIplst)
{
    UtIpLstSt* pIpLst;

    UT_NULL_CHECK(lphIplst);

    EOS_CONTRUCT_STRUCT(pIpLst,UtIpLstSt);

    eos_lst_init(&pIpLst->stLst);
    if(eos_sem_create(UT_IP_SEM_NAME,&pIpLst->stSemId)!=EOS_OK)
    {
        EOS_DESTRUCT_STRUCT(pIpLst);
        return EOS_ERROR;    
    }
    pIpLst->bInit=EOS_TRUE;

    *lphIplst=(HIPLST)pIpLst;
    return EOS_OK;
}

_INT32 utIpDestruct(HIPLST hIplst)
{
    EOS_DESTRUCT_STRUCT((void*)hIplst);
    return EOS_OK;
}

__STATIC__ EOSBool pfIpLstOpCompFunc(_NODE* pNode, const _VOID* pKey)
{
    UtIpSt* pIp=(UtIpSt*)pNode;
    
    return (!pIp->bDeleted && !strcmp(pIp->acDnsNa, pKey));
}

__STATIC__ EOSBool pfIpFuncCompFunc(_NODE* pNode, const _VOID* pKey)
{
    ST_UT_IP_CB* pIpCb=(ST_UT_IP_CB*)pNode;
    return (pIpCb->fp == pKey);
}

_INT32 utIpGetAddr(HIPLST hIplst, const _CHAR8* pcIp,_CHAR8* pcNewIp,_UINT32* puiIp)
{
    UtIpLstSt* pIpLst=(UtIpLstSt*)hIplst;
    UtIpSt* pIp;
    _UINT32 ulIpv4 = -1;
    _INT32 ret = EOS_ERROR;

    UT_NULL_CHECK(hIplst)
    UT_NULL_CHECK(pcIp)
    UT_NULL_STR_CHECK(pcIp)

    /* 2014.09.24 pengjian added */
    if (eos_address_string_to_ipv4(&ulIpv4, pcIp))
    {
        if(pcNewIp && pcNewIp != pcIp)
            strcpy(pcNewIp, pcIp);
        
        if(puiIp)
            *puiIp=ulIpv4;

        return EOS_OK;
    }
    
    if(pIpLst->bInit!=EOS_TRUE)
        return EOS_ERROR;

    UtIpSemP(pIpLst->stSemId,EOS_WAIT_FOREVER);
    
    /* 2014.09.24 pengjian modified */
    pIp = (UtIpSt*)eos_lst_search(&pIpLst->stLst,(_VOID*)pcIp, pfIpLstOpCompFunc);
    if (NULL == pIp)
    {        
        /* construct a node and add it to list */
        pIp = UT_TALLOC(UtIpSt);
        if (NULL == pIp)
        {
            UtIpSemV(pIpLst->stSemId);
            return EOS_ERROR;
        }

        memset(pIp, 0, sizeof(UtIpSt));
        
        eos_buf_strcpy(pIp->acDnsNa, pcIp);
        eos_lst_init(&pIp->lsUpdates);
        
        eos_lst_add(&pIpLst->stLst, &pIp->node);
    }
    else if (pIp->bValid)
    {
        if(pcNewIp)
            strcpy(pcNewIp,pIp->acIpStr);
        
        if(puiIp)
            *puiIp=pIp->uiIp;
                
        ret = EOS_OK;
    }

    UtIpSemV(pIpLst->stSemId);

    return ret;
}

/* 2014.03.28 pengjian added */
_INT32 utIpAddAddr(HIPLST hIplst,
                   const _CHAR8* pcIp, 
                   FP_UTIP_UPDATE fpUpdate, 
                   FP_UTIP_RESOLVFAIL fpResolvFail,
                   _VOID* data)
{
    UtIpLstSt* pIpLst=(UtIpLstSt*)hIplst;
    UtIpSt* pIp;
    _UINT32 ulIpv4;

    UT_NULL_CHECK(hIplst)
    UT_NULL_CHECK(pcIp)
    UT_NULL_STR_CHECK(pcIp)

    if(pIpLst->bInit!=EOS_TRUE)
        return EOS_ERROR;

    UtIpSemP(pIpLst->stSemId,EOS_WAIT_FOREVER);
    
    pIp = (UtIpSt*)eos_lst_search(&pIpLst->stLst,(_VOID*)pcIp,pfIpLstOpCompFunc);
    if (NULL == pIp)
    {
        pIp = UT_TALLOC(UtIpSt);
        if (NULL == pIp)
        {
            UtIpSemV(pIpLst->stSemId);
            return EOS_ERROR;
        }

        memset(pIp, 0, sizeof(UtIpSt));

        eos_buf_strcpy(pIp->acDnsNa, pcIp);
        eos_lst_init(&pIp->lsUpdates);

        eos_lst_add(&pIpLst->stLst,(_NODE*)pIp);
    }

    /* deleted by pengjian on 2006.06.06 */
    /* pfIpLstForEachProcFunc((_NODE*)pIp, pcIp); */
    /* added by pengjian on 2006.09.15 */
    if (eos_address_string_to_ipv4(&ulIpv4, pcIp))
    {
        strcpy(pIp->acIpStr, pcIp);
        pIp->uiOldIp = pIp->uiIp = ulIpv4;
        pIp->bValid = EOS_TRUE;
    }

    if (fpUpdate != NULL)
    {
        ST_UT_IP_CB* pIpCb;

        pIpCb = (ST_UT_IP_CB*)eos_lst_search(&pIp->lsUpdates, fpUpdate, pfIpFuncCompFunc);
        if (pIpCb != NULL)
        {
            /* update the function entity */
            pIpCb->fpResolvFail = fpResolvFail;
            pIpCb->data = data;
        }
        else
        {
            /* add a new entity */
            if (utIpCbConsturct(&pIpCb, fpUpdate, fpResolvFail, data))
            {
                UtIpSemV(pIpLst->stSemId);
                return EOS_ERROR;
            }
            
            eos_lst_add(&pIp->lsUpdates, &pIpCb->node);
        }
        
        if (EOS_TRUE == pIp->bValid)
        {
            fpUpdate(pIp->acDnsNa, pIp->acIpStr, pIp->uiIp, data);
        }
    }
    else
    {
        pIp->bNullFuncAdded = EOS_TRUE;
    }

    UtIpSemV(pIpLst->stSemId);
    
    return EOS_OK;
}

_INT32 utIpDelAddr(HIPLST hIplst,_CHAR8* pcIp, FP_UTIP_UPDATE fpUpdate)
{
    UtIpLstSt* pIpLst=(UtIpLstSt*)hIplst;
    UtIpSt* pIp;
    
    UT_NULL_CHECK(hIplst)
    UT_NULL_CHECK(pcIp)
    UT_NULL_STR_CHECK(pcIp)

    if(pIpLst->bInit!=EOS_TRUE)
        return EOS_ERROR;

    UtIpSemP(pIpLst->stSemId,EOS_WAIT_FOREVER);
    
    pIp=(UtIpSt*)eos_lst_search(&pIpLst->stLst,(_VOID*)pcIp,pfIpLstOpCompFunc);
    if (pIp != NULL)
    {
        if (fpUpdate != NULL)
        {
            ST_UT_IP_CB* pIpCb;

            pIpCb = (ST_UT_IP_CB*)eos_lst_search(&pIp->lsUpdates, fpUpdate, pfIpFuncCompFunc);
            if (pIpCb != NULL)
            {
                /* delete a function node */
                eos_lst_del(&pIp->lsUpdates, &pIpCb->node);
                utIpCbDestruct(pIpCb);

                if (!pIp->bNullFuncAdded && eos_lst_count(&pIp->lsUpdates) == 0)
                {
                    pIp->bDeleted = EOS_TRUE;
                }
            }
        }
        else
        {
            eos_lst_uninit(&pIp->lsUpdates, (EOS_LSTFREE_FUNC)utIpCbDestruct);
            
            pIp->bDeleted = EOS_TRUE;
        }
    }

    UtIpSemV(pIpLst->stSemId);
    
    return EOS_OK;
}

/* pengjian modified on 2006.06.06 */
_INT32 utIpUpdatesAddr(HIPLST hIplst)
{
    UtIpLstSt* pIpLst=(UtIpLstSt*)hIplst;
    _NODE* pn;

    UT_NULL_CHECK(hIplst)

    if(pIpLst->bInit!=EOS_TRUE)
        return EOS_ERROR;

    UtIpSemP(pIpLst->stSemId,EOS_WAIT_FOREVER);
    
    pn = eos_lst_first(&pIpLst->stLst);    
    for (; pn; pn = eos_lst_next(pn))
    {
        UtIpSt* pstIp = (UtIpSt*)pn;
        _UINT32 s_addr;

        if (pstIp->bDeleted)
        {
            continue;
        }

        if (pstIp->bValid && (tickGet() < pstIp->uiLastTick + UT_IP_UPDATE_INTVL))
        {
            continue;
        }

        if ((s_addr = inet_addr(pstIp->acDnsNa)) == INADDR_NONE)
        {
            /* give semaphore because of the long timeout of this operation probably */
            UtIpSemV(pIpLst->stSemId);
        
            s_addr = utl_gethostbyname(pstIp->acDnsNa, NULL, 0);
            
            UtIpSemP(pIpLst->stSemId,EOS_WAIT_FOREVER);

            /* may deleted by other threads */
            if (pstIp->bDeleted)
            {
                continue;
            }
            
            if (INADDR_NONE == s_addr)
            {
                UtIpSt ip_node;

                /* store the ip node to trace */
                memcpy(&ip_node, pstIp, sizeof(ip_node));
                
                utIpInvokeResolvFailCB(pstIp);
                
                continue;
            }
            
            pstIp->uiIp = s_addr;
            eos_address_ipv4_to_string(pstIp->acIpStr,sizeof(pstIp->acIpStr),pstIp->uiIp);
        }else{
            strcpy(pstIp->acIpStr,pstIp->acDnsNa);
            eos_address_string_to_ipv4(&pstIp->uiIp,pstIp->acDnsNa);
        }

        pstIp->bValid = EOS_TRUE;
        pstIp->uiLastTick = tickGet();

        if (pstIp->uiOldIp != pstIp->uiIp)
        {
            utIpInvokeUpdateCB(pstIp);
            pstIp->uiOldIp = pstIp->uiIp;
        }
    }
    
    UtIpSemV(pIpLst->stSemId);
    
    return EOS_OK;
}

__STATIC__ _INT32 pfIpLstShowForEachProcFunc (_NODE * pNode, const _VOID* pData)
{
    UtIpShowSt* pIpShow=(UtIpShowSt*)pData;
    UtIpSt* pIp=(UtIpSt*)pNode;
    _CHAR8 cBuf[128];

    UT_NULL_CHECK(pNode)
    UT_NULL_CHECK(pData)

    sprintf(cBuf,"%20s|%20s|%10x|%10s\r\n",pIp->acDnsNa,pIp->acIpStr,pIp->uiIp,
        pIp->bValid?"Ok":"Error");
    pIpShow->pPrintFunc(pIpShow->pData,cBuf);
    return EOS_OK;
}


_INT32 utIpShow(HIPLST hIplst,EOS_INFO_PRINTF pPrintFunc,_VOID *pData)
{
    UtIpLstSt* pIpLst=(UtIpLstSt*)hIplst;
    UtIpShowSt stIpShow;

    _CHAR8 cBuf[128];
    _INT32 iTotal;

    UT_NULL_CHECK(hIplst)

    if(pIpLst->bInit!=EOS_TRUE)
        return EOS_ERROR;

    UtIpSemP(pIpLst->stSemId,EOS_WAIT_FOREVER);
    iTotal=eos_lst_count(&pIpLst->stLst);
    if(iTotal==0)
    {   
        UtIpSemV(pIpLst->stSemId);
        return EOS_ERROR;
   }
    
    sprintf(cBuf,"Ip List Total Number:%d\r\n",iTotal);
    pPrintFunc(pData,cBuf);
    

    stIpShow.pData=pData;
    stIpShow.pPrintFunc=pPrintFunc;

    
    sprintf(cBuf,"%20s|%20s|%10s|%10s\r\n","Dns Name","STR-IP","INT-IP","STATE");
    pPrintFunc(pData,cBuf);
    
    eos_lst_foreach(&pIpLst->stLst,pfIpLstShowForEachProcFunc,&stIpShow);

    UtIpSemV(pIpLst->stSemId);
    return EOS_OK;
}

/*         test code        
_VOID testIpAddr()
{
    HIPLST hIpLst;

    _CHAR8* apcIplst[]={
        "www.163.com",
        "www.imag.com",
        "www.sina.com",
        "www.163.com"};
        
    int i;
    eos_init();

    utIpContruct(&hIpLst);

    for(i=0;i<EOS_NELEMENTS(apcIplst);i++)
        utIpAddAddr(hIpLst,apcIplst[i]);

    utIpDelAddr(hIpLst,apcIplst[2]);

    utIpShow(hIpLst,EOS_DEFAULT_INFO_PRINT_FUNC,NULL);
    
}
*/

