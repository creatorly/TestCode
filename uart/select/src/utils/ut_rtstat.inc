
#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
    EN_UT_RTS_STATE_IDLE = 0,
    EN_UT_RTS_STATE_WORKING,

    EN_UT_RTS_STATE_BUTT
}EN_UT_RTS_STATE;

typedef struct tagstUtRtsNode* PST_UT_RTS_NODE;

typedef struct tagstUtRtsNode
{
    _NODE node;
    
    _INT32 tid;
    EN_UT_RTS_STATE enState;

    const _CHAR8* pcFile;
    _UINT32 uiBeginLine;
    _UINT32 uiEndLine;
    
    _UINT32 iBeginTicks;
    _INT32 bReBeginPrompt;

    _UINT32 iTotTicks;
    _UINT32 iMaxTicks;
    _UINT32 iLastTicks;
    _UINT32 count;
}ST_UT_RTS_NODE;

typedef struct tagstUtRtsQue
{
    _NODE node;
    
    _INT32 tid;
    H_UT_QUEUE que;
}ST_UT_RTS_QUE;

typedef struct tagstUtRts
{
    FP_UT_RTS_PRINTF fpPrint;    
    _VOID* data;
    
    const _CHAR8* pcFile;
    _UINT32 uiLine;

    SEM_ID mtx;
    
    _LIST lsRtsEnts;
    _LIST lsEndEnts;
    _LIST lsEntQue;
}ST_UT_RTS;

#define UT_RTS_PRINTF(r, fmt, args...)  (r)->fpPrint((r)->data, fmt, ##args)

ST_UT_RTS_NODE* utRtsFindNode(ST_UT_RTS* pstRts, const _CHAR8* file, _INT32 line);
ST_UT_RTS_NODE* utRtsFindEndNode(ST_UT_RTS* pstRts, const _CHAR8* file, _UINT32 line, _UINT32 end_line);
ST_UT_RTS_QUE* utRtsFindQue(ST_UT_RTS* pstRts);

_VOID utRtsAddNode(ST_UT_RTS* pstRts, ST_UT_RTS_NODE* prn);
_VOID utRtsAddEndNode(ST_UT_RTS* pstRts, ST_UT_RTS_NODE* prn);
_VOID utRtsAddQue(ST_UT_RTS* pstRts, ST_UT_RTS_QUE* pQue);

ST_UT_RTS_NODE* utRtsNodeCreate(const _CHAR8* file, _INT32 line);
_VOID utRtsNodeDestroy(ST_UT_RTS_NODE* pRtsNode);

ST_UT_RTS_QUE* utRtsQueCreate(_VOID);
_VOID utRtsQueDestroy(ST_UT_RTS_QUE* pQue);

_BOOL utRtsNodeEqu(ST_UT_RTS_NODE* pNode1, ST_UT_RTS_NODE* pNode2);

#ifdef ANDROID
const _CHAR8* utRtsRemovePath(const _CHAR8* filename);
#endif

#if defined(__cplusplus)
}
#endif

