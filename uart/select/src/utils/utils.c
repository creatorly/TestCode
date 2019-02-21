
#include <ctype.h>

#ifdef __VXWORKS_OS__
#include <vxWorks.h>
#include <taskLib.h>
#include <ftpLib.h>
#include <ioLib.h>
#endif

#include <eos.h>
#include <eos_time.h>
#include <utl_string.h>
#include <utl_md5.h>
#include <utl_pudp.h>
#include <utl_crc32.h>
#include <utl_dns.h>
#include <utl_token.h>
#include <utl_addr.h>

#include "ut_ip.h"
#include "ut_fluxctl.h"
#include "ut_rtstat.h"
#include "ut_rtss.h"
#include "ut_fselect.h"

#include "ut_exc.h"
#include "ut_log.h"

#include "ut_debug.h"
#include "ut_mem.h"
#include "utils.h"

#define UT_DNS_UPDATE_SECOND 300

typedef struct tagstUtConfig
{
#ifdef ENABLE_UT_FLUXCTL
    ST_UTFC_CONFIG stFcCfg;
#endif
}ST_UT_CONFIG;

typedef struct tagstUtils
{
    _BOOL bIsInitialized;
    HIPLST hIpList;

#ifdef ENABLE_UT_FLUXCTL
    H_UTFC hFc;
#endif

    H_UT_RTSS hRtss;
    _VOID* rts;         /* the default RTS */
}ST_UTILS;

typedef struct stUtUdpRecvBuf
{
    SEM_ID sem;
    _CHAR8* pcBuf;
    _UINT32* puiLen;    
}ST_UT_UDPRECVBUF;

ST_UT_CONFIG g_stUtCfg;
ST_UTILS g_stUtils = {EOS_FALSE, NULL, NULL};

#if defined(__cplusplus)
extern "C" {
#endif

_VOID utMainProc(ST_UTILS* pstUtils);
_UINT32 utPortsParse(const _CHAR8* ports, _UCHAR8 ucMaxPort, _UINT32* puiPorts, _UCHAR8* pucPorts);

#ifdef ENABLE_UT_RTS
_VOID utRtsStatMain(_VOID* pPeriod);
#endif

typedef struct tagstUtilsPrintInfo
{
    EOS_INFO_PRINTF pi_printf;
    _VOID* pi_data;
}ST_UT_PRINT_INFO;

_INT32 utInfoPrintf(ST_UT_PRINT_INFO* print_info, const _CHAR8* fmt, ...)
{
    _CHAR8 buf[1024];
    va_list va;

    va_start(va, fmt);
    eos_vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    return print_info->pi_printf(print_info->pi_data, buf);
}

#ifdef ENABLE_UT_FLUXCTL

_VOID utFcConfig(ST_UTFC_CONFIG* pstCfg, _BOOL bAclEnabled)
{
    pstCfg->bAclEnable = bAclEnabled;
    
    pstCfg->uiTimePerPiece = 500;
    pstCfg->uiMaxCpuUsage = 90;
    pstCfg->uiMinVoicePassRate = 90;
}

#endif

_INT32 utExcStackDump(_VOID* data, const _CHAR8* buf)
{
    return UT_LogBuf(buf, strlen(buf));
}

_UINT32 UT_Init(_VOID)
{
    _UINT32 uiRet = EOS_ERROR;

    if (EOS_TRUE == g_stUtils.bIsInitialized)
    {
        printf("utils already initialized!\r\n");
        return EOS_OK;
    }

    memset(&g_stUtils, 0, sizeof(g_stUtils));

    UT_LogInit();

    /* initialize the utils memory pool with the debug disabled */
    UT_ExcInit(utExcStackDump, NULL);

    utl_crc32_init();
    utl_pudp_init(ACOM_PORT_NUM*8, 8192);

    utl_dns_init(60);

#ifdef ENABLE_UT_RTS
    uiRet = UT_RTSS_Construct(&g_stUtils.hRtss);
    if (EOS_OK != uiRet)
    {
        printf("utils initialize RTSS failed!\r\n");
        return uiRet;
    }

    UT_RtsConstruct(&g_stUtils.rts, __FILE__, __LINE__, NULL, NULL);
#endif

    if (UT_TaskMgrInit())
    {
        printf("utils initialize task manager failed!\r\n");
        return EOS_ERROR;
    }

    if (UT_FSelectInit(120, 32*1024))
    {
        printf("utils initialize file select failed!\r\n");
        return EOS_ERROR;
    }

    return EOS_OK;
}

_UINT32 UT_Init2(_BOOL bAclEnabled)
{
    _UINT32 uiRet = EOS_ERROR;

#ifdef ENABLE_UT_DNSIP
    uiRet = utIpContruct(&g_stUtils.hIpList);
    if (EOS_OK != uiRet)
    {
        printf("utils initialize UTIP failed!\r\n");
        return uiRet;
    }
#endif

#ifdef ENABLE_UT_FLUXCTL
    utFcConfig(&g_stUtCfg.stFcCfg, bAclEnabled);

    uiRet = UTFC_Construct(&g_stUtils.hFc, &g_stUtCfg.stFcCfg);
    if (EOS_OK != uiRet)
    {
        printf("utils initialize UTFC failed!\r\n");
        return uiRet;
    }
#endif

    g_stUtils.bIsInitialized = EOS_TRUE;
    
    return EOS_OK;
}

_UINT32 UT_Start(EOS_TIMER_ID TimerGID)
{
#if  (ACOM_HW_TYPE != HWUUWIFI)
    _UINT32 uiRet = EOS_OK;

    if ((uiRet = UT_LogStart(TimerGID)))
    {
        return uiRet;
    }
#endif

#ifdef ENABLE_UT_DNSIP
    eos_task_start("tUtDnsMain", 120, 64*1024, utMainProc, &g_stUtils);
#endif

#ifdef ENABLE_UT_RTS
#if 0
    eos_wdg_add_timer(30*1000, (_VOID(*)(_VOID*)) utRtsStat, NULL);
#else
    eos_task_start("utRtsMain", 80, 64*1024, utRtsStatMain, (_VOID*)30);
#endif
#endif

    return UT_FSelectStart("tUtFsMain");
}

_UINT32 UT_Uninit(_VOID)
{
    if (EOS_FALSE == g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_FALSE;
    }

#ifdef ENABLE_UT_RTS
    UT_RTSS_Destruct(g_stUtils.hRtss);
#endif

#ifdef ENABLE_UT_FLUXCTL
    UTFC_Destruct(g_stUtils.hFc);
#endif

#ifdef ENABLE_UT_DNSIP
    utIpDestruct(g_stUtils.hIpList);
#endif

    memset(&g_stUtils, 0, sizeof(ST_UTILS));
    g_stUtils.bIsInitialized = EOS_FALSE;
    
    return EOS_OK;
}


_UINT32 UT_Show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData)
{
    pPrintFunc(pData, "\r\nUt Statistics Begin >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n"); 

    UT_MemShow((ut_mem_print_fp)pPrintFunc, pData);
    pPrintFunc(pData, "\r\n");

    UT_LogStat(pPrintFunc, pData);
    pPrintFunc(pData, "\r\n");

#ifdef ENABLE_UT_DNSIP
    utIpShow(g_stUtils.hIpList, pPrintFunc, pData);
    pPrintFunc(pData, "\r\n");
#endif

#if 0
    UTFC_Show(g_stUtils.hFc, pPrintFunc, pData);
    pPrintFunc(pData, "\r\n");
#endif

#ifdef ENABLE_UT_RTS
    pPrintFunc(pData, "\r\nUT RTS Statistics:\r\n"); 

    ST_UT_PRINT_INFO printInfo;
    printInfo.pi_printf = pPrintFunc;
    printInfo.pi_data = pData;
    
    UT_RTSS_Show(g_stUtils.hRtss, (FP_UT_RTS_PRINTF) utInfoPrintf, &printInfo);
    
    pPrintFunc(pData, "\r\n");
#endif

    UT_TaskMgrStat(pPrintFunc, pData);
    pPrintFunc(pData, "\r\n");

    pPrintFunc(pData, "Ut Statistics End >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
    return EOS_OK;
}

int UT_DnsAdd(const char* resolv_conf, char* dns_list)
{
    FILE* fp = fopen(resolv_conf, "w");
    
    if (NULL == fp)
    {
        UT_WARN(-1, EOS_ERROR, "open file failed!", "filename = %s", resolv_conf);
        return EOS_ERROR;
    }

    utl_token_t tok;
    const char* dns;
    int dns_cnt = 0;
    char dns_addrs[128];

    eos_buf_strcpy(dns_addrs, dns_list);
    utl_token_init(&tok, ",", dns_addrs, false);
    
    while ((dns = utl_token_get(&tok)))
    {
        if (utl_addr_is_ipv4(dns))
        {
            fprintf(fp, "nameserver %s\n", dns);
            dns_cnt++;
        }
    }
    
    utl_token_free(&tok);

    if (dns_cnt < 1)
    {
        /* add a default DNS server */
        fprintf(fp, "nameserver 8.8.8.8\n");
    }
    
    fclose(fp);

    return EOS_OK;
}

#ifdef ENABLE_UT_DNSIP

_UINT32 UT_DnsIpAdd(const _CHAR8* pcIp)
{
    if (EOS_TRUE != g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_FALSE;
    }

    return utIpAddAddr(g_stUtils.hIpList, pcIp, NULL, NULL, NULL);
}

_UINT32 UT_DnsIpAddEx(const _CHAR8* pcIp, FP_UT_IPUPDATE fpUpdate, _VOID* data)
{
    if (EOS_TRUE != g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_FALSE;
    }
    
    return utIpAddAddr(g_stUtils.hIpList, pcIp, fpUpdate, NULL, data);
}

_UINT32 UT_DnsIpAddEx2(const _CHAR8* pcIp, FP_UT_IPUPDATE fpUpdate, FP_UT_IPRESOLVFAIL fpResolvFail, _VOID* data)
{
    if (EOS_TRUE != g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_FALSE;
    }
    
    return utIpAddAddr(g_stUtils.hIpList, pcIp, fpUpdate, fpResolvFail, data);
}

_UINT32 UT_DnsIpDel(_CHAR8* pcIp, FP_UT_IPUPDATE fpUpdate)
{
    if (EOS_TRUE != g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_FALSE;
    }
    
    return utIpDelAddr(g_stUtils.hIpList, pcIp, fpUpdate);
}

_UINT32 UT_DnsIpGet(const _CHAR8* pcIp, _CHAR8* pcNewIp, _UINT32* puiNewIp)
{
    if (EOS_TRUE != g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_ERROR;
    }
    
    return utIpGetAddr(g_stUtils.hIpList, pcIp, pcNewIp, puiNewIp);
}

_UINT32 UT_DnsShow(EOS_INFO_PRINTF pPrintFunc,_VOID *pData)
{
    if (EOS_TRUE != g_stUtils.bIsInitialized)
    {
        printf("utils not initialized!\r\n");
        return EOS_FALSE;
    }
    
    return utIpShow(g_stUtils.hIpList, pPrintFunc, pData);
}

#endif

#ifdef ENABLE_UT_FLUXCTL

_BOOL UT_FcRecvHook(_UINT32 uiChanId, _VOID* pMblk)
{
    return UTFC_RecvHook(g_stUtils.hFc, uiChanId, pMblk);
}

_BOOL UT_FcSendHook(_UINT32 uiChanId, _VOID* pMblk)
{
    return UTFC_SendHook(g_stUtils.hFc, uiChanId, pMblk);
}

_UINT32 UT_FcAclAdd(_UINT32 uiIfNum, 
                     EN_UTFC_IPPROTO enProto,
                    _CHAR8* pcLocIp, 
                    _USHORT16 usLocPort, 
                    _CHAR8* pcRemIp,
                    _USHORT16 usRemPort)
{
    _UINT32 auiProtos[] = 
        {
            UTFC_IPPROTO_UDP,
            UTFC_IPPROTO_TCP,
            UTFC_IPPROTO_UDPTCP
        };
    
    return UTFC_AclAdd(g_stUtils.hFc, uiIfNum, 
        auiProtos[enProto], pcLocIp, usLocPort, pcRemIp, usRemPort);
}

_UINT32 UT_FcAclUpdate(_UINT32 uiIfNum, const _CHAR8* pcLocIp)
{
    return UTFC_AclUpdate(g_stUtils.hFc, uiIfNum, pcLocIp);
}

_UINT32 UT_FcShow(EOS_INFO_PRINTF pPrintFunc,_VOID *pData)
{
    return UTFC_Show(g_stUtils.hFc, pPrintFunc, pData);
}

_UINT32 UT_FcIsVoicePkgHookAdd(FP_UT_FCISVOICEPKG fpIsVoice)
{
    return UTFC_IsVoicePkgHookAdd(g_stUtils.hFc, fpIsVoice);
}

_UINT32 UT_FcGetTalkCntHookAdd(FP_UT_FCGETTALKCNT fpGetTalkCnt)
{
    return UTFC_GetTalkCntHookAdd(g_stUtils.hFc, fpGetTalkCnt);
}

#endif

#ifdef ENABLE_UT_RTS

_VOID utRtsStat()
{
    utl_string rtsBuf = utl_str_init(1024);

    if (rtsBuf != NULL)
    {
        UT_RtsShowAll(utl_str_printf, rtsBuf);
        UT_LDBG3(-1, "UT rts stat:", "\r\n%s\r\n", utl_str_str(rtsBuf));

        utl_str_free(rtsBuf);
    }
}

_VOID utRtsStatMain(_VOID* pPeriod)
{
    _INT32 period = (_INT32) pPeriod;
    
    while (1)
    {
        taskDelay(period*sysClkRateGet());
        
        utRtsStat();
    }
}

_UINT32 UT_RtsConstruct(_VOID** phRts, 
                        const _CHAR8* pcFile,
                        _UINT32 uiLine,
                        _VOID* fpPrint,
                        _VOID* data)
{    
    H_UT_RTS hRts = NULL;
    
    if (NULL == phRts)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    *phRts = NULL;
    if (NULL == g_stUtils.hRtss)
    {
        UT_ERROR(EOS_ERROR, "utils not initialize RTSS!\r\n");
        return EOS_ERROR;
    }
    
    hRts = UT_RTSS_Add(g_stUtils.hRtss, pcFile, uiLine, fpPrint, data);
    if (NULL == hRts)
    {
        return EOS_ERROR;
    }

    *phRts = hRts;
    
    return EOS_OK;
}

_UINT32 UT_RtsDestruct(H_UT_RTS hRts)
{
    if (NULL == hRts)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    if (NULL == g_stUtils.hRtss)
    {
        UT_ERROR(EOS_ERROR, "utils not initialize RTSS!\r\n");
        return EOS_ERROR;
    }

    UT_RTSS_Del(g_stUtils.hRtss, hRts);
    
    return EOS_OK;
}

_UINT32 UT_RtsBegin(_VOID* hRts, const _CHAR8* file, _INT32 line)
{
    return UTRTS_Begin(hRts ? hRts : g_stUtils.rts, file, line);
}

_UINT32 UT_RtsEnd(_VOID* hRts, const _CHAR8* file, _INT32 line)
{
    return UTRTS_End(hRts ? hRts : g_stUtils.rts, file, line);
}

_VOID UT_RtsShow(_VOID* hRts, FP_UT_RTS_PRINTF fpPrint, _VOID* data)
{
    return UTRTS_Show(hRts ? hRts : g_stUtils.rts, fpPrint, data);
}

_VOID UT_RtsShowAll(FP_UT_RTS_PRINTF fpPrint, _VOID* data)
{
    UT_RTSS_Show(g_stUtils.hRtss, fpPrint, data);
}

#endif

_VOID UT_MemDump(_VOID* data, _UINT32 uiLen)
{
    _UCHAR8* pucData = (_UCHAR8*)data;
    _UINT32 i = 0;

    if (uiLen < 1)
    {
        return;
    }
    
    printf("%08x: ", ((_UINT32)pucData & 0xfffffff0));
    
    for (i=0; i<((_UINT32)pucData & 0xf); i++)
    {
        if (8 == i)
        {
            printf("- ");
        }
        printf(".. ");
    }

    printf("%02x ", pucData[0]);
    
    for (i=1; i<uiLen; i++)
    {                
        if (0 == ((_UINT32)&pucData[i] & 0xf))
        {
            printf("\r\n%08x: ", ((_UINT32)&pucData[i] & 0xfffffff0));
        }
        else if (0 == ((_UINT32)&pucData[i] & 0x7))
        {
            printf("- ");
        }

        printf("%02x ", pucData[i]);
    }

    for (; (_UINT32)&pucData[i] & 0xf; i++)
    {
        if (0 == ((_UINT32)&pucData[i] & 0x7))
        {
            printf("- ");
        }

        printf(".. ");    
    }
    
    printf("\r\n");
}

_INT32 UT_StrCaseCmp(const _CHAR8* s1, const _CHAR8* s2)
{
    for (; *s1 && *s2; ++s1, ++s2)
    {
        _INT32 ret = toupper(*s1) - toupper(*s2);

        if (ret != 0)
        {
            return ret;
        }
    }
    
    return *s1 ? 1 : (*s2 ? -1 : 0);
}

_INT32 UT_StrNCaseCmp(const _CHAR8* s1, const _CHAR8* s2, _UINT32 n)
{
    for (; *s1 && *s2 && n > 0; ++s1, ++s2, --n)
    {
        _INT32 ret = toupper(*s1) - toupper(*s2);

        if (ret != 0)
        {
            return ret;
        }
    }

    if (*s1 && *s2 && n > 0)
    {
        return toupper(*s1) - toupper(*s2);
    }
    
    return 0;
}

_BOOL UT_StrIsDigit(const _CHAR8* str)
{
    while (*str && strchr("0123456789", *str))
    {
        str++;
    }

    return ('\0' == *str);
}

_CHAR8* UT_StrSkpSpc(_CHAR8* str)
{
    while (isspace(*str))
    { 
        str++; 
    }

    return str;
}

_UINT32 UT_PortsParse(const _CHAR8* ports, _UCHAR8* pucPorts, _UCHAR8 ucMaxPort)
{    
    _INT32 i = 0;

    _UINT32 auiPorts[0x100/UT_BITSOF(_UINT32)];

    if (!strcmp(ports, "all") || !strcmp(ports, "*"))
    {
        for (i = 0; i < ucMaxPort; i++)
        {
            pucPorts[i] = i;
        }

        return ucMaxPort;
    }

    for (i = 0; i < EOS_NELEMENTS(auiPorts); ++i)
    {
        auiPorts[i] = 0;
    }

    return utPortsParse(ports, ucMaxPort, auiPorts, pucPorts);
}

_UINT32 UT_PortMaskParse(const _CHAR8* ports, _UINT32* puiPorts, _UINT32 uiMaxPort)
{    
    _INT32 i = 0;

    if (!strcmp(ports, "all") || !strcmp(ports, "*"))
    {
        for (i = 0; i < uiMaxPort/UT_BITSOF(_UINT32); i++)
        {
            puiPorts[i] = ~0;
        }

        if (uiMaxPort % UT_BITSOF(_UINT32))
        {
            puiPorts[i] = (1 << (uiMaxPort % UT_BITSOF(_UINT32))) - 1;
        }

        return uiMaxPort;
    }

    for (i = 0; i < (uiMaxPort+UT_BITSOF(_UINT32)-1)/UT_BITSOF(_UINT32); ++i)
    {
        puiPorts[i] = 0;
    }

    return utPortsParse(ports, uiMaxPort, puiPorts, NULL);
}

_INT32 UT_IdsParse(const _CHAR8** ppStrIds, _INT32* piIds, _INT32 maxIdNum)
{
    const _CHAR8* strIds = *ppStrIds;
    _INT32 i = 0;
    
    while (*strIds && (i < maxIdNum))
    {
        _INT32 id = -1;

        /* ignore all ',' and space */
        while (isspace((int)*strIds) || (',' == *strIds)) ++strIds;
        
        if (sscanf(strIds, "%d", &id) > 0)
        {            
            piIds[i++] = id;
        }

        /* find next ',' */
        while (*strIds && *strIds != ',') ++strIds;
    }

    *ppStrIds = strIds;

    return i;
}

const _CHAR8* UT_Mem2Str(const _VOID* pMem, _INT32 len, _CHAR8* buf, _INT32 size)
{
    /* assume the buf is enough */
    _UCHAR8* pucMem = (_UCHAR8*)pMem;

    _INT32 i = 0;
    _INT32 j = 0;

    if (len > (size - 1)/2)
    {
        len = (size - 1)/2;
    }

    for (; i < len; i++)
    {
        buf[j++] = "0123456789ABCDEF"[pucMem[i] >> 4];
        buf[j++] = "0123456789ABCDEF"[pucMem[i] & 0xf];
    }

    buf[j] = '\0';

    return buf;
}

_UINT32 utPortsParse(const _CHAR8* ports, _UCHAR8 ucMaxPort, _UINT32* puiPorts, _UCHAR8* pucPorts)
{    
    _UINT32 uiPorts = 0;
    _INT32 i = 0;
    
    while (*ports)
    {
        _INT32 iMinPort = -1;
        _INT32 iMaxPort = -1;

        /* ignore all ',' and space */
        while (isspace((int)*ports) || (',' == *ports)) ++ports;
        
        i = sscanf(ports, "%d - %d", &iMinPort, &iMaxPort);
        if (i > 0)
        {
            if (1 == i)
            {
                iMaxPort = iMinPort;
            }
        
            /* a range specified */
            if ((iMinPort < 1) || (iMaxPort > ucMaxPort))
            {
                return 0;
            }
            
            while (iMinPort <= iMaxPort)
            {
                _INT32 iPortNo = iMinPort - 1;

                if (!UT_PORTS_ISSET(puiPorts, iPortNo))
                {
                    UT_PORTS_SET(puiPorts, iPortNo);

                    if (pucPorts != NULL)
                    {
                        pucPorts[uiPorts] = (_UCHAR8)iPortNo;
                    }
                    
                    uiPorts++;
                }

                iMinPort++;
            }
        }

        /* find next ',' */
        while (*ports && *ports != ',') ++ports;
    }

    return uiPorts;
}

const _CHAR8* UT_FmtTime(time_t tv_sec, _CHAR8* buf, const _CHAR8* fmtYmdHms)
{
    static _CHAR8 g_strTime[32];
    struct tm ct;

    if (NULL == buf)
    {
        buf = g_strTime;
    }

    if (NULL == fmtYmdHms)
    {
        fmtYmdHms = "%04d-%02d-%02d %02d:%02d:%02d";
    }

    eos_localtime_r(&tv_sec, &ct);

    sprintf(buf, fmtYmdHms,
        ct.tm_year + 1900, ct.tm_mon + 1,
        ct.tm_mday, ct.tm_hour, ct.tm_min, ct.tm_sec);

    return buf;
}

_VOID utUdpReplyProcCB(_INT32 iSock,eosMsgEvents enEvent,_VOID* data)
{
    ST_UT_UDPRECVBUF* buf = (ST_UT_UDPRECVBUF*)data;

    eos_socket_receive_buffer(&iSock, buf->pcBuf, *(buf->puiLen),
                              (_INT32*)buf->puiLen, NULL);
    semGive(buf->sem);

    return;
}

_UINT32 UT_UdpSendRecv(eosSocket* locSock,
            _CHAR8* msg, _UINT32 len, eosAddress* remAddr, 
            _CHAR8* reply, _UINT32* replyLen, _UINT32 msec)
{
    _INT32 iRet = EOS_ERROR;
    size_t uiLenSent = 0;
    _UINT32 T0 = 0, T1 = 500, T2 = 4*1000;    /* milliseconds */
    static EOS_MSG_ID iMsgId = 0;
    ST_UT_UDPRECVBUF buf;

    if (iMsgId == 0)
    {
        if (eos_msg_create("msgTftpc", &iMsgId, NULL, NULL))
        {
            return EOS_ERROR;
        }
    }

    buf.pcBuf = reply;
    buf.puiLen = replyLen;
    
    buf.sem = semBCreate(0, SEM_EMPTY);

    /* eos_sem_v should be invoked in callback function */
    eos_msg_callon(NULL, iMsgId, *locSock, eosSelectRead, &utUdpReplyProcCB, &buf);

    do
    {
        eos_socket_send_buffer(locSock, msg, len, remAddr, &uiLenSent);

        if (T0 + T1 < msec)
        {
            iRet = semTake(buf.sem, T1*sysClkRateGet()/1000);
            
            T0 += T1;
            T1 = EOS_MIN(T1*2, T2);
        }
        else    /* the last timeout */
        {
            iRet = semTake(buf.sem, (msec - T0)*sysClkRateGet()/1000);
            
            if (EOS_OK != iRet)
            {
                *replyLen = 0;
            }
            
            break;
        }
    }while(EOS_OK != iRet);

    eos_msg_callon(NULL, iMsgId, *locSock, eosSelectRead, NULL, NULL);

    semDelete(buf.sem);
        
    return uiLenSent;
}

int UT_FtpGetFile(const char* host, const char* file, const char* usr, const char* pwd, char** pptr)
{
    int tot = 0;
    
#ifdef __VXWORKS_OS__
    int dataFd,ctrlFd,ret;
    char buf[1024];
    char* pBuf = NULL;
    int size = 0;

    *pptr = NULL;

    printf("UT_FtpGetFile: connect ftp server %s ...\r\n", host);
    
    if ((ctrlFd = ftpHookup((char*)host)) == EOS_ERROR)
    {
        printf("UT_FtpGetFile: failed to connect ftp server %s!\r\n", host);
        return EOS_ERROR;
    }         

    if (ftpLogin(ctrlFd, (char*)usr, (char*)pwd, "") == EOS_ERROR)
    {
        close(ctrlFd);

        printf("UT_FtpGetFile: failed to login ftp server %s!\r\n", host);
        return EOS_ERROR;
    }         

    if ((ftpCommandEnhanced(ctrlFd, "TYPE I", 0,0,0,0,0,0,0,0) == EOS_ERROR)
        || (ftpCommandEnhanced(ctrlFd, "SIZE %s", (int)file, 
                0, 0, 0, 0, 0, buf, sizeof(buf)) == EOS_ERROR))
    {
        ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
        close(ctrlFd);

        printf("UT_FtpGetFile: file \"%s\" not exist!\r\n", file);
        return EOS_ERROR;
    }         

    sscanf(buf, "%d %d", &ret, &size);
    if (size <= 0)
    {
        ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
        close(ctrlFd);

        printf("UT_FtpGetFile: file \"%s\" not exist! size = %d\r\n", file, size);
        return EOS_ERROR;
    }
    
    printf("UT_FtpGetFile: file \"%s\" size is %d\r\n", file, size);
    
    pBuf = (char *)UT_MALLOC(size);
    if(pBuf==NULL)
    {    
        printf("UT_FtpGetFile: failed to allocate memory(%d)!\r\n", size);

        ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
        close(ctrlFd);

        return EOS_ERROR;
    }
    
    printf("UT_FtpGetFile: start download file \"%s\" ... ", file);
    
    if ( ((dataFd = ftpDataConnInitPassiveMode(ctrlFd)) == EOS_ERROR)
        || (ftpCommandEnhanced(ctrlFd, "RETR %s", (int)file,0,0,0,0,0,0,0) == EOS_ERROR) )
    {
        printf("UT_FtpGetFile: Fail to connect!\r\n");

        if (dataFd != EOS_ERROR)
        {
            close(dataFd);
        }

        ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
        close(ctrlFd);

        return EOS_ERROR;
    }

    tot = 0;

    while ((ret = read(dataFd, buf, sizeof(buf))) > 0)
    {
        if ((tot + ret) > size)
        {
            printf("UT_FtpGetFile: Wrong(File size (%d) larger than (%d))\r\n", tot+ret, size);
            
            close(dataFd);
            ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
            close(ctrlFd);
            
            UT_FREE(pBuf);
            return EOS_ERROR;
        }

        memcpy(&pBuf[tot], buf, ret);
        
        if ((tot % (16*1024)) + ret >= (16*1024))
        {
            printf(".");
        }

        tot += ret;
    }

    close(dataFd);
    
    ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
    close(ctrlFd);

    *pptr = pBuf;
    
    printf("Done\r\n");
#else
    printf("UT_FtpGetFile: not implement!\r\n");
#endif

return tot;
}

int UT_FtpDelFile(const char* host, const char* file, const char* usr, const char* pwd)
{
#ifdef __VXWORKS_OS__
    int ctrlFd;
    char buf[128];

    printf("UT_FtpDelFile: connect ftp server %s ...\r\n", host);
    
    if ((ctrlFd = ftpHookup((char*)host)) == EOS_ERROR)
    {
        printf("UT_FtpDelFile: failed to connect ftp server %s!\r\n", host);
        return EOS_ERROR;
    }         

    if (ftpLogin(ctrlFd, (char*)usr, (char*)pwd, "") == EOS_ERROR)
    {
        close(ctrlFd);

        printf("UT_FtpDelFile: failed to login ftp server %s!\r\n", host);
        return EOS_ERROR;
    }         

    if (ftpCommandEnhanced(ctrlFd, "DELE %s", (int)file, 
                0, 0, 0, 0, 0, buf, sizeof(buf)) == EOS_ERROR)
    {
        ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
        close(ctrlFd);

        printf("UT_FtpDelFile: file \"%s\" not exist or no authorization!\r\n", file);
        return EOS_ERROR;
    }         
    
    ftpCommandEnhanced(ctrlFd, "QUIT", 0, 0, 0, 0, 0, 0, 0, 0);
    close(ctrlFd);
    
    printf("Done\r\n");
#else
    printf("UT_FtpDelFile: not implement!\r\n");
#endif

    return EOS_OK;
}

_VOID UT_Md5Digest(const _VOID* data, _INT32 len, const _CHAR8* pkey, _CHAR8 digest[32])
{
    utl_md5_ctx_t md5ctx;
    
    utl_md5_init(&md5ctx);
    utl_md5_update(&md5ctx, data, len);

    if (pkey != NULL)
    {
        utl_md5_update(&md5ctx, pkey, strlen(pkey));
    }

    utl_md5_final(&md5ctx);
    
    utl_md5_get_digest(&md5ctx, NULL, digest);
}

_INT32 UT_Md5Verify(const _VOID* data, _INT32 len, const _CHAR8* pkey)
{
    char digest[32+1];
    
    if (len <= 32)
    {
        printf("UT_Md5Verify: data length must be larger than 32!\r\n");
        return EOS_ERROR;
    }

    UT_Md5Digest(data, len - 32, pkey, digest);
    digest[32] = '\0';

    printf("UT_Md5Verify: the correct digest is: %s\r\n", digest);

    if (memcmp(digest, ((char*)data) + len - 32, 32))
    {
        memcpy(digest, ((char*)data) + len - 32, 32);
        printf("UT_Md5Verify: failed! the original digest is: %s\r\n", digest);
        
        return EOS_ERROR;
    }

    return EOS_OK;
}

_INT32 UT_FloatStr2Int(const _CHAR8* pcFloatStr, _INT32 bits)
{
    _INT32 i = 0;
    _INT32 iVal = atoi(pcFloatStr);
    const _CHAR8* pc = strchr(pcFloatStr, '.');

    if (pc != NULL)
    {
        for (++pc; i < bits && isdigit((int)pc[i]); ++i)
        {
            iVal = iVal*10 + (pc[i] - '0');
        }
    }

    for (; i < bits; i++)
    {
        iVal *= 10;
    }

    return iVal;
}

#ifdef ENABLE_UT_DNSIP

_VOID utMainProc(ST_UTILS* pstUtils)
{
    while (1)
    {
        utIpUpdatesAddr(pstUtils->hIpList);
        taskDelay(UT_DNS_UPDATE_SECOND * sysClkRateGet());
    }
}

#endif

#if defined(__cplusplus)
}
#endif

