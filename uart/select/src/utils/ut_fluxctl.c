
/******************************************************************************
 * Description: flux control
 * Author: pengjian
 * Histroy: 1.00, 2006.5.22 created
 * CopyRight: Ejoin Tech. Ltd. Co, 2006.5~2006
******************************************************************************/

#include "eos.h"
#include "eos_stat.h"
#include "ut_netinet.h"
#include "ut_debug.h"
#include "acomsys.h"
#include "ut_mem.h"

#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
#include "netinet/if_ether.h"
#include "netinet/ip.h"
#endif

#include "ut_fluxctl.h"
#include "ut_fluxctl.inc"

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_UT_FLUXCTL

_UINT32 UTFC_Construct(H_UTFC* phUtfc, ST_UTFC_CONFIG* pstCfg)
{
    _UINT32 uiCurTicks = tickGet();
    _UINT32 uiTicksOfTimePiece = 0;
    _UINT32 i = 0;
    _UINT32 uiCurTalkCnt = 0;
    ST_UTFC_DESC* pstFc = NULL;

    if (NULL == phUtfc)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    *phUtfc = NULL;

    if ( (NULL == pstCfg) 
        || (pstCfg->uiTimePerPiece * sysClkRateGet() < 1000) 
        || (pstCfg->uiMaxCpuUsage <= 0) || (pstCfg->uiMaxCpuUsage > 100)
        || (pstCfg->uiMinVoicePassRate <= 0) || (pstCfg->uiMinVoicePassRate > 100) )
    {
        UT_ERROR(EOS_ERROR, "invalid parameters passed!\r\n");
        return EOS_ERROR;
    }

    pstFc = (ST_UTFC_DESC*)UT_MALLOC(sizeof(ST_UTFC_DESC));
    if (NULL == pstFc)
    {
        UT_ERROR(EOS_ERROR, "memory allocation failed!\r\n");
        return EOS_ERROR;
    }

    uiTicksOfTimePiece = pstCfg->uiTimePerPiece * sysClkRateGet() / 1000;

    memset(pstFc, 0, sizeof(ST_UTFC_DESC));

    pstFc->bAclEnable = pstCfg->bAclEnable;
    pstFc->uiTimePerPiece = uiTicksOfTimePiece;
    pstFc->uiMaxCpuUsage = pstCfg->uiMaxCpuUsage;
    pstFc->uiMinVoicePassRate = pstCfg->uiMinVoicePassRate;
    pstFc->fpIsVoicePkg = NULL;
    pstFc->fpGetTalkCnt = NULL;
    
    eos_lst_init(&pstFc->alsAcls[0]);
    eos_lst_init(&pstFc->alsAcls[1]);
    
    for (i=0; i<EOS_NELEMENTS(pstFc->aastFilters); i++)
    {
        _UINT32 j = 0;

        uiCurTalkCnt = utfcGetTalkCntHookInvoke(pstFc, i);
        for (j=0; j<EOS_NELEMENTS(pstFc->aastFilters[0]); j++)
        {
            pstFc->aastFilters[i][j].uiTimeBegin = uiCurTicks;
            pstFc->aastFilters[i][j].uiTimeEnd = uiCurTicks + uiTicksOfTimePiece;

            pstFc->aastFilters[i][j].uiPkgsRcvdPerTalk = 0;
            pstFc->aastFilters[i][j].uiPkgsSentPerTalk = 0;
            pstFc->aastFilters[i][j].uiMaxPkgsRecv = UTFC_DEF_PKGS_RECV;
            pstFc->aastFilters[i][j].uiMaxPkgsSend = UTFC_DEF_PKGS_SEND;
            pstFc->aastFilters[i][j].uiMaxDatasRecv = -1;
            pstFc->aastFilters[i][j].uiMaxDatasSend = -1;
            pstFc->aastFilters[i][j].uiRecvDataPassRate = UTFC_FULL_PASSRATE;
            pstFc->aastFilters[i][j].uiMaxTalkCnt = uiCurTalkCnt;
        }
    }
    
    pstFc->uiCurFilter = 0;
    *phUtfc = pstFc;

    return EOS_OK;
}

_UINT32 UTFC_Destruct(H_UTFC hUtfc)
{
    if (NULL == hUtfc)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    memset(hUtfc, 0, sizeof(ST_UTFC_DESC));
    UT_FREE(hUtfc);

    return EOS_OK;
}

_BOOL UTFC_RecvHook(H_UTFC hUtfc, _UINT32 uiChanId, _VOID* pMblk)
{
    ST_UTFC_DESC* pstFc = (ST_UTFC_DESC*)hUtfc;
    ST_UTFC_PKG_FILTER* pstCurFilter = NULL;
    ST_UT_IP* pstIp = NULL;

    if ((NULL == hUtfc) || (uiChanId >= EOS_NELEMENTS(pstFc->aastFilters)))
    {
        return EOS_TRUE;
    }

    if (!pstFc->bAclEnable)
    {
        return EOS_TRUE;
    }

    pstCurFilter = &pstFc->aastFilters[uiChanId][pstFc->uiCurFilter];
    
    pstCurFilter->uiTimeCur = tickGet();
    if (pstCurFilter->uiTimeCur >= pstCurFilter->uiTimeEnd)
    {
        utfcDescUpdate(hUtfc, uiChanId);  /* update time of piece */
        pstCurFilter = &pstFc->aastFilters[uiChanId][pstFc->uiCurFilter];
    }

    if (utfcIsIp(&pMblk, (_VOID**)&pstIp))
    {
        ST_UT_IP stIp;

        memcpy(&stIp, pstIp, sizeof(ST_UT_IP));
        
        /* to filter ip package */
        if (utfcIsVoicePkgHookInvoke(pstFc, uiChanId, stIp.ip_src.s_addr))
        {
            pstCurFilter->uiVoiceRcvd++;
        }
        else    /* is data package */
        {
            _VOID* pvUdpTcp = NULL;
            
            pstCurFilter->uiDataRcvd++;

            if (UT_IP_ISUDP(&stIp))
            {
                ST_UT_UDPHDR stUdpHdr;
            
                utfcIpGetUdpTcp(&pMblk, pstIp, &pvUdpTcp);
                memcpy(&stUdpHdr, pvUdpTcp, 4);
                
                if (!utfcAclFilter(&pstFc->alsAcls[uiChanId], UTFC_IPPROTO_UDP,
                        stIp.ip_dst.s_addr, UtUdpHdrGetDport(&stUdpHdr),
                        stIp.ip_src.s_addr, UtUdpHdrGetSport(&stUdpHdr))
                    && !(((rand()%UTFC_FULL_PASSRATE) < pstCurFilter->uiRecvDataPassRate)
                        && (pstCurFilter->uiDataRcvdPassed < pstCurFilter->uiMaxDatasRecv)) )
                {
                    UTFC_INFO4(" FC: rcvd udp pkg(src: %08x:%d, dst: %08x:%d) is discarded!\r\n", 
                        stIp.ip_src.s_addr, UtUdpHdrGetSport(&stUdpHdr), 
                        stIp.ip_dst.s_addr, UtUdpHdrGetDport(&stUdpHdr));
                    return EOS_FALSE;
                }
            }
            else if (UT_IP_ISTCP(&stIp))
            {
                ST_UT_TCPHDR stTcpHdr;
            
                utfcIpGetUdpTcp(&pMblk, pstIp, &pvUdpTcp);
                memcpy(&stTcpHdr, pvUdpTcp, 4);
                
                if (!utfcAclFilter(&pstFc->alsAcls[uiChanId], UTFC_IPPROTO_TCP,
                        stIp.ip_dst.s_addr, UtTcpHdrGetDport(&stTcpHdr),
                        stIp.ip_src.s_addr, UtTcpHdrGetSport(&stTcpHdr))
                    && !(((rand()%UTFC_FULL_PASSRATE) < pstCurFilter->uiRecvDataPassRate)
                        && (pstCurFilter->uiDataRcvdPassed < pstCurFilter->uiMaxDatasRecv)) )
                {
                    UTFC_INFO4(" FC: rcvd tcp pkg(src: %08x:%d, dst: %08x:%d) is discarded!\r\n", 
                        stIp.ip_src.s_addr, UtTcpHdrGetSport(&stTcpHdr), 
                        stIp.ip_dst.s_addr, UtTcpHdrGetDport(&stTcpHdr));
                    return EOS_FALSE;
                }
            }
            else if (UT_IP_ISICMP(&stIp))
            {
                ST_UT_ICMPHDR stIcmpHdr;
                int myOffset;

                myOffset = utfcIpGetUdpTcp(&pMblk, pstIp, &pvUdpTcp);
                utfcIpMblkGetBuf(&pMblk, myOffset, sizeof(stIcmpHdr), &stIcmpHdr);

                if ((stIcmpHdr.ih_type == 8) && (stIcmpHdr.ih_code == 0))
                {
                    /* the id and seq is host sequence */                    
                    printf("Recieved icmp request! "
                        "id = %04x, seq = %d/%d\r\n",
                        eosConvertNetworkToHost16(stIcmpHdr.ih_id), 
                        eosConvertNetworkToHost16(stIcmpHdr.ih_seq), stIcmpHdr.ih_seq);
                }
            }

            pstCurFilter->uiDataRcvdPassed++;
        }
    }

    return EOS_TRUE;
}

/* no need to take flux control when send package */
_BOOL UTFC_SendHook(H_UTFC hUtfc, _UINT32 uiChanId, _VOID* pMblk)
{
    ST_UTFC_DESC* pstFc = (ST_UTFC_DESC*)hUtfc;
    ST_UTFC_PKG_FILTER* pstCurFilter = NULL;
    ST_UT_IP* pstIp = NULL;

    if ((NULL == pstFc) || (uiChanId >= EOS_NELEMENTS(pstFc->aastFilters)))
    {
        return EOS_TRUE;
    }

    if (!pstFc->bAclEnable)
    {
        return EOS_TRUE;
    }
    
    pstCurFilter = &pstFc->aastFilters[uiChanId][pstFc->uiCurFilter];

    pstCurFilter->uiTimeCur = tickGet();
    if (pstCurFilter->uiTimeCur >= pstCurFilter->uiTimeEnd)
    {
        utfcDescUpdate(hUtfc, uiChanId);  /* update time of piece */
    }

    if (utfcIsIp(&pMblk, (_VOID**)&pstIp))
    {
        ST_UT_IP stIp;

        memcpy(&stIp, pstIp, sizeof(ST_UT_IP));
    
        if (utfcIsVoicePkgHookInvoke(pstFc, uiChanId, stIp.ip_dst.s_addr))
        {
            pstCurFilter->uiVoiceSent++;
        }
        else    /* is data package */
        {
            _VOID* pvUdpTcp = NULL;
            
            pstCurFilter->uiDataSent++;
            
            if (UT_IP_ISUDP(&stIp))
            {
                ST_UT_UDPHDR stUdpHdr;
            
                utfcIpGetUdpTcp(&pMblk, pstIp, &pvUdpTcp);
                memcpy(&stUdpHdr, pvUdpTcp, 4);

                if (!utfcAclFilter(&pstFc->alsAcls[uiChanId], UTFC_IPPROTO_UDP,
                        stIp.ip_src.s_addr, UtUdpHdrGetSport(&stUdpHdr),
                        stIp.ip_dst.s_addr, UtUdpHdrGetDport(&stUdpHdr))
                    && !(((rand()%UTFC_FULL_PASSRATE) < pstCurFilter->uiSendDataPassRate)
                        && (pstCurFilter->uiDataSentPassed < pstCurFilter->uiMaxDatasSend)) )
                {
                    UTFC_INFO4(" FC: send udp pkg(src: %08x:%d, dst: %08x:%d) is discarded!\r\n", 
                        stIp.ip_src.s_addr, UtUdpHdrGetSport(&stUdpHdr), 
                        stIp.ip_dst.s_addr, UtUdpHdrGetDport(&stUdpHdr));
                    return EOS_FALSE;
                }
            }
            else if (UT_IP_ISTCP(&stIp))
            {
                ST_UT_TCPHDR stTcpHdr;
            
                utfcIpGetUdpTcp(&pMblk, pstIp, &pvUdpTcp);
                memcpy(&stTcpHdr, pvUdpTcp, 4);

                if (!utfcAclFilter(&pstFc->alsAcls[uiChanId], UTFC_IPPROTO_TCP,
                        stIp.ip_src.s_addr, UtTcpHdrGetSport(&stTcpHdr),
                        stIp.ip_dst.s_addr, UtTcpHdrGetDport(&stTcpHdr))
                    && !(((rand()%UTFC_FULL_PASSRATE) < pstCurFilter->uiSendDataPassRate)
                        && (pstCurFilter->uiDataSentPassed < pstCurFilter->uiMaxDatasSend)) )
                {
                    UTFC_INFO4(" FC: send tcp pkg(src: %08x:%d, dst: %08x:%d) is discarded!\r\n", 
                        stIp.ip_src.s_addr, UtTcpHdrGetSport(&stTcpHdr), 
                        stIp.ip_dst.s_addr, UtTcpHdrGetDport(&stTcpHdr));
                    return EOS_FALSE;
                }
            }
            else if (UT_IP_ISICMP(&stIp))
            {
                ST_UT_ICMPHDR stIcmpHdr;
                int myOffset;

                myOffset = utfcIpGetUdpTcp(&pMblk, pstIp, &pvUdpTcp);
                utfcIpMblkGetBuf(&pMblk, myOffset, sizeof(stIcmpHdr), &stIcmpHdr);

                if ((stIcmpHdr.ih_type == 0) && (stIcmpHdr.ih_code == 0))
                {
                    /* the id and seq is host sequence */                    
                    printf("Send icmp response! "
                        "id = %04x, seq = %d/%d\r\n",
                        eosConvertNetworkToHost16(stIcmpHdr.ih_id), 
                        eosConvertNetworkToHost16(stIcmpHdr.ih_seq), stIcmpHdr.ih_seq);
                }
            }

            pstCurFilter->uiDataSentPassed++;
        }
    }

    return EOS_TRUE;
}

_UINT32 UTFC_IsVoicePkgHookAdd(H_UTFC hUtfc, FP_UTFC_ISVOICEPKG fpIsVoice)
{
    ST_UTFC_DESC* pstFc = (ST_UTFC_DESC*)hUtfc;

    if (NULL == hUtfc)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstFc->fpIsVoicePkg = fpIsVoice;
    return EOS_OK;
}

_UINT32 UTFC_GetTalkCntHookAdd(H_UTFC hUtfc, FP_UTFC_GETTALKCNT fpGetTalkCnt)
{
    ST_UTFC_DESC* pstFc = (ST_UTFC_DESC*)hUtfc;

    if (NULL == hUtfc)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstFc->fpGetTalkCnt = fpGetTalkCnt;
    return EOS_OK;
}

_UINT32 UTFC_AclAdd(H_UTFC hUtfc,
                   _UINT32 uiChanId,
                   _UINT32 uiProto,
                   _CHAR8* pcLocIp, 
                   _USHORT16 usLocPort, 
                   _CHAR8* pcRemIp,
                   _USHORT16 usRemPort)
{
    _UINT32 uiLocIp = 0;
    _UINT32 uiRemIp = 0;
    ST_UTFC_DESC* pstFc = (ST_UTFC_DESC*)hUtfc;
    ST_UTFC_ACD* pstAcd = NULL;

    UT_NULLPTR_CHK(pstFc, EOS_ERROR);
    UT_CONDCHK(uiChanId < EOS_NELEMENTS(pstFc->alsAcls), 
        EOS_ERROR, "invalid interface unit number!\r\n");

    if ((NULL != pcLocIp) && ('\0' != pcLocIp[0]))
    {
        if (!eos_address_string_to_ipv4(&uiLocIp, pcLocIp))
        {
            UT_ERROR(EOS_ERROR, "invalid inetaddr!\r\n");
            return EOS_ERROR;
        }
    }

    if ((NULL != pcRemIp) && ('\0' != pcRemIp[0]))
    {
        if (!eos_address_string_to_ipv4(&uiRemIp, pcRemIp))
        {
            UT_ERROR(EOS_ERROR, "invalid inetaddr!\r\n");
            return EOS_ERROR;
        }
    }

    /* check if the acl had beed added */
    pstAcd = (ST_UTFC_ACD*)eos_lst_first(&pstFc->alsAcls[uiChanId]);
    while (pstAcd != NULL)
    {
        if ( (uiProto == (uiProto & pstAcd->uiProto))
           && (pstAcd->uiLocIp == uiLocIp) && (pstAcd->usLocPort == usLocPort)
           && (pstAcd->uiRemIp == uiRemIp) && (pstAcd->usRemPort == usRemPort) )
        {
            /* the acl had been added before! */
            return EOS_OK;
        }

        pstAcd = (ST_UTFC_ACD*)eos_lst_next(&pstAcd->node);
    }
    
    UT_ST_CONSTRUCT(pstAcd, ST_UTFC_ACD);
    pstAcd->uiLocIp = uiLocIp;
    pstAcd->uiRemIp = uiRemIp;
    pstAcd->usLocPort = usLocPort;
    pstAcd->usRemPort = usRemPort;
    pstAcd->uiProto = uiProto;

    eos_lst_add(&pstFc->alsAcls[uiChanId], &pstAcd->node);

    return EOS_OK;
}

_UINT32 UTFC_AclUpdate(H_UTFC hUtfc,_UINT32 uiChanId, const _CHAR8* pcLocIp)
{
    _UINT32 uiLocIp = 0;
    ST_UTFC_DESC* pstFc = (ST_UTFC_DESC*)hUtfc;
    ST_UTFC_ACD* pstAcd = NULL;

    UT_NULLPTR_CHK(pstFc, EOS_ERROR);
    UT_CONDCHK(uiChanId < EOS_NELEMENTS(pstFc->alsAcls), 
        EOS_ERROR, "invalid interface unit number!\r\n");

    if ((NULL == pcLocIp) || ('\0' == pcLocIp[0]))
    {
        UT_ERROR(EOS_ERROR, "null local inetaddr!\r\n");
        return EOS_ERROR;
    }
    
    if (!eos_address_string_to_ipv4(&uiLocIp, pcLocIp))
    {
        UT_ERROR(EOS_ERROR, "invalid inetaddr!\r\n");
        return EOS_ERROR;
    }

    pstAcd = (ST_UTFC_ACD*)eos_lst_first(&pstFc->alsAcls[uiChanId]);
    while (NULL != pstAcd)
    {
        if (0 != pstAcd->uiLocIp)
        {
            pstAcd->uiLocIp = uiLocIp;
        }
        
        pstAcd = (ST_UTFC_ACD*)eos_lst_next(&pstAcd->node);
    }

    return EOS_OK;
}

_UINT32 UTFC_Show(H_UTFC hUtfc, EOS_INFO_PRINTF fpPrint,_VOID *pData)
{
    ST_UTFC_DESC* pstFc = NULL;
    EOS_INFO_PRINTF fpPrintf = fpPrint ? fpPrint : EOS_DEFAULT_INFO_PRINT_FUNC;

    if (NULL == hUtfc)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }
    
    pstFc = (ST_UTFC_DESC*)UT_MALLOC(sizeof(ST_UTFC_DESC));
    if (NULL == pstFc)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return EOS_ERROR;
    }

    memcpy(pstFc, hUtfc, sizeof(ST_UTFC_DESC));
    fpPrintf(pData, "Flux Control Statistics:\r\n\r\n");

    utfcAclShow(pstFc, fpPrintf, pData);
    fpPrintf(pData, "\r\n");

    utfcFiltersShow(pstFc, fpPrintf, pData);

    UT_FREE(pstFc);
    return EOS_OK;
}

_VOID utfcDescUpdate(ST_UTFC_DESC* pstFc, _UINT32 uiChanId)
{
    _UINT32 uiCurTicks = tickGet();
    _UINT32 uiNext = 0;
    _UINT32 i = 0;
    ST_UTFC_PKG_FILTER* pstNextFlt = NULL;

    uiNext = (pstFc->uiCurFilter + 1) % EOS_NELEMENTS(pstFc->aastFilters[0]);

    memset(&pstFc->aastFilters[0][uiNext], 0, sizeof(ST_UTFC_PKG_FILTER));
    memset(&pstFc->aastFilters[1][uiNext], 0, sizeof(ST_UTFC_PKG_FILTER));

    utfcDescPkgsPerTalkUpdate(pstFc, ASYS_WAN_UNIT);
    utfcDescPkgsPerTalkUpdate(pstFc, ASYS_LAN_UNIT);
    utfcDescDataPassRateUpdate(pstFc, uiChanId);

    for (i=0; i<EOS_NELEMENTS(pstFc->aastFilters); i++)
    {
        pstNextFlt = &pstFc->aastFilters[i][uiNext];
        pstNextFlt->uiTimeBegin = uiCurTicks;
        pstNextFlt->uiTimeCur = uiCurTicks;
        pstNextFlt->uiTimeEnd = uiCurTicks + pstFc->uiTimePerPiece;
    }
    
    pstFc->uiCurFilter = uiNext;
}

_VOID utfcDescPkgsPerTalkUpdate(ST_UTFC_DESC* pstFc, _UINT32 uiChanId)
{
    _UINT32 uiMaxTalkCnt = 0;
    _UINT32 uiCur = 0;
    _UINT32 uiNext = 0;
    ST_UTFC_PKG_FILTER* pstFlt = &pstFc->aastFilters[uiChanId][0];

    uiCur = pstFc->uiCurFilter;
    uiNext = (uiCur + 1) % EOS_NELEMENTS(pstFc->aastFilters[0]);

    uiMaxTalkCnt = utfcGetTalkCntHookInvoke(pstFc, uiChanId);
    pstFlt[uiNext].uiMaxTalkCnt = uiMaxTalkCnt;
    pstFlt[uiNext].uiPkgsRcvdPerTalk = pstFlt[uiCur].uiPkgsRcvdPerTalk;
    pstFlt[uiNext].uiPkgsSentPerTalk = pstFlt[uiCur].uiPkgsSentPerTalk;
    
    if (uiMaxTalkCnt == pstFlt[uiCur].uiMaxTalkCnt)
    {
        if (0 != uiMaxTalkCnt)
        {
            _INT32 iTmp;
                
            iTmp = (pstFlt[uiCur].uiPkgsRcvdPerTalk << 2) 
                  + pstFlt[uiCur].uiVoiceRcvd / uiMaxTalkCnt
                  + pstFlt[uiCur].uiResiRcvdPerTalk;
            pstFlt[uiNext].uiPkgsRcvdPerTalk = iTmp / 5;
            pstFlt[uiNext].uiResiRcvdPerTalk = iTmp % 5;

            iTmp = (pstFlt[uiCur].uiPkgsSentPerTalk << 2) 
                  + pstFlt[uiCur].uiVoiceSent / uiMaxTalkCnt
                  + pstFlt[uiCur].uiResiSentPerTalk;
            pstFlt[uiNext].uiPkgsSentPerTalk = iTmp / 5;
            pstFlt[uiNext].uiResiSentPerTalk = iTmp % 5;
        }
    }
    else if (uiMaxTalkCnt > pstFlt[uiCur].uiMaxTalkCnt)
    {
        pstFlt[uiCur].uiMaxTalkCnt = uiMaxTalkCnt;
    }
}

_VOID utfcDescDataPassRateUpdate(ST_UTFC_DESC* pstFc, _UINT32 uiChanId)
{
    _INT32 iPkgsDist = 0;
    _INT32 iPkgsDist0 = 0;
    _INT32 iPkgsDist1 = 0;
    _UINT32 uiCur = 0;
    _UINT32 uiPre = 0;
    _UINT32 uiNext = 0;
    _UINT32 uiPrePre = 0;
    _UINT32 uiTemp = 0;
    ST_UTFC_PKG_FILTER* pstFlt = &pstFc->aastFilters[ASYS_WAN_UNIT][0];
    ST_UTFC_PKG_FILTER* pstLanFlt = &pstFc->aastFilters[ASYS_LAN_UNIT][0];

    uiCur = pstFc->uiCurFilter;
    uiPre = (uiCur - 1) % EOS_NELEMENTS(pstFc->aastFilters[0]);
    uiNext = (uiCur + 1) % EOS_NELEMENTS(pstFc->aastFilters[0]);
    uiPrePre = (uiPre - 1) % EOS_NELEMENTS(pstFc->aastFilters[0]);

    pstFlt[uiNext].uiRecvDataPassRate = UTFC_FULL_PASSRATE;
    pstFlt[uiNext].uiSendDataPassRate = UTFC_FULL_PASSRATE;
    pstFlt[uiNext].uiMaxPkgsRecv = pstFlt[uiCur].uiMaxPkgsRecv;
    pstFlt[uiNext].uiMaxPkgsSend = pstFlt[uiCur].uiMaxPkgsSend;

    iPkgsDist1 = pstFlt[uiCur].uiDataRcvd - pstFlt[uiPre].uiDataRcvd;
    iPkgsDist0 = pstFlt[uiPre].uiDataRcvd - pstFlt[uiPrePre].uiDataRcvd;
    iPkgsDist = iPkgsDist1 + ((iPkgsDist1 - iPkgsDist0) >> 3);
    pstFlt[uiNext].uiDataRecvEstimate = pstFlt[uiCur].uiDataRcvd + iPkgsDist;
    if (pstFlt[uiNext].uiDataRecvEstimate <= 0)
    {
        pstFlt[uiNext].uiDataRecvEstimate = 1;
    }
    
    iPkgsDist1 = pstFlt[uiCur].uiDataSent - pstFlt[uiPre].uiDataSent;
    iPkgsDist0 = pstFlt[uiPre].uiDataSent - pstFlt[uiPrePre].uiDataSent;
    iPkgsDist = iPkgsDist1 + ((iPkgsDist1 - iPkgsDist0) >> 3);
    pstFlt[uiNext].uiDataSendEstimate = pstFlt[uiCur].uiDataSent + iPkgsDist;
    if (pstFlt[uiNext].uiDataSendEstimate <= 0)
    {
        pstFlt[uiNext].uiDataSendEstimate = 1;
    }

    if (pstFlt[uiCur].uiMaxTalkCnt > 0)
    {
        _INT32 iVoiceLmt = 0;
        _INT32 iTotPkgs = 0;

        iTotPkgs = pstFlt[uiCur].uiDataRcvd + pstFlt[uiCur].uiVoiceRcvd;
        iVoiceLmt = pstFlt[uiCur].uiPkgsRcvdPerTalk * pstFlt[uiCur].uiMaxTalkCnt;    
        if (pstFlt[uiCur].uiVoiceRcvd * 100 < iVoiceLmt * pstFc->uiMinVoicePassRate)
        {
            uiTemp = iTotPkgs - (iVoiceLmt - pstFlt[uiCur].uiVoiceRcvd);
            if (uiTemp < pstFlt[uiCur].uiMaxPkgsRecv)
            {
                pstFlt[uiNext].uiMaxPkgsRecv 
                    = ((pstFlt[uiCur].uiMaxPkgsRecv << 2) + uiTemp + 4) / 5;
            }
        }
        else if (iTotPkgs > pstFlt[uiCur].uiMaxPkgsRecv)
        {
            pstFlt[uiNext].uiMaxPkgsRecv 
                = ((pstFlt[uiCur].uiMaxPkgsRecv << 2) + iTotPkgs + 4) / 5;
        }

        pstFlt[uiNext].uiMaxDatasRecv = pstFlt[uiNext].uiMaxPkgsRecv - iVoiceLmt;
        pstFlt[uiNext].uiRecvDataPassRate 
            = pstFlt[uiNext].uiMaxDatasRecv * UTFC_FULL_PASSRATE 
            / pstFlt[uiNext].uiDataRecvEstimate;
        
        iTotPkgs = pstFlt[uiCur].uiDataSent+ pstFlt[uiCur].uiVoiceSent;
        iVoiceLmt = pstFlt[uiCur].uiPkgsSentPerTalk * pstFlt[uiCur].uiMaxTalkCnt;    
        if (pstFlt[uiCur].uiVoiceSent * 100 < iVoiceLmt * pstFc->uiMinVoicePassRate)
        {
            uiTemp = iTotPkgs - (iVoiceLmt - pstFlt[uiCur].uiVoiceSent);
            if (uiTemp < pstFlt[uiCur].uiMaxPkgsSend)
            {
                pstFlt[uiNext].uiMaxPkgsSend 
                    = ((pstFlt[uiCur].uiMaxPkgsSend << 2) + uiTemp + 4) / 5;
            }
        }
        else if (iTotPkgs > pstFlt[uiCur].uiMaxPkgsSend)
        {
            pstFlt[uiNext].uiMaxPkgsSend 
                = ((pstFlt[uiCur].uiMaxPkgsSend << 2) + iTotPkgs + 4) / 5;
        }

        pstFlt[uiNext].uiMaxDatasSend = pstFlt[uiNext].uiMaxPkgsSend - iVoiceLmt;
        pstFlt[uiNext].uiSendDataPassRate 
            = pstFlt[uiNext].uiMaxDatasSend * UTFC_FULL_PASSRATE 
            / pstFlt[uiNext].uiDataSendEstimate;
    }
    
    if (pstFlt[uiNext].uiRecvDataPassRate >= UTFC_FULL_PASSRATE)
    {
        pstFlt[uiNext].uiRecvDataPassRate = UTFC_FULL_PASSRATE;
        pstFlt[uiNext].uiMaxDatasRecv = UTFC_MAX_PKGS_RECV;
    }

    if (pstFlt[uiNext].uiSendDataPassRate >= UTFC_FULL_PASSRATE)
    {
        pstFlt[uiNext].uiSendDataPassRate = UTFC_FULL_PASSRATE;
        pstFlt[uiNext].uiMaxDatasSend = UTFC_MAX_PKGS_SEND;
    }

    pstLanFlt[uiNext].uiRecvDataPassRate = pstFlt[uiNext].uiSendDataPassRate;
    pstLanFlt[uiNext].uiMaxDatasRecv = pstFlt[uiNext].uiMaxDatasRecv;
    
    pstFlt[uiNext].uiSendDataPassRate = UTFC_FULL_PASSRATE;
    pstFlt[uiNext].uiMaxDatasSend = UTFC_MAX_PKGS_SEND;
    pstLanFlt[uiNext].uiSendDataPassRate = UTFC_FULL_PASSRATE;
    pstLanFlt[uiNext].uiMaxDatasSend = UTFC_MAX_PKGS_SEND;

    uiTemp = utfcGetCpuUsage();
    if (uiTemp > pstFc->uiMaxCpuUsage)
    {        
        if ((pstFlt[uiCur].uiDataRcvdPassed >> 1) < pstFlt[uiNext].uiMaxDatasRecv)
        {
            pstFlt[uiNext].uiRecvDataPassRate *= (pstFlt[uiCur].uiDataRcvdPassed >> 1);
            pstFlt[uiNext].uiRecvDataPassRate /= pstFlt[uiNext].uiMaxDatasRecv;
            pstFlt[uiNext].uiMaxDatasRecv = pstFlt[uiCur].uiDataRcvdPassed >> 1;
        }
        
        if ((pstLanFlt[uiCur].uiDataRcvdPassed >> 1) < pstLanFlt[uiNext].uiMaxDatasRecv)
        {
            pstLanFlt[uiNext].uiRecvDataPassRate *= (pstLanFlt[uiCur].uiDataRcvdPassed >> 1);
            pstLanFlt[uiNext].uiRecvDataPassRate /= pstLanFlt[uiNext].uiMaxDatasRecv;
            pstLanFlt[uiNext].uiMaxDatasRecv = pstLanFlt[uiCur].uiDataRcvdPassed >> 1;
        }

        UT_INFO2(" cpu usage %d%% exceeded, new pass rate %d%%!", 
            uiTemp, pstFlt[uiNext].uiRecvDataPassRate);
    }
}

_UINT32 utfcGetCpuUsage()
{
    eos_cpu_stat_t cstat;
    
    eos_get_cpustat(&cstat, 100);
    
    return 100 - cstat.cs_idle;
}

_BOOL utfcIsIp(_VOID** ppvMblk, _VOID** ppIp)
{
#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
    M_BLK_ID pMblk = (M_BLK_ID)(*ppvMblk);
    _VOID* pIp = NULL;
    _USHORT16 usProto = 0;

    memcpy(&usProto, &((struct ether_header*)pMblk->m_data)->ether_type, sizeof(usProto));
    usProto = eosConvertNetworkToHost16(usProto);
    
    if (UT_ETHERTYPE_IP == usProto)
    {
        if (pMblk->m_len > sizeof(struct ether_header))
        {
            pIp = pMblk->m_data + sizeof(struct ether_header);
        }
        else
        {
            pMblk = pMblk->m_next;
            pIp = pMblk->m_data;
        }
    }
    else if (UT_ETHERTYPE_PPPOE_SESS == usProto)
    {
        _CHAR8* pcPppHdr = NULL;

        if (pMblk->m_len > sizeof(struct ether_header))
        {
            pcPppHdr = pMblk->m_data + sizeof(struct ether_header);
        }
        else
        {
            pMblk = pMblk->m_next;
            pcPppHdr = pMblk->m_data;
        }

        memcpy(&usProto, &((ST_UT_PPPOE*)pcPppHdr)->usProto, sizeof(usProto));
        if (UT_PPPOEPROTO_IP == eosConvertNetworkToHost16(usProto))
        {
            if (pMblk->m_data + pMblk->m_len > pcPppHdr + sizeof(ST_UT_PPPOE))
            {
                pIp = pcPppHdr + sizeof(ST_UT_PPPOE);
            }
            else
            {
                pMblk = pMblk->m_next;
                pIp = pMblk->m_data;
            }
        }
        else
        {
            return EOS_FALSE;
        }
    }
    else
    {
        return EOS_FALSE;
    }

    *ppvMblk = pMblk;
    *ppIp = pIp;
    
    return EOS_TRUE;    
#else
    return EOS_FALSE;
#endif    
}

_INT32 utfcIpGetUdpTcp(_VOID** ppvMblk, _VOID* pvIpHdr, _VOID** ppvUdpTcp)
{
#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
    M_BLK_ID pMblk = (M_BLK_ID)(*ppvMblk);
    ST_UT_IP stIp;
    int len;

    if (pMblk == NULL)
    {
        *ppvUdpTcp = NULL;
        return -1;
    }

    memcpy(&stIp, pvIpHdr, sizeof(_UINT32));

    len = UtIpGetHdrLen(&stIp) + ((_CHAR8*)pvIpHdr - pMblk->m_data);
    
    do {
        if (pMblk->m_len > len)
        {
            *ppvMblk = pMblk;
            *ppvUdpTcp = pMblk->m_data + len;
            return len;
        }

        len -= pMblk->m_len;
        pMblk = pMblk->m_next;
    } while (pMblk != NULL);
    
    *ppvUdpTcp = NULL;
#else
    *ppvUdpTcp = NULL;
#endif

    return -1;
}

int utfcIpMblkGetBuf(_VOID** ppMblk, int offset, int len, void* buf)
{
#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
    M_BLK_ID pMblk = (M_BLK_ID)(*ppMblk);
    int nRead = 0;
    
    if (pMblk == NULL)
    {
        memset(buf, 0, len);
        return 0;
    }

    if (pMblk->m_len >= offset + len)
    {
        memcpy(buf, pMblk->m_data + offset, len);
        return len;
    }

    memcpy(buf, pMblk->m_data + offset, pMblk->m_len - offset);
    nRead = pMblk->m_len - offset;
        
    while ((pMblk = pMblk->m_next))
    {
        if (pMblk->m_len >= len - nRead)
        {
            memcpy(buf + nRead, pMblk->m_data, len - nRead);
            *ppMblk = pMblk;
            
            return len;
        }

        memcpy(buf + nRead, pMblk->m_data, pMblk->m_len);
        nRead += pMblk->m_len;
    }
    
    return nRead;
#else
    memset(buf, 0, len);
    return 0;
#endif
}

_BOOL utfcIsVoicePkgHookInvoke(ST_UTFC_DESC* pstFc, _UINT32 uiChanId, _UINT32 uiIpAddr)
{
    if (NULL == pstFc->fpIsVoicePkg)
    {
        return EOS_TRUE;
    }

    return pstFc->fpIsVoicePkg(uiChanId, uiIpAddr);
}

_UINT32 utfcGetTalkCntHookInvoke(ST_UTFC_DESC* pstFc, _UINT32 uiChanId)
{
    if (NULL == pstFc->fpGetTalkCnt)
    {
        return 0;
    }

    return pstFc->fpGetTalkCnt(uiChanId);
}

_BOOL utfcAclFilter(_LIST* lsAcl, 
                   _UINT32 uiProto,
                      _UINT32 uiLocIp,                      
                      _USHORT16 usLocPort,
                      _UINT32 uiRemIp,
                      _USHORT16 usRemPort)
{
    ST_UTFC_ACD* pstAcd = (ST_UTFC_ACD*)eos_lst_first(lsAcl);

    while (NULL != pstAcd)
    {
        if (uiProto == (uiProto & pstAcd->uiProto))
        {
            if ( ((uiLocIp == pstAcd->uiLocIp) || (0 == pstAcd->uiLocIp))
                && ((uiRemIp == pstAcd->uiRemIp) || (0 == pstAcd->uiRemIp))
                && ((usLocPort == pstAcd->usLocPort) || (0 == pstAcd->usLocPort))
                && ((usRemPort == pstAcd->usRemPort) || (0 == pstAcd->usRemPort)) )
            {
                return EOS_TRUE;
            }
        }
        
        pstAcd = (ST_UTFC_ACD*)eos_lst_next(&pstAcd->node);
    }

    return EOS_FALSE;
}

_UINT32 utfcAclShow(ST_UTFC_DESC* pstFc, EOS_INFO_PRINTF fpPrint,_VOID *pData)
{
    _UINT32 i = 0;
    _UINT32 uiTotNum = 0;
    _CHAR8 acInfo[128];

    for (i=0; i<EOS_NELEMENTS(pstFc->alsAcls); i++)
    {
        uiTotNum += eos_lst_count(&pstFc->alsAcls[i]);
    }
    
     sprintf(acInfo, "ACL Total Numbers: %d\r\n", uiTotNum);
    fpPrint(pData, acInfo);

    if (uiTotNum > 0)
    {
        sprintf(acInfo, "%10s|%22s|%22s|%10s\r\n", 
            "Interface", "Local", "Remote", "Portocol");
        fpPrint(pData, acInfo);
    }
    
    for (i=0; i<EOS_NELEMENTS(pstFc->alsAcls); i++)
    {
        _CHAR8 acIfName[8];
        ST_UTFC_ACD* pstAcd;

        pstAcd = (ST_UTFC_ACD*)eos_lst_first(&pstFc->alsAcls[i]);
        sprintf(acIfName, "%s%d", ASYS_IFNAME, i);
        
        while (NULL != pstAcd)
        {
            _CHAR8 acLocal[32];
            _CHAR8 acRemote[32];
            _CHAR8 acProto[8]  = {""};

            eos_address_ipv4_to_string(acLocal, sizeof(acLocal), pstAcd->uiLocIp);
            eos_address_ipv4_to_string(acRemote, sizeof(acRemote), pstAcd->uiRemIp);
            sprintf(&acLocal[strlen(acLocal)], ":%d", pstAcd->usLocPort);
            sprintf(&acRemote[strlen(acRemote)], ":%d", pstAcd->usRemPort);

            if (pstAcd->uiProto & UTFC_IPPROTO_UDP)
            {
                strcat(acProto, "UDP");
            }

            if (pstAcd->uiProto & UTFC_IPPROTO_TCP)
            {
                strcat(acProto, "TCP");
            }

            sprintf(acInfo, "%10s|%22s|%22s|%10s\r\n", 
                acIfName, acLocal, acRemote, acProto);
            fpPrint(pData, acInfo);
            
            pstAcd = (ST_UTFC_ACD*)eos_lst_next(&pstAcd->node);
        }
    }

    return EOS_OK;
}

_UINT32 utfcFiltersShow(ST_UTFC_DESC* pstFc, EOS_INFO_PRINTF fpPrint,_VOID *pData)
{
    _UINT32 i = 0;
    _CHAR8 acInfo[128];

     sprintf(acInfo, "Flux Control Current: %d\r\n", pstFc->uiCurFilter);
    fpPrint(pData, acInfo);
    
    sprintf(acInfo, "%8s|%10s|%8s|%10s|%10s|%10s|%10s\r\n", 
        "Voice", "Data", "Code", "Max", "Esti", "Rate", "Pass");
    fpPrint(pData, acInfo);
    
    for (i=0; i<EOS_NELEMENTS(pstFc->aastFilters); i++)
    {
        _UINT32 j = pstFc->uiCurFilter;
        ST_UTFC_PKG_FILTER* pstFlt = &pstFc->aastFilters[i][0];

        sprintf(acInfo, "%8s|%10s|%8s|%10s|%10s|%10s|%10s\r\n", 
            "", "", "", "", "", "", "");
        fpPrint(pData, acInfo);
        
        do
        {
            _CHAR8 acVoice[32], acData[32], acCode[32];
            _CHAR8 acMax[32], acEsti[32], acRate[32], acPass[32];
            
            sprintf(acVoice, "%d:%d", pstFlt[j].uiVoiceRcvd, pstFlt[j].uiVoiceSent);
            sprintf(acData, "%d:%d", pstFlt[j].uiDataRcvd, pstFlt[j].uiDataSent);
            sprintf(acCode, "%d:%d", pstFlt[j].uiPkgsRcvdPerTalk, pstFlt[j].uiPkgsSentPerTalk);
            sprintf(acMax,     "%d:%d", pstFlt[j].uiMaxPkgsRecv, pstFlt[j].uiMaxPkgsSend);
            sprintf(acEsti, "%d:%d", pstFlt[j].uiDataRecvEstimate, pstFlt[j].uiDataSendEstimate);
            sprintf(acRate, "%d:%d", pstFlt[j].uiRecvDataPassRate, pstFlt[j].uiSendDataPassRate);
            sprintf(acPass, "%d:%d", pstFlt[j].uiDataRcvdPassed, pstFlt[j].uiDataSentPassed);

            sprintf(acInfo, "%8s|%10s|%8s|%10s|%10s|%10s|%10s\r\n", 
                acVoice, acData, acCode, acMax, acEsti, acRate, acPass);
            fpPrint(pData, acInfo);

            j = (j + EOS_NELEMENTS(pstFc->aastFilters[0]) - 1) % EOS_NELEMENTS(pstFc->aastFilters[0]);
        }while(j != pstFc->uiCurFilter);
    }

    return EOS_OK;
}

#endif

#if defined(__cplusplus)
}
#endif

