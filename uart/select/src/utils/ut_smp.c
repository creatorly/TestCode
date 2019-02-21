
/******************************************************************************
 * Description: media stream server
 * Author: pengjian
 * Histroy: 1.00, 2006.5.22 created
 * CopyRight: Ejoin Tech. Ltd. Co, 2006.5~2006
******************************************************************************/

#include "eos.h"
#include "ut_debug.h"
#include "ut_mem.h"

#include "ut_smp.h"
#include "ut_smp.inc"

#if defined(__cplusplus)
extern "C" {
#endif

_UINT32 SMP_Construct(H_SMP* phSmp, 
                      const _CHAR8* pcCmdKey,
                      const _CHAR8* pcCmdDlmts, 
                      const _CHAR8* pcParaDlmts)
{
    ST_SMP* pstSmp = NULL;

    UT_NULLPTR_CHK(phSmp, EOS_ERROR);
    UT_NULLPTR_CHK(pcCmdDlmts, EOS_ERROR);
    UT_NULLPTR_CHK(pcParaDlmts, EOS_ERROR);

    *phSmp = NULL;

    pstSmp = (ST_SMP*)UT_MALLOC(sizeof(ST_SMP));
    UT_MALLOC_CHK(pstSmp, EOS_ERROR);
    memset(pstSmp, 0, sizeof(ST_SMP));

    strncpy(pstSmp->acCmdDlmts, pcCmdDlmts, sizeof(pstSmp->acCmdDlmts)-1);
    strncpy(pstSmp->acParaDlmts, pcParaDlmts, sizeof(pstSmp->acParaDlmts)-1);
    strncpy(pstSmp->acCmdKey, pcCmdKey, sizeof(pstSmp->acCmdKey)-1);
    eos_lst_init(&pstSmp->lsCmds);

    *phSmp = pstSmp;

    return EOS_OK;
}

_UINT32 SMP_Destruct(H_SMP hSmp)
{
    /* add destruct code here later */
    return EOS_OK;
}

_UINT32 SMP_CmdProcRegister(H_SMP hSmp, 
                            const _CHAR8* pcCmd, 
                            FP_SMP_CMDPROC fpProc,
                            _VOID* data)
{
    ST_SMP* pstSmp = (ST_SMP*)hSmp;
    ST_SMP_NODE* pstCmd = NULL;

    UT_NULLPTR_CHK(hSmp, EOS_ERROR);
    UT_NULLPTR_CHK(pcCmd, EOS_ERROR);
    UT_NULLPTR_CHK(fpProc, EOS_ERROR);
    
    UT_CONDCHK((strlen(pcCmd)<sizeof(pstCmd->acCmd)), 
        EOS_ERROR, "invalid command length!\r\n");

    pstCmd = (ST_SMP_NODE*)eos_lst_search(&pstSmp->lsCmds, pcCmd, smpNodeEquByCmd);
    UT_CONDCHK(!pstCmd, EOS_ERROR, "cmd node already exist!\r\n");

    pstCmd = (ST_SMP_NODE*)UT_MALLOC(sizeof(ST_SMP_NODE));
    UT_MALLOC_CHK(pstCmd, EOS_ERROR);
    memset(pstCmd, 0, sizeof(ST_SMP_NODE));

    strcpy(pstCmd->acCmd, pcCmd);
    pstCmd->fpProc = fpProc;
    pstCmd->data = data;

    eos_lst_add(&pstSmp->lsCmds, &pstCmd->node);

    return EOS_OK;
}

_UINT32 SMP_MsgProcTrigger(H_SMP hSmp, _CHAR8* pcMsg)
{
    _CHAR8* pcCmd = NULL;
    ST_SMP* pstSmp = (ST_SMP*)hSmp;
    ST_EOS_STRTOK lsCmds;

    UT_ASSERT(hSmp);
    UT_ASSERT(pcMsg);

    EOS_StrTokConstruct(&lsCmds, pstSmp->acCmdDlmts, pcMsg);

    pcCmd = EOS_StrTokGetToken(&lsCmds);
    while (pcCmd)
    {
        ST_EOS_CLIVA va;
        ST_SMP_NODE* pstCmd = NULL;
        const _CHAR8* pcVal;

        UT_INFO2("%s%s", pcCmd, "\r\n");
        
        EOS_CliVaBegin(&va, pstSmp->acParaDlmts, pcCmd);

        pcVal = EOS_CliVaGetPop(&va, pstSmp->acCmdKey);
        pstCmd = (ST_SMP_NODE*)eos_lst_search(&pstSmp->lsCmds, pcVal, smpNodeEquByCmd);
        if (NULL != pstCmd)
        {
            pstCmd->fpProc(&va, pstCmd->data);
        }
        else
        {
            UT_ERROR(EOS_ERROR, "invalid command!\r\n");
        }
        
        EOS_CliVaEnd(&va);
        
        pcCmd = EOS_StrTokGetToken(&lsCmds);
    }

    EOS_StrTokDestruct(&lsCmds);
    
    return EOS_OK;
}

_BOOL smpNodeEquByCmd(_NODE* node, const _VOID* data)
{
    return !strcmp(((ST_SMP_NODE*)node)->acCmd, (_CHAR8*)data);
}

#if defined(__cplusplus)
}
#endif

