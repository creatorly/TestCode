#include "eos.h"
#include "ut_mem.h"
#include "ut_peer_addr.h"

#define UT_PEER_ADDR_STR_MAX            32

typedef struct tagPeerSock
{
    _CHAR8 acIp[UT_PEER_ADDR_STR_MAX];
    _USHORT16 usPort;
    _USHORT16 usPad;
}UtPeerSockSt;

typedef struct tagUtPeerAddr
{
    _NODE node;
    _CHAR8 acPeerId[UT_PEER_ADDR_STR_MAX];
    UtPeerSockSt stSock;
    _BOOL bDropPeerId;
}UtPeerAddrSt;

typedef struct tagUtPeerAddrLst
{
    EOSBool    bInit;
    _LIST    stLst;
}UtPeerAddrLstSt;

typedef struct tagPeerAppShow{
    EOS_INFO_PRINTF pPrintFunc;
    _VOID *pData;
}PeerAppShowSt;


_UINT32 utPeerConstruct(UtPeerAddrSt** ppstPeer, 
                        _CHAR8* pcId, 
                        _CHAR8* pcIp, 
                        _USHORT16 usPort,
                        _BOOL bDrop)
{
    UtPeerAddrSt* pstPeer = NULL;

    if (NULL == ppstPeer)
         return EOS_ERROR;
   

    pstPeer = (UtPeerAddrSt*)UT_MALLOC(sizeof(UtPeerAddrSt));
    if (NULL == pstPeer)
        return EOS_ERROR;
  
    memset(pstPeer, 0, sizeof(UtPeerAddrSt));
    strcpy(pstPeer->acPeerId, pcId);
    strcpy(pstPeer->stSock.acIp, pcIp);
    pstPeer->stSock.usPort = usPort;
    pstPeer->bDropPeerId = bDrop;

    *ppstPeer = pstPeer;
    return EOS_OK;
}

_UINT32 utPeerDestruct(UtPeerAddrSt* pstPeer)
{
    if (NULL == pstPeer)
         return EOS_ERROR;
 
    memset(pstPeer, 0, sizeof(UtPeerAddrSt));
    UT_FREE(pstPeer);
    
    return EOS_OK;
}

_BOOL     utPeerEquById(_NODE* node, const _VOID* pKey)
{
    _CHAR8* pcPeerId = ((UtPeerAddrSt*)node)->acPeerId;
    return (0 == strncmp(pcPeerId, (_CHAR8*)pKey, strlen(pcPeerId)));
}


_UINT32 utGetPeerAddrByCallNum(UtPeerAddrLstSt*pLst,_CHAR8** ppcCallNum, _CHAR8* pcIp, _USHORT16* pusPort)
{
    UtPeerAddrSt* pstPeer = NULL;

    pstPeer = (UtPeerAddrSt*)eos_lst_search(&pLst->stLst, *ppcCallNum, utPeerEquById);
    if (NULL == pstPeer)
        return EOS_ERROR;
   
    strcpy(pcIp, pstPeer->stSock.acIp);
    *pusPort = pstPeer->stSock.usPort;

    if (pstPeer->bDropPeerId)
    {
        (*ppcCallNum) += strlen(pstPeer->acPeerId);
    }

    return EOS_OK;
}

__STATIC__ EOSBool pfPeerAddrOpCompFunc(_NODE * pNode, const _VOID* pKey)
{
    UtPeerAddrSt* pPeerAddr=(UtPeerAddrSt*)pNode;

    if((pNode==NULL)||(pKey==NULL))
        return EOS_FALSE;

    if(!strcmp(pPeerAddr->acPeerId,(_CHAR8*)pKey))
        return EOS_TRUE;

    return EOS_FALSE;
}

_INT32 utPeerAppConstruct(LPHPAL lpHApp)
{
    UtPeerAddrLstSt* pPeerAddrLst;

    if(lpHApp==NULL)
        return EOS_ERROR;

    pPeerAddrLst=(UtPeerAddrLstSt*)UT_MALLOC(sizeof(UtPeerAddrLstSt));
    if(pPeerAddrLst==NULL)
        return EOS_ERROR;

    memset(pPeerAddrLst,0,sizeof(UtPeerAddrLstSt));

    eos_lst_init(&pPeerAddrLst->stLst);
    pPeerAddrLst->bInit=EOS_TRUE;
    
    *lpHApp=(HPAL)pPeerAddrLst;
    return EOS_OK;
}

_INT32 utPeerAppDestruct(HPAL pHApp)
{
    if(pHApp==NULL)
        return EOS_ERROR;

    UT_FREE((_VOID*)pHApp);

    return EOS_OK;
}

_INT32 utPeerAppAddAddr(HPAL pHApp,_CHAR8* pcId, _CHAR8* pcIp, _USHORT16 usPort,_BOOL bDrop)
{
    UtPeerAddrLstSt* pPeerAddrLst=(UtPeerAddrLstSt*)pHApp;
    UtPeerAddrSt* pPeerAddr;

    if(pHApp==NULL)
        return EOS_ERROR;

    
    pPeerAddr=(UtPeerAddrSt*)eos_lst_search(&pPeerAddrLst->stLst,pcId,pfPeerAddrOpCompFunc);
    if(pPeerAddr!=NULL)
        return EOS_ERROR;
    
    if(utPeerConstruct(&pPeerAddr,pcId,pcIp,usPort,bDrop)==EOS_ERROR)
        return EOS_ERROR;

    eos_lst_add(&pPeerAddrLst->stLst,(_NODE*)pPeerAddr);

    return EOS_OK;
}

_INT32 utPeerAppDelAddr(HPAL pHApp,_CHAR8* pcId)
{
    UtPeerAddrLstSt* pPeerAddr=(UtPeerAddrLstSt*)pHApp;
    _NODE* pNode;

    if(pHApp==NULL)
        return EOS_ERROR;

    pNode=eos_lst_search(&pPeerAddr->stLst,pcId,pfPeerAddrOpCompFunc);

    if(pNode)
    {
        eos_lst_del(&pPeerAddr->stLst,pNode);
        utPeerDestruct((UtPeerAddrSt*)pNode);
    }

    return EOS_OK;
}

_INT32 utPeerAppGetAddr(HPAL pHApp,_CHAR8** ppcCallNum, _CHAR8* pcIp, _USHORT16* pusPort)
{
    UtPeerAddrLstSt* pPeerAddr=(UtPeerAddrLstSt*)pHApp;

    if(pHApp==NULL)
        return EOS_ERROR;

    return(utGetPeerAddrByCallNum(pPeerAddr,ppcCallNum,pcIp,pusPort));
}

_INT32 pfPeerAppLstForEachProcFunc (_NODE * pNode, const _VOID* pData)
{
    PeerAppShowSt* pPAShow=(PeerAppShowSt*)pData;
    UtPeerAddrSt* pPeerAddr=(UtPeerAddrSt*)pNode;
    _CHAR8 cBuf[128];

    if((pNode==NULL)||(pData==NULL))
        return EOS_ERROR;

    sprintf(cBuf,"%10s|%20s|%10d|%10s\r\n",pPeerAddr->acPeerId,pPeerAddr->stSock.acIp,
        pPeerAddr->stSock.usPort,((pPeerAddr->bDropPeerId==EOS_TRUE)?"TRUE":"FALSE"));
    pPAShow->pPrintFunc(pPAShow->pData,cBuf);

    return EOS_OK;
}


_INT32 utPeerAppShow(HPAL pHApp,EOS_INFO_PRINTF pPrintFunc,_VOID *pData)
{
    UtPeerAddrLstSt* pPeerAddrLst=(UtPeerAddrLstSt*)pHApp;
    PeerAppShowSt stPAShow;

    _CHAR8 cBuf[128];
    _INT32 iTotal;

    if(pPeerAddrLst->bInit!=EOS_TRUE)
        return EOS_ERROR;
    
    iTotal=eos_lst_count(&pPeerAddrLst->stLst);
    if(iTotal==0)
        return EOS_ERROR;
    
    sprintf(cBuf,"PBook Total Number:%d\r\n",iTotal);
    pPrintFunc(pData,cBuf);

    stPAShow.pData=pData;
    stPAShow.pPrintFunc=pPrintFunc;

    
    sprintf(cBuf,"%10s|%20s|%10s|%10s\r\n","ID","IP","PORT","DROP");
    pPrintFunc(pData,cBuf);
    
    eos_lst_foreach(&pPeerAddrLst->stLst,pfPeerAppLstForEachProcFunc,&stPAShow);

    return EOS_OK;
}


/********************* TEST CODE********************
typedef struct{
    _CHAR8 pId[32];
    _CHAR8 pIp[32];
    _USHORT16 port;
    EOSBool    bDrop;
}peeraddrunit;

_VOID testPeerAddr()
{
    HPAL pPeerAddr;

    peeraddrunit addlst[]={
        {"0","192.168.0.12",9090,EOS_TRUE},
        {"1","192.169.0.23",19090,EOS_TRUE},
        {"1","192.169.0.23",19090,EOS_TRUE},
        {"3","192.169.0.23",90,EOS_TRUE}};
    int i;
    char* c="013798416793";
    char ip[32];
    short port;

    eos_init();

    utPeerAppConstruct(&pPeerAddr);

    for(i=0;i<EOS_NELEMENTS(addlst);i++)
        utPeerAppAddAddr(pPeerAddr,addlst[i].pId,
        addlst[i].pIp,addlst[i].port,addlst[i].bDrop);


    utPeerAppGetAddr(pPeerAddr,&c,ip,&port);
    printf("call=%s,Ip=%s,port=%d\r\n",c,ip,port);

    
    utPeerAppDelAddr(pPeerAddr,addlst[2].pId);


        
}

*/



