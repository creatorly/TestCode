
#if defined(__cplusplus)
extern "C" {
#endif

#define UtRtssSemP(semid) \
do{ \
    eos_sem_p(semid, EOS_WAIT_FOREVER); \
}while(0)

#define UtRtssSemV(semid) \
do{ \
    eos_sem_v(semid); \
}while(0)

typedef struct tagstUtRtss
{
    EOS_SEM_ID m;
    _LIST lsRtss;
}ST_UT_RTSS;

typedef struct tagstUtRtssNode
{
    _NODE node;
    _VOID* hRts;
}ST_UT_RTSS_NODE;

_UINT32 utRtssNodeFree(ST_UT_RTSS_NODE* pstRtssNode);
_BOOL utRtssNodeEqu(_NODE * pNode, const _VOID* pKey);

#if defined(__cplusplus)
}
#endif

