
#if defined(__cplusplus)
extern "C" {
#endif

#define UTFC_FULL_PASSRATE  100
#define UTFC_DEF_PKGS_RECV  100
#define UTFC_DEF_PKGS_SEND  100
#define UTFC_MAX_PKGS_RECV  0x7fff
#define UTFC_MAX_PKGS_SEND  0x7fff

#ifdef UTFC_DEBUG
#define UTFC_INFO2(fmt2, arg1, arg2) UT_INFO2(fmt2, arg1, arg2)
#define UTFC_INFO4(fmt4, arg1, arg2, arg3, arg4) \
    UT_INFO4(fmt4, arg1, arg2, arg3, arg4)
#else
#define UTFC_INFO2(fmt2, arg1, arg2)
#define UTFC_INFO4(fmt4, arg1, arg2, arg3, arg4)
#endif

/* transport mode, recieve or send */
typedef enum
{
    EN_UTFC_TRANSMODE_RECV = 0,
    EN_UTFC_TRANSMODE_SEND,

    EN_UTFC_TRANSMODE_BUTT
}EN_UTFC_TRANSMODE;

/* package filter take effect in a specific peice of time */
typedef struct tagstUtfcPkgFilter
{
    _UINT32 uiTimeBegin;        /* begin time of piece, ticks if VxWorks */
    _UINT32 uiTimeEnd;          /* end time of piece, ticks if VxWorks */
    _UINT32 uiTimeCur;          /* current time, ticks if VxWorks */

    _SHORT16 uiVoiceRcvd;        /* voice package received */
    _SHORT16 uiVoiceSent;        /* voice package sent */
    _SHORT16 uiDataRcvd;         /* data package received */
    _SHORT16 uiDataSent;         /* data package sent */
    _SHORT16 uiDataRcvdPassed;   /* data to be recieve passed really */
    _SHORT16 uiDataSentPassed;   /* data to be send passed really */

    _SHORT16 uiPkgsRcvdPerTalk;  /* packages recieved of one talking in a piece of time */
    _SHORT16 uiPkgsSentPerTalk;  /* packages sent of one talking in a piece of time */
    _SHORT16 uiResiRcvdPerTalk;  /* residual packages recieved of one talking in a piece of time */
    _SHORT16 uiResiSentPerTalk;  /* residual packages sent of one talking in a piece of time */

    _SHORT16 uiMaxPkgsRecv;         /* max packages recieved maken no difference to voice */
    _SHORT16 uiMaxPkgsSend;      /* max packages sent maken no difference to voice */
    _USHORT16 uiMaxDatasRecv;     /* max data pkgs recieved maken no difference to voice */
    _USHORT16 uiMaxDatasSend;      /* max data pkgs sent maken no difference to voice */

    _SHORT16 uiDataRecvEstimate; /* data to be recieved estimately */
    _SHORT16 uiDataSendEstimate; /* data to be sent estimately */
    
    _SHORT16 uiRecvDataPassRate; /* percent of the data pkg pass rate */
    _SHORT16 uiSendDataPassRate; /* percent of the data pkg pass rate */

    _SHORT16 uiMinTalkCnt;       /* min talking count */
    _SHORT16 uiMaxTalkCnt;       /* max talking count */
}ST_UTFC_PKG_FILTER;

typedef struct tagstUtfcAcDesc
{
    _NODE node;

    _UINT32 uiLocIp;
    _UINT32 uiRemIp;
    _USHORT16 usLocPort;
    _USHORT16 usRemPort;
    _UINT32 uiProto;
}ST_UTFC_ACD;

/* flux control description */
typedef struct tagstUtfcDesc
{
    _UINT32 bAclEnable;         /* enable or disable the flux control */
    _UINT32 uiTimePerPiece;     /* time of a piece, ticks if VxWorks */
    _UINT32 uiMaxCpuUsage;      /* max percent usage of CPU */
    _UINT32 uiMinVoicePassRate; /* min percent pass rate of voice data */

    FP_UTFC_ISVOICEPKG fpIsVoicePkg;    /* pointer to function to tell if a pkg is voice */
    FP_UTFC_GETTALKCNT fpGetTalkCnt;    /* pointer to function to get the count of current talking */
    _LIST alsAcls[2];                    /* access control list */

    _UINT32 uiCurFilter;                /* current package filter */
    /* calculate data package pass-rate on the last 3 time pieces */
    ST_UTFC_PKG_FILTER aastFilters[2][4];
}ST_UTFC_DESC;

_VOID utfcDescUpdate(ST_UTFC_DESC* pstFc, _UINT32 uiChanId);
_VOID utfcDescPkgsPerTalkUpdate(ST_UTFC_DESC* pstFc, _UINT32 uiChanId);
_VOID utfcDescDataPassRateUpdate(ST_UTFC_DESC* pstFc, _UINT32 uiChanId);

_UINT32 utfcGetCpuUsage();

_BOOL utfcIsIp(_VOID** ppvMblk, _VOID** ppIp);
_BOOL utfcIsVoicePkgHookInvoke(ST_UTFC_DESC* pstFc, _UINT32 uiChanId, _UINT32 uiIpAddr);
_INT32 utfcIpGetUdpTcp(_VOID** ppvMblk, _VOID* pvIpHdr, _VOID** ppvUdpTcp);
_UINT32 utfcGetTalkCntHookInvoke(ST_UTFC_DESC* pstFc, _UINT32 uiChanId);

_BOOL utfcAclFilter(_LIST* lsAcl, 
                   _UINT32 uiProto,
                      _UINT32 uiLocIp,                      
                      _USHORT16 usLocPort,
                      _UINT32 uiRemIp,
                      _USHORT16 usRemPort);

_UINT32 utfcAclShow(ST_UTFC_DESC* pstFc, EOS_INFO_PRINTF fpPrint,_VOID *pData);
_UINT32 utfcFiltersShow(ST_UTFC_DESC* pstFc, EOS_INFO_PRINTF fpPrint,_VOID *pData);

int utfcIpMblkGetBuf(_VOID** ppMblk, int offset, int len, void* buf);

#if defined(__cplusplus)
}
#endif

