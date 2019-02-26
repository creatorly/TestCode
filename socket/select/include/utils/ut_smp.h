
#ifndef UT_SMP_H
#define UT_SMP_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef _VOID* H_SMP;
typedef _UINT32 (*FP_SMP_CMDPROC)(ST_EOS_CLIVA* va, _VOID* data);

_UINT32 SMP_Construct(H_SMP* phSmp, 
                      const _CHAR8* pcCmdKey,
                      const _CHAR8* pcCmdDlmts, 
                      const _CHAR8* pcParaDlmts);

_UINT32 SMP_Destruct(H_SMP hSmp);

_UINT32 SMP_CmdProcRegister(H_SMP hSmp, 
                            const _CHAR8* pcCmd, 
                            FP_SMP_CMDPROC fpProc,
                            _VOID* data);

_UINT32 SMP_MsgProcTrigger(H_SMP hSmp, _CHAR8* pcMsg);

#if defined(__cplusplus)
}
#endif

#endif

