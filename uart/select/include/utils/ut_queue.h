
#ifndef UT_QUEUE_H
#define UT_QUEUE_H

#if defined(__cplusplus)
extern "C" {
#endif

#undef UT_QUE_DEBUG

typedef enum
{
    EN_UT_QUEUE_MODE_FIFO = 0,
    EN_UT_QUEUE_MODE_LIFO,

    EN_UT_QUEUE_MODE_BUTT
}EN_UT_QUEUE_MODE;

typedef _VOID* H_UT_QUEUE;

_UINT32 UT_QueueConstruct(H_UT_QUEUE* phQue, EN_UT_QUEUE_MODE enMode);
_UINT32 UT_QueueDestruct(H_UT_QUEUE hQue);

#ifdef UT_QUE_DEBUG
#define UT_QueuePush(q, d)  UT_QueuePushEx(q, d, __FILE__, __LINE__)
_UINT32 UT_QueuePushEx(H_UT_QUEUE hQue, _VOID* data, const _CHAR8* file, _INT32 line);
#else
_UINT32 UT_QueuePush(H_UT_QUEUE hQue, _VOID* data);
#endif

_VOID* UT_QueuePop(H_UT_QUEUE hQue);
_INT32 UT_QueueFind(H_UT_QUEUE hQue, _VOID* data);
_VOID* UT_QueueGet(H_UT_QUEUE hQue, _UINT32 uiPos);

_VOID* UT_QueuePopIf(H_UT_QUEUE hQue, _VOID* data, 
    _BOOL (*fpCmp)(_VOID* ele, _VOID* data));

_VOID* UT_QueueFindIf(H_UT_QUEUE hQue, _VOID* data, 
    _BOOL (*fpCmp)(_VOID* ele, _VOID* data));

_UINT32 UT_QueueInsertIf(H_UT_QUEUE hQue, _VOID* data, 
    _BOOL (*fpCmp)(_VOID* ele, _VOID* data));

_UINT32 UT_QueueSize(H_UT_QUEUE hQue);

_VOID UT_QueueShow(H_UT_QUEUE hQue, EOS_INFO_PRINTF fpPrint, _VOID* data);

#if defined(__cplusplus)
}
#endif

#endif

