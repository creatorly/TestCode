
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstUtQueue
{
    EN_UT_QUEUE_MODE enMode;
    _LIST lsEles;
    _LIST lsFreeNodes;
}ST_UT_QUEUE;

typedef struct tagstUtQueueNode
{
    _NODE node;
    _VOID* data;
}ST_UT_QUEUE_NODE;

_UINT32 utQueNodeFree(ST_UT_QUEUE_NODE* pstQueNode);

#if defined(__cplusplus)
}
#endif

