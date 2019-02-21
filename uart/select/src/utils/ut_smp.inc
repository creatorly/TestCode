
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstSmpNode
{
    _NODE node;

    _CHAR8 acCmd[8];
    FP_SMP_CMDPROC fpProc;
    _VOID* data;
}ST_SMP_NODE;

typedef struct tagstSmp
{
    _CHAR8 acCmdKey[8];
    _CHAR8 acCmdDlmts[8];
    _CHAR8 acParaDlmts[8];
    
    _LIST lsCmds;
}ST_SMP;

_BOOL smpNodeEquByCmd(_NODE* node, const _VOID* data);

#if defined(__cplusplus)
}
#endif

