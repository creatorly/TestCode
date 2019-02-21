
#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_UT_DIALSTR

#include "eos.h"
#include "ut_mem.h"
#include "ut_queue.h"

#include "ut_dialstr.h"
#include "ut_dialstr_.h"
#include "ut_debug.h"

static _UINT32 ccmDigitStrGetPrefixCB(H_CCM_MATCHSTR* phMatchStr, 
                                    H_CCM_DIGITSTR hDgStr, 
                                    _CHAR8* pcDialStr, 
                                    _VOID* data)
{
    ST_CCM_MATCHSTR* pstMatch = (ST_CCM_MATCHSTR*)(*phMatchStr);
    ST_CCM_DIGITSTR_EX* pstDsEx = (ST_CCM_DIGITSTR_EX*)hDgStr;

    if (ccmDigitStrMatchEnded(pstDsEx))
    {
        pstMatch->stPrefix.uiPrefixLen = pstDsEx->pstDigitStr->uiEleNum;
        pstMatch->stPrefix.pcPrefix = NULL;
        pstMatch->stPrefix.pcReplace = (_CHAR8*)data;
    }
    
    return EOS_OK;
}

static _CHAR8* ccmNumAnaNumReplaceCB(ST_CCM_NUMANA* pstNumAna, 
                                    _CHAR8* pcNewNums, 
                                    _VOID* data)
{
    _UINT32 i = 0;
    _UINT32 uiLen = 0;
    ST_CCM_NUMPRE* pstPrefix = &(*(ST_CCM_MATCHSTR**)data)->stPrefix;
    _CHAR8* pcDials = pstNumAna->acCurDialStr;
    _CHAR8* pcReplace = pstPrefix->pcReplace;
    _CHAR8* pcNums = pcNewNums;

    if (NULL == pcReplace)
    {
        return pcDials;
    }

    uiLen = strlen(pcReplace);
    if (uiLen > pstPrefix->uiPrefixLen)
    {
        uiLen -= pstPrefix->uiPrefixLen;
        memcpy(pcNums, pcReplace, uiLen);
        pcNums += uiLen;
        pcReplace += uiLen;
        uiLen = pstPrefix->uiPrefixLen;
    }
    else
    {
        pcDials += (pstPrefix->uiPrefixLen - uiLen);
    }

    for (i=0; i<uiLen; i++)
    {
        pcNums[i] = (pcReplace[i] == 'x') ? pcDials[i] : pcReplace[i];
    }
    pcNums += i;
    pcDials += i;
    
    strcpy(pcNums, pcDials);

    return pcNewNums;
}


_UINT32 CCM_DialStrConstruct(H_CCM_DIALSTR* phDialStr)
{
    ST_CCM_DIALSTR* pstDialStr = NULL;
    
    if (NULL == phDialStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstDialStr = (ST_CCM_DIALSTR*)UT_MALLOC(sizeof(ST_CCM_DIALSTR));
    if (NULL == pstDialStr)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return EOS_ERROR;
    }

    memset(pstDialStr, 0, sizeof(ST_CCM_DIALSTR));
    
    eos_lst_init(&pstDialStr->lsDigitStr);
    eos_lst_init(&pstDialStr->lsPrefix);
    
    *phDialStr = (H_CCM_DIALSTR)pstDialStr;
    return EOS_OK;
}

_UINT32 CCM_DialStrDestruct(H_CCM_DIALSTR hDialStr)
{
    ST_CCM_DIALSTR* pstDialStr = NULL;
    
    if (NULL == hDialStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstDialStr = (ST_CCM_DIALSTR*)hDialStr;
    /*eos_lst_uninit(&pstDialStr->lsDigitStr, (EOS_LSTFREE_FUNC)CCM_DigitStrDestruct);*/
    memset(pstDialStr, 0, sizeof(ST_CCM_DIALSTR));
    
    UT_FREE(hDialStr);
    
    return EOS_OK;
}

_UINT32 CCM_DialStrAddDialPlan(H_CCM_DIALSTR hDialStr, 
                              const _CHAR8* pcDialPlan, 
                               FP_CCM_DIGITSTR_CB fp, 
                              _VOID* data)
{
    _UINT32 uiRet = EOS_ERROR;
    H_CCM_DIGITSTR hDigitStr = NULL;
    _CHAR8 acDefDialPlan[CCM_DIAL_DIGIT_NUM];
    ST_CCM_DIALSTR* pstDialStr = (ST_CCM_DIALSTR*)hDialStr;
        
    if (NULL == hDialStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    if (NULL == pcDialPlan)
    {
        memset(acDefDialPlan, CCM_DIAL_WILDCARD, sizeof(acDefDialPlan));
        acDefDialPlan[sizeof(acDefDialPlan)-1] = '\0';
        pcDialPlan = acDefDialPlan;
    }

    if (eos_lst_search(&pstDialStr->lsDigitStr, pcDialPlan, ccmDigitStrCmpByStr))
    {
        UT_ERROR(EOS_ERROR, "repeated dialplan!\r\n");
        return EOS_ERROR;
    }

    uiRet = CCM_DigitStrConstruct(&hDigitStr, pcDialPlan, fp, data);
    if (EOS_OK != uiRet)
    {
        return uiRet;
    }

    eos_lst_add(&pstDialStr->lsDigitStr, (_NODE*)hDigitStr);

    return EOS_OK;
}

_UINT32 CCM_DialStrDelDialPlan(H_CCM_DIALSTR hDialStr, _CHAR8* pcDialPlan)
{
    return EOS_OK;
}

_UINT32 CCM_DialStrDialPlanSize(H_CCM_DIALSTR hDialStr)
{
    ST_CCM_DIALSTR* pstDialStr = (ST_CCM_DIALSTR*)hDialStr;

    return eos_lst_count(&pstDialStr->lsDigitStr);
}

_UINT32 CCM_DialStrAddPrefix(H_CCM_DIALSTR hDialStr, const _CHAR8* pcPrefix, const _CHAR8* pcReplace)
{
    _UINT32 uiRet = EOS_ERROR;
    _CHAR8* pcRepStr = NULL;
    H_CCM_DIGITSTR hDigitStr = NULL;
    ST_CCM_DIALSTR* pstDialStr = (ST_CCM_DIALSTR*)hDialStr;
    
    if ((NULL == hDialStr) || (NULL == pcPrefix) || (NULL == pcReplace))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    if (eos_lst_search(&pstDialStr->lsPrefix, pcPrefix, ccmDigitStrCmpByStr))
    {
        UT_ERROR(EOS_ERROR, "repeated prefix plan!\r\n");
        return EOS_ERROR;
    }

    /* this memory block will be lost!!! */
    pcRepStr = (_CHAR8*)UT_MALLOC(strlen(pcReplace)+1);
    if (NULL == pcRepStr)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return EOS_ERROR;
    }
    
    strcpy(pcRepStr, pcReplace);
    uiRet = CCM_DigitStrConstruct(&hDigitStr, pcPrefix, 
        ccmDigitStrGetPrefixCB, pcRepStr);
    if (EOS_OK != uiRet)
    {
        UT_FREE(pcRepStr);
        return uiRet;
    }

    eos_lst_add(&pstDialStr->lsPrefix, (_NODE*)hDigitStr);

    return EOS_OK;
}

_UINT32 CCM_DialStrDelPrefix(H_CCM_DIALSTR hDialStr, _CHAR8* pcPrefix)
{
    return EOS_OK;
}

_UINT32 CCM_DialStrPrefixSize(H_CCM_DIALSTR hDialStr)
{
    ST_CCM_DIALSTR* pstDialStr = (ST_CCM_DIALSTR*)hDialStr;

    return eos_lst_count(&pstDialStr->lsPrefix);
}

_UINT32 CCM_DialStrRemoveDigitStr(H_CCM_DIALSTR hDialStr, H_CCM_DIGITSTR hDigitStr)
{
    ST_CCM_DIALSTR* pstDialStr = NULL;

    if ((NULL == hDialStr) || (NULL == hDigitStr))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstDialStr = (ST_CCM_DIALSTR*)hDialStr;
    eos_lst_del(&pstDialStr->lsDigitStr, (_NODE*)hDigitStr);

    return EOS_OK;
}

_UINT32 CCM_DialStrRegisterCB(H_CCM_DIALSTR hDialStr, 
                              _CHAR8* pcPattern, 
                              FP_CCM_DIGITSTR_CB fp, 
                              _VOID* data)
{
    _NODE* pstNode = NULL;
    ST_CCM_DIGITSTR stDgStr;

    if (NULL == hDialStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstNode = eos_lst_first(&((ST_CCM_DIALSTR*)hDialStr)->lsDigitStr);
    if (NULL == pcPattern)
    {
        while (pstNode)
        {
            ST_CCM_DIGITSTR* pstDigitStr = (ST_CCM_DIGITSTR*)pstNode;

            if (NULL == pstDigitStr->fpMatched)
            {
                pstDigitStr->fpMatched = fp;
                pstDigitStr->cbData = data;
            }

            pstNode = eos_lst_next(pstNode);
        }

        return EOS_OK;
    }
    
    if (!ccmDigitStringParser(&stDgStr, pcPattern))
    {
        UT_ERROR(EOS_ERROR, "parser num prefix failed!\r\n");
        return EOS_ERROR;
    }

    pstNode = eos_lst_first(&((ST_CCM_DIALSTR*)hDialStr)->lsDigitStr);
    while (pstNode)
    {
        _INT32 i = 0;
        ST_CCM_DIGITSTR* pstDigitStr = (ST_CCM_DIGITSTR*)pstNode;

        if ( (NULL != pstDigitStr->fpMatched)
            || (pstDigitStr->uiEleNum < stDgStr.uiEleNum))
        {
            pstNode = eos_lst_next(pstNode);
            continue;
        }

        for (i=0; i<stDgStr.uiEleNum; i++)
        {
            if (!strstr(stDgStr.astDigitStrEles[i].acRange, pstDigitStr->astDigitStrEles[i].acRange))
            {
                break;
            }
        }

        if (i == stDgStr.uiEleNum)
        {
            pstDigitStr->fpMatched = fp;
            pstDigitStr->cbData = data;
        }

        pstNode = eos_lst_next(pstNode);
    }

    return EOS_OK;
}

_UINT32 CCM_DigitStrConstruct(H_CCM_DIGITSTR* phDigitStr, 
                              const _CHAR8* pcDialDigit, 
                              FP_CCM_DIGITSTR_CB fpMatch, 
                              _VOID* data)
{
    ST_CCM_DIGITSTR* pstDigitStr = NULL;
    _UINT32 uiDigitStrEntLen = sizeof(ST_CCM_DIGITSTR);

    if (NULL == phDigitStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    uiDigitStrEntLen += (strlen(pcDialDigit) + 1);
    pstDigitStr = (ST_CCM_DIGITSTR*)UT_MALLOC(uiDigitStrEntLen);
    if (NULL == pstDigitStr)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return EOS_ERROR;
    }
    memset(pstDigitStr, 0, uiDigitStrEntLen);

    if(!ccmDigitStringParser(pstDigitStr, pcDialDigit))
    {
        UT_FREE(pstDigitStr);
        UT_ERROR(EOS_ERROR, "paser dialstring failed!\r\n");
        return EOS_ERROR;
    }

    pstDigitStr->pcDigitStr = (_CHAR8*)(pstDigitStr + 1);
    strcpy(pstDigitStr->pcDigitStr, pcDialDigit);
    
    pstDigitStr->fpMatched = fpMatch;
    pstDigitStr->cbData = data;

    *phDigitStr = (H_CCM_DIGITSTR)pstDigitStr;
    return EOS_OK;
}

_UINT32 CCM_DigitStrDestruct(H_CCM_DIGITSTR hDigitStr)
{
    return EOS_OK;
}

_CHAR8* CCM_DigitStrGetDialPlan(H_CCM_DIGITSTR hDigitStr)
{
    return ((ST_CCM_DIGITSTR*)hDigitStr)->pcDigitStr;
}

FP_CCM_DIGITSTR_CB CCM_DigitStrGetFunc(H_CCM_DIGITSTR hDigitStr)
{
    return ((ST_CCM_DIGITSTR*)hDigitStr)->fpMatched;
}

_VOID*  CCM_DigitStrGetData(H_CCM_DIGITSTR hDigitStr)
{
    return ((ST_CCM_DIGITSTR*)hDigitStr)->cbData;
}

_UINT32 CCM_MatchStrConstruct(H_CCM_MATCHSTR* phMatchStr, H_CCM_DIALSTR hDialStr)
{
    _UINT32 uiRet = EOS_ERROR;
    ST_CCM_MATCHSTR* pstMatchStr = NULL;
    ST_CCM_DIALSTR* pstDialStr = (ST_CCM_DIALSTR*)hDialStr;
    
    if ((NULL == phMatchStr) || (NULL == hDialStr))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    pstMatchStr = (ST_CCM_MATCHSTR*)CcmDsMalloc(sizeof(ST_CCM_MATCHSTR));
    if (NULL == pstMatchStr)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return EOS_ERROR;
    }
    memset(pstMatchStr, 0, sizeof(ST_CCM_MATCHSTR));

    /* initialize number analysys by the digit string list */
    uiRet = ccmNumAnaConstruct(&pstMatchStr->stCcmNaNum, &pstDialStr->lsDigitStr, 
                                ccmNumAnaNumReplaceCB, phMatchStr);
    if (EOS_OK != uiRet)
    {
        CcmDsFree(pstMatchStr);
        return uiRet;
    }
    
    /* initialize prefix analysys by the prefix list */
    uiRet = ccmNumAnaConstruct(&pstMatchStr->stCcmNaPrefix, 
                                &pstDialStr->lsPrefix, NULL, phMatchStr);
    if (EOS_OK != uiRet)
    {
        ccmNumAnaDestruct(&pstMatchStr->stCcmNaNum);
        CcmDsFree(pstMatchStr);
        return uiRet;
    }

    *phMatchStr = (H_CCM_MATCHSTR)pstMatchStr;
    return EOS_OK;
}

_UINT32 CCM_MatchStrDestruct(H_CCM_MATCHSTR hMatchStr)
{
    ST_CCM_MATCHSTR* pstMatchStr = (ST_CCM_MATCHSTR*)hMatchStr;

    if (NULL == hMatchStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    ccmNumAnaDestruct(&pstMatchStr->stCcmNaPrefix);
    ccmNumAnaDestruct(&pstMatchStr->stCcmNaNum);

    memset(pstMatchStr, 0, sizeof(ST_CCM_MATCHSTR));
    CcmDsFree(pstMatchStr);

    return EOS_OK;
}

_UINT32 CCM_MatchStrReset(H_CCM_MATCHSTR hMatchStr)
{
    ST_CCM_MATCHSTR* pstMatchStr = (ST_CCM_MATCHSTR*)hMatchStr;

    if (NULL == hMatchStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    ccmNumAnaReset(&pstMatchStr->stCcmNaPrefix);
    ccmNumAnaReset(&pstMatchStr->stCcmNaNum);

    return EOS_OK;
}

EN_CCM_NASTATUS CCM_MatchStrAddDigit(H_CCM_MATCHSTR hMatchStr, 
                                     _CHAR8 cDigit,
                                     H_CCM_DIGITSTR* phDigitStr)
{
    EN_CCM_NASTATUS enNaStatus = EN_CCM_NASTATUS_FAIL;
    ST_CCM_MATCHSTR* pstMatchStr = (ST_CCM_MATCHSTR*)hMatchStr;

    if (NULL == hMatchStr)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EN_CCM_NASTATUS_FAIL;
    }

    /* match number prefix */
    UT_INFO("Match prefix with digit %c%s", cDigit, "\r\n");
    ccmNumAnaAddDigit(&pstMatchStr->stCcmNaPrefix, cDigit, NULL);
    
    /* match number */
    UT_INFO("Match dialplan with digit %c%s", cDigit, "\r\n");
    enNaStatus = ccmNumAnaAddDigit(&pstMatchStr->stCcmNaNum, cDigit, phDigitStr);

    return enNaStatus;
}

EN_CCM_NASTATUS CCM_MatchStrAddDigits(H_CCM_MATCHSTR hMatchStr, 
                                      const _CHAR8* pcDigits,
                                      H_CCM_DIGITSTR* phDigitStr)
{
    EN_CCM_NASTATUS enNaStatus = EN_CCM_NASTATUS_CONTINUE;
    ST_CCM_MATCHSTR* pstMatchStr = (ST_CCM_MATCHSTR*)hMatchStr;

    if ((NULL == hMatchStr) || (NULL == pcDigits))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EN_CCM_NASTATUS_FAIL;
    }

    while ((*pcDigits) && (EN_CCM_NASTATUS_CONTINUE == enNaStatus))
    {
        /* match number prefix */
        UT_INFO("Match prefix with digit %c%s", *pcDigits, "\r\n");
        ccmNumAnaAddDigit(&pstMatchStr->stCcmNaPrefix, *pcDigits, NULL);
        
        /* match number */
        UT_INFO("Match dialplan with digit %c%s", *pcDigits, "\r\n");
        enNaStatus = ccmNumAnaAddDigit(&pstMatchStr->stCcmNaNum, *pcDigits, phDigitStr);

        pcDigits++;
    }

    return enNaStatus;
}

EN_CCM_NASTATUS CCM_MatchStrGetStatus(H_CCM_MATCHSTR hMatchStr)
{
    return ((ST_CCM_MATCHSTR*)hMatchStr)->stCcmNaNum.enStatus;
}

EN_CCM_NASTATUS CCM_MatchStrUpdate(H_CCM_MATCHSTR hMatchStr, H_CCM_DIGITSTR* phDigitStr)
{
    ((ST_CCM_MATCHSTR*)hMatchStr)->stCcmNaNum.enStatus = EN_CCM_NASTATUS_CONTINUE;

    return CCM_MatchStrAddDigit(hMatchStr, '\0', phDigitStr);
}

_UINT32 ccmNumAnaConstruct(ST_CCM_NUMANA* pstNumAna, 
                          _LIST* plsDigitStr, 
                          FP_CCM_NUMANA_CB fpMatched, 
                          _VOID* data)
{
    _INT32 i = 0;
    _NODE* pstNode = NULL;
    ST_CCM_DIGITSTR_EX* pstDsEx = NULL;
    
    if ((NULL == pstNumAna) && (NULL == plsDigitStr))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    memset(pstNumAna, 0, sizeof(ST_CCM_NUMANA));
    pstNumAna->uiNodeNum = eos_lst_count(plsDigitStr);
    if (0 == pstNumAna->uiNodeNum)
    {
        pstNumAna->enStatus = EN_CCM_NASTATUS_FAIL;
        pstNumAna->pstNodeMem = NULL;
        return EOS_OK;
    }

    i = sizeof(ST_CCM_DIGITSTR_EX) * pstNumAna->uiNodeNum;
    pstDsEx = (ST_CCM_DIGITSTR_EX*)CcmDsMalloc(i);
    if (NULL == pstDsEx)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return EOS_ERROR;
    }
    memset(pstDsEx, 0, i);

    pstNumAna->enStatus = EN_CCM_NASTATUS_CONTINUE;

    eos_lst_init(&pstNumAna->lsCurrent);
    eos_lst_init(&pstNumAna->lsMatched);
    eos_lst_init(&pstNumAna->lsBackup);

    /* initialize the exatct match list by the dialstring list */
    pstNode = eos_lst_first(plsDigitStr);
    for (i=0; i<pstNumAna->uiNodeNum; i++)
    {
        pstDsEx[i].pstDigitStr = (ST_CCM_DIGITSTR*)pstNode;
        pstDsEx[i].uiMatchPrior = 0;
        memset(&pstDsEx[i].stStatus, 0, sizeof(ST_CCM_DSE_STATUS));
        pstDsEx[i].stStatus.ucDseNo = 0;
        pstDsEx[i].stStatus.ucMinRepCnt 
            = pstDsEx[i].pstDigitStr->astDigitStrEles[0].ucMinRepCnt;
        pstDsEx[i].stStatus.ucStart = 0;
        pstDsEx[i].stStatus.ucRepCnt = 0;
        UT_QueueConstruct(&pstDsEx[i].hPartMatched, EN_UT_QUEUE_MODE_LIFO);
        eos_lst_add(&pstNumAna->lsCurrent, &pstDsEx[i].node);
        pstNode = eos_lst_next(pstNode);
    }

    /* clear the current dial string */
    pstNumAna->pstNodeMem = pstDsEx;    /* for free */
    pstNumAna->uiCurDialStrLen = 0;
    memset(pstNumAna->acCurDialStr, 0, sizeof(pstNumAna->acCurDialStr));
    pstNumAna->fpMatched = fpMatched;
    pstNumAna->data = data;

    return EOS_OK;
}

_UINT32 ccmNumAnaDestruct(ST_CCM_NUMANA* pstNumAna)
{
    if (NULL == pstNumAna)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EOS_ERROR;
    }

    if (NULL != pstNumAna->pstNodeMem)
    {
        _UINT32 i = 0;
        
        for (i=0; i<pstNumAna->uiNodeNum; i++)
        {
            UT_QueueDestruct(pstNumAna->pstNodeMem[i].hPartMatched);
        }

        CcmDsFree(pstNumAna->pstNodeMem);
    }
    
    memset(pstNumAna, 0, sizeof(ST_CCM_NUMANA));

    return EOS_OK;
}

_VOID ccmNumAnaReset(ST_CCM_NUMANA* pstNumAna)
{
    _INT32 i = 0;
    ST_CCM_DIGITSTR_EX* pstDsEx;

    pstNumAna->enStatus = EN_CCM_NASTATUS_CONTINUE;

    eos_lst_init(&pstNumAna->lsCurrent);
    eos_lst_init(&pstNumAna->lsMatched);
    eos_lst_init(&pstNumAna->lsBackup);

    pstDsEx = (ST_CCM_DIGITSTR_EX*)pstNumAna->pstNodeMem;

    /* initialize the exatct match list by the dialstring list */
    for (i=0; i<pstNumAna->uiNodeNum; i++)
    {
        pstDsEx[i].uiMatchPrior = 0;

        pstDsEx[i].stStatus.ucDseNo = 0;
        pstDsEx[i].stStatus.ucMinRepCnt 
            = pstDsEx[i].pstDigitStr->astDigitStrEles[0].ucMinRepCnt;
        pstDsEx[i].stStatus.ucStart = 0;
        pstDsEx[i].stStatus.ucRepCnt = 0;

        UT_QueueDestruct(pstDsEx[i].hPartMatched);
        UT_QueueConstruct(&pstDsEx[i].hPartMatched, EN_UT_QUEUE_MODE_LIFO);
        
        eos_lst_add(&pstNumAna->lsCurrent, &pstDsEx[i].node);
    }

    /* clear the current dial string */
    pstNumAna->uiCurDialStrLen = 0;
    memset(pstNumAna->acCurDialStr, 0, sizeof(pstNumAna->acCurDialStr));
}

EN_CCM_NASTATUS ccmDigitStrOnMatchEnd(ST_CCM_NUMANA* pstNumAna,
                                      ST_CCM_DIGITSTR_EX* pstDgsEx,
                                      H_CCM_DIGITSTR* phDigitStr)
{                 
    _UINT32 uiRet = EOS_OK;
    _CHAR8 acDials[CCM_DIAL_DIGIT_NUM];
    _CHAR8* pcDials = pstNumAna->acCurDialStr;
    
    pstNumAna->acCurDialStr[pstNumAna->uiCurDialStrLen] = '\0';

    if (NULL != pstNumAna->fpMatched)
    {
        pcDials = pstNumAna->fpMatched(pstNumAna, acDials, pstNumAna->data);
        UT_INFO(" send dial number %s to 0x%08x\r\n", 
            pcDials, (_UINT32)pstDgsEx->pstDigitStr->cbData);
    }

    if (NULL != pstDgsEx->pstDigitStr->fpMatched)
    {
        /* v.207: 2006.08.05 pengjian modified */
        uiRet = pstDgsEx->pstDigitStr->fpMatched(pstNumAna->data, pstDgsEx, 
            pcDials, pstDgsEx->pstDigitStr->cbData);
    }

    if (phDigitStr)
    {
        *phDigitStr = (H_CCM_DIGITSTR)pstDgsEx->pstDigitStr;
    }

    pstNumAna->enStatus = uiRet ? EN_CCM_NASTATUS_CONTINUE : EN_CCM_NASTATUS_DONE;

    return pstNumAna->enStatus;
}

EN_CCM_NASTATUS ccmNumAnaAddDigit(ST_CCM_NUMANA* pstNumAna, 
                                  _CHAR8 cDigit,
                                  H_CCM_DIGITSTR* phDigitStr)
{
    EN_CCM_MATCHTYPE enCurMatchType = 0;
    ST_CCM_DIGITSTR* pstDigitStr = NULL;
    _NODE* node = NULL;
    _NODE* nextNode = NULL;
    _LIST lsCurDump;
    _BOOL bMatchEnded = EOS_FALSE;

    if (NULL == pstNumAna)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!\r\n");
        return EN_CCM_NASTATUS_FAIL;
    }
    
    if (EN_CCM_NASTATUS_CONTINUE != pstNumAna->enStatus)
    {
        return pstNumAna->enStatus;
    }

    if (cDigit && (pstNumAna->uiCurDialStrLen < sizeof(pstNumAna->acCurDialStr)-1))
    {
        pstNumAna->acCurDialStr[pstNumAna->uiCurDialStrLen++] = cDigit;
    }

    /* match in current digit string list */    

LINE_DSMATICH_IN_CUR:
    eos_lst_init(&lsCurDump);
    node = eos_lst_first(&pstNumAna->lsCurrent);
    while (node)
    {
        ST_CCM_DIGITSTR_ELE* pstEle = NULL;
        ST_CCM_DIGITSTR_EX* pstDgsEx = (ST_CCM_DIGITSTR_EX*)node;

        nextNode = eos_lst_next(node);
        
        pstDigitStr = pstDgsEx->pstDigitStr;   
        pstEle = &pstDigitStr->astDigitStrEles[pstDgsEx->stStatus.ucDseNo];
        if (ccmDigitStrExMatching(pstDgsEx, 
            pstNumAna->acCurDialStr, pstNumAna->uiCurDialStrLen))
        {
            if (pstEle->ucMatchType > enCurMatchType)
            {
                /* priorer to the current match priority, remove previous to backup */
                enCurMatchType = pstEle->ucMatchType;

                if (eos_lst_prev(node))
                {
                    CcmDsLstExtractToHead(&pstNumAna->lsCurrent, 
                        eos_lst_first(&pstNumAna->lsCurrent), 
                        eos_lst_prev(node), &pstNumAna->lsBackup);
                }

                if ((bMatchEnded = ccmDigitStrMatchEnded(pstDgsEx)))
                {           
                    eos_lst_del(&pstNumAna->lsCurrent, node);
                    ccmDsListInsertOrderly(&pstNumAna->lsMatched, node, ccmNodeMatchPriorMore);
                }
            }
            else if (pstEle->ucMatchType < enCurMatchType)
            {
                /* the current match priority is prirorer, remove it to backup */
                eos_lst_del(&pstNumAna->lsCurrent, node);
                if (ccmDigitStrMatchEnded(pstDgsEx))
                {           
                    ccmDsListInsertOrderly(&pstNumAna->lsMatched, node, ccmNodeMatchPriorMore);
                }
                else
                {
                    ccmDsListInsertOrderly(&pstNumAna->lsBackup, node, ccmNodeMatchPriorMore);
                }
            }
            /* equal to the current match priority, reserve it */
            else 
            {
                if (ccmDigitStrMatchEnded(pstDgsEx))
                {
                    if (eos_lst_prev(node))
                    {
                        CcmDsLstExtractToHead(&pstNumAna->lsCurrent, 
                            eos_lst_first(&pstNumAna->lsCurrent), 
                            eos_lst_prev(node), &pstNumAna->lsBackup);
                    }

                    eos_lst_del(&pstNumAna->lsCurrent, node);
                    ccmDsListInsertOrderly(&pstNumAna->lsMatched, node, ccmNodeMatchPriorMore);
                }
                else if (bMatchEnded)
                {
                    eos_lst_del(&pstNumAna->lsCurrent, node);
                    ccmDsListInsertOrderly(&pstNumAna->lsBackup, node, ccmNodeMatchPriorMore);
                }
            }
        }
        else
        {
            eos_lst_del(&pstNumAna->lsCurrent, node);
            eos_lst_add(&lsCurDump, node);
        }

        node = nextNode;
    }

    ccmNumAnaShow(pstNumAna);

    if (0 == eos_lst_count(&pstNumAna->lsCurrent))
    {
        node = eos_lst_first(&pstNumAna->lsMatched);
        while (node)    /* match successfully */
        {
            nextNode = eos_lst_next(node);
            eos_lst_del(&pstNumAna->lsMatched, node);

            if (EN_CCM_NASTATUS_DONE == ccmDigitStrOnMatchEnd(pstNumAna, 
                (ST_CCM_DIGITSTR_EX*)node, phDigitStr))
            {
                return EN_CCM_NASTATUS_DONE;
            }

            node = nextNode;
        }

        if (CCM_DIAL_END == cDigit)
        {
            /* 081: pengjian added on 2009.02.17, delete the last '#' */
            pstNumAna->uiCurDialStrLen--;

            node = eos_lst_first(&lsCurDump);
            while (node)    /* match successfully */
            {
                if (EN_CCM_NASTATUS_DONE == ccmDigitStrOnMatchEnd(pstNumAna, 
                    (ST_CCM_DIGITSTR_EX*)node, phDigitStr))
                {
                    return EN_CCM_NASTATUS_DONE;
                }

                node = eos_lst_next(node);
            }

            /* 081: restore the last '#' */
            pstNumAna->acCurDialStr[pstNumAna->uiCurDialStrLen++] = cDigit;
        }
        
        /* match fail, update the current match digit string */
        if(EOS_TRUE != ccmMatchStrUpdate(pstNumAna))
        {
            return (pstNumAna->enStatus = EN_CCM_NASTATUS_FAIL);
        }

        goto LINE_DSMATICH_IN_CUR;
    }

    return EN_CCM_NASTATUS_CONTINUE;
}

_BOOL ccmDigitStrEleIsMatch(ST_CCM_DIGITSTR_ELE* pstEle, _CHAR8 cDigit)
{
    return ((CcmDsStrInRange(pstEle->acRange, cDigit)) 
          ^ (EOS_TRUE == pstEle->ucNotInRange));
}

_BOOL ccmDigitStrExMatching(ST_CCM_DIGITSTR_EX* pstDsEx, 
                           _CHAR8* pcDialStr, 
                           _UINT32 uiLen)
{
    ST_CCM_DSE_STATUS* pstStatus = &pstDsEx->stStatus;
    ST_CCM_DIGITSTR* pstDigitStr = pstDsEx->pstDigitStr;

    while ((uiLen > (pstStatus->ucStart + pstStatus->ucRepCnt)) 
        && (pstStatus->ucDseNo < pstDigitStr->uiEleNum))
    {
        if (pstStatus->ucRepCnt < pstStatus->ucMinRepCnt)
        {
            if (ccmDigitStrEleIsMatch(&pstDigitStr->astDigitStrEles[pstStatus->ucDseNo], 
                pcDialStr[pstStatus->ucStart + pstStatus->ucRepCnt]))
            {
                pstDsEx->uiMatchPrior *= EN_CCM_MATCHTYPE_BUTT;
                pstDsEx->uiMatchPrior += 
                    pstDigitStr->astDigitStrEles[pstStatus->ucDseNo].ucMatchType;
                pstDsEx->stStatus.ucRepCnt++;

                ccmDigitStrNodeUpdate(pstDsEx);
            }
            else if (UT_QueueSize(pstDsEx->hPartMatched) > 0)
            {
                *(_VOID**)pstStatus = UT_QueuePop(pstDsEx->hPartMatched);
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            ccmDigitStrNodeUpdate(pstDsEx);
        }
    }

    return (uiLen <= (pstStatus->ucStart + pstStatus->ucRepCnt));
}

_BOOL ccmMatchStrUpdate(ST_CCM_NUMANA* pstNumAna)
{
    _UINT32 uiCurPrior = 0;
    _BOOL bIsMatched = EOS_FALSE;
    _NODE* node = NULL;
    _NODE* nextNode = NULL;
    _LIST lsMatched;

    eos_lst_init(&lsMatched);

    /* select priorer node matched to current dial number */
    node = eos_lst_first(&pstNumAna->lsBackup);
    while (node)
    {
        ST_CCM_DIGITSTR_EX* pstDsNode = (ST_CCM_DIGITSTR_EX*)node;

        if ((pstDsNode->uiMatchPrior < uiCurPrior) && bIsMatched)
        {
            break;  /* the priority of rest node lower than uiCurPrior */
        }        

        uiCurPrior = pstDsNode->uiMatchPrior;        

        nextNode = eos_lst_next(node);
        eos_lst_del(&pstNumAna->lsBackup, node);

        /* to find a useful digit string, if found, add to matched list */
        /* the last digit will match in current list */
        bIsMatched = ccmDigitStrExMatching(pstDsNode, 
            pstNumAna->acCurDialStr, pstNumAna->uiCurDialStrLen - 1);
        if (EOS_TRUE == bIsMatched)
        {
            ccmDsListInsertOrderly(&lsMatched, node, ccmNodeMatchPriorMore);
        }
        
        node = nextNode;
    }

    /* distribute the priorer nodes to current list and else to backup list */
    uiCurPrior = 0;
    node = eos_lst_first(&lsMatched);
    while (node)
    {
        /* push front of the backup list */
        if (((ST_CCM_DIGITSTR_EX*)node)->uiMatchPrior < uiCurPrior)
        {
            eos_lst_concat(&lsMatched, &pstNumAna->lsBackup);
            memcpy(&pstNumAna->lsBackup, &lsMatched, sizeof(_LIST));
            break;
        }

        uiCurPrior = ((ST_CCM_DIGITSTR_EX*)node)->uiMatchPrior;

        /* add to the current list */
        nextNode = eos_lst_next(node);
        eos_lst_del(&lsMatched, node);
        eos_lst_add(&pstNumAna->lsCurrent, node);
        node = nextNode;
    }

    return bIsMatched;
}

_BOOL ccmDigitStrMatchEnded(ST_CCM_DIGITSTR_EX* pstDsEx)
{
    return (pstDsEx->stStatus.ucDseNo >= pstDsEx->pstDigitStr->uiEleNum);
}

_BOOL ccmDspIsValidDigit(_CHAR8 cDigit)
{
    return CcmDsStrInRange(CCM_DSP_CHARSET, cDigit);
}

_BOOL ccmDigitStrEleAddDigit(ST_CCM_DIGITSTR_ELE* pstEls, _CHAR8 cDigit)
{
    _UINT32 uiLen = 0;

    if (CcmDsStrInRange(pstEls->acRange, cDigit)
        || (EN_CCM_MATCHTYPE_WILD == pstEls->ucMatchType))
    {
        return EOS_TRUE;
    }

    if (CCM_DIAL_WILDCARD == cDigit)
    {
        strcpy(pstEls->acRange, CCM_DIAL_WILDSET);
        pstEls->ucMatchType = EN_CCM_MATCHTYPE_WILD;
        return EOS_TRUE;
    }
    else if (CCM_DIAL_DIGIT == cDigit)
    {
        strcpy(pstEls->acRange, CCM_DIAL_DIGITSET);
        pstEls->ucMatchType = EN_CCM_MATCHTYPE_FUZZY;
        return EOS_TRUE;
    }
    
    uiLen = strlen(pstEls->acRange);
    if (uiLen > (sizeof(pstEls->acRange)-2))
    {
        return EOS_FALSE;
    }

    pstEls->acRange[uiLen++] = cDigit;
    pstEls->acRange[uiLen] = '\0';
    pstEls->ucMatchType = (1 == uiLen) ? 
        EN_CCM_MATCHTYPE_EXACT : EN_CCM_MATCHTYPE_FUZZY;

    return EOS_TRUE;
}

_BOOL ccmDigitStrEleAddDigits(ST_CCM_DIGITSTR_ELE* pstEls, _CHAR8 cMin, _CHAR8 cMax)
{
    _BOOL bRet = EOS_FALSE;
    _CHAR8 c;

    for (c=cMin; c<=cMax; c++)
    {
        bRet = ccmDigitStrEleAddDigit(pstEls, c);
        if (EOS_TRUE != bRet)
        {
            return bRet;
        }
    }

    return EOS_TRUE;
}

_VOID* ccmDspNormal(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data)
{
    ST_CCM_DIGITSTR_ELE* pstEle;
    
    while (**ppDialStr)
    {
        if (ccmDspIsValidDigit(**ppDialStr))
        {
            pstEle = &pstDigitStr->astDigitStrEles[pstDigitStr->uiEleNum];
            ccmDseConstruct(pstEle);
            if (EOS_TRUE != ccmDigitStrEleAddDigit(pstEle, **ppDialStr))
            {
                return NULL;
            }

            pstDigitStr->uiEleNum++;
        }
        else if (CCM_DSP_RANGE_BEGIN == (**ppDialStr))
        {
            (*ppDialStr)++;
            return ccmDspRange;
        }
        else if (CCM_DSP_REPEAT_BEGIN == (**ppDialStr))
        {
            (*ppDialStr)++;
            return ccmDspRepeat;
        }
        else if (CCM_DSP_REPEAT_NORO == (**ppDialStr))
        {
            /* repeat previous digit string element none or once */
            if (pstDigitStr->uiEleNum > 0)
            {
                pstEle = &pstDigitStr->astDigitStrEles[pstDigitStr->uiEleNum-1];
                pstEle->ucMinRepCnt = 0;
                pstEle->ucMaxRepCnt = 1;
            }
        }
        else if (CCM_DSP_REPEAT_OORM == (**ppDialStr))
        {
            /* repeat previous digit string element once or more */
            if (pstDigitStr->uiEleNum > 0)
            {
                pstEle = &pstDigitStr->astDigitStrEles[pstDigitStr->uiEleNum-1];
                pstEle->ucMinRepCnt = 1;
                pstEle->ucMaxRepCnt = CCM_DSP_REPEAT_MAX;
            }
        }
        else
        {
            break;
        }

        (*ppDialStr)++;
    }

    return NULL;
}

_VOID* ccmDspRange(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data)
{
    enum
    {
        EnParseNormal = 0,
        EnParseMinFound,
        EnParseMaxFound,
        EnParseFlgFound
    }enState;
    
    ST_CCM_DIGITSTR_ELE* pstEle;
    _UINT32 uiEleCnt = 0;
    _CHAR8 cMin = '\0';
    _CHAR8 cMax = '\0';

    pstEle = &pstDigitStr->astDigitStrEles[pstDigitStr->uiEleNum];
    ccmDseConstruct(pstEle);

    pstEle->ucNotInRange = EOS_FALSE;
    if (CCM_DSP_RANGE_NOT == **ppDialStr)
    {
        pstEle->ucNotInRange = EOS_TRUE;
        (*ppDialStr)++;
    }
    
    enState = EnParseNormal;
    while (**ppDialStr)
    {
        if (ccmDspIsValidDigit(**ppDialStr))
        {
            switch (enState)
            {
                case EnParseMinFound:
                    if (!ccmDigitStrEleAddDigit(pstEle, cMin))
                    {
                        return NULL;
                    }
                    uiEleCnt = 1;
                case EnParseNormal:
                    cMax = cMin = **ppDialStr;
                    enState = EnParseMinFound;
                    break;
                    
                case EnParseFlgFound:
                    if ((**ppDialStr) < cMin)
                        return NULL;
                    cMax = **ppDialStr;
                    if (!ccmDigitStrEleAddDigits(pstEle, cMin, cMax))
                    {
                        return NULL;
                    }
                    uiEleCnt = 1;
                    enState = EnParseNormal;
                    break;
                    
                default:
                   return NULL; 
            }
        }
        else if (CCM_DSP_AREA_FLAG == (**ppDialStr))
        {
            if (EnParseMinFound != enState)
            {
                break;
            }
            enState = EnParseFlgFound;
        }
        else if (CCM_DSP_RANGE_END == (**ppDialStr))
        {
            if (EnParseFlgFound == enState)
            {
                break;
            }
            else if (EnParseMinFound == enState)
            {
                if (!ccmDigitStrEleAddDigit(pstEle, cMin))
                {
                    return NULL;
                }
                uiEleCnt = 1;
            }
            
            pstDigitStr->uiEleNum += uiEleCnt;
            (*ppDialStr)++;
            return ccmDspNormal;
        }
        else if (CCM_DSP_AREA_SEP == (**ppDialStr))
        {
        }
        else
        {
            break;
        }

        (*ppDialStr)++;
        CcmDsSkipSpace(*ppDialStr);
    }

    return NULL;
}

_VOID* ccmDspRepeat(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data)
{
    enum
    {
        EnParseFindMin = 0,
        EnParseFindMax,
        EnParseFindOff
    }enState;
    
    ST_CCM_DIGITSTR_ELE* pstEle;
    _UCHAR8 ucMin = 0;
    _UCHAR8 ucMax = 0;
    _CHAR8  cAdd  = 1;

    if (pstDigitStr->uiEleNum < 1)
    {
        /* ignore the repeat digit string */
        *ppDialStr = strchr(*ppDialStr, CCM_DSP_REPEAT_END);
        if (*ppDialStr && **ppDialStr)
        {
            /* find the repeat ended char */
            (*ppDialStr)++;
        }
        
        return ccmDspNormal;
    }
    
    pstEle = &pstDigitStr->astDigitStrEles[pstDigitStr->uiEleNum-1];

    enState = EnParseFindMin;
    while (**ppDialStr)
    {
        CcmDsSkipSpace(*ppDialStr);
        switch (enState)
        {
            case EnParseFindMin:
                if (CcmDsIsDigit(**ppDialStr))
                {
                    ucMin = (ucMin << 3) + (ucMin << 1);
                    ucMin += (**ppDialStr - '0');
                }
                else if (CCM_DSP_ADDFLG == (**ppDialStr))
                {
                    cAdd = 1;
                    enState = EnParseFindOff;
                }
                else if (CCM_DSP_DECFLG == (**ppDialStr))
                {
                    cAdd = -1;
                    enState = EnParseFindOff;
                }
                else
                {
                    CcmDsSkipSpace(*ppDialStr);
                    if (CCM_DSP_REPEAT_END == (**ppDialStr))
                    {
                        pstEle->ucMinRepCnt = EOS_MAX(ucMin, 1);
                        pstEle->ucMaxRepCnt = EOS_MAX(ucMin, 1);
                        (*ppDialStr)++;
                        return ccmDspNormal;
                    }
                    else if (CCM_DSP_AREA_SEP == (**ppDialStr))
                    {
                        cAdd = 1;
                        enState = EnParseFindMax;
                    }
                    else
                    {
                        return NULL;
                    }
                }
                break;

            case EnParseFindMax:
                if (CcmDsIsDigit(**ppDialStr))
                {
                    ucMax = (ucMax << 3) + (ucMax << 1);
                    ucMax += (**ppDialStr - '0');
                }
                else 
                {
                    CcmDsSkipSpace(*ppDialStr);
                    if ((CCM_DSP_REPEAT_END != (**ppDialStr)) || (ucMax < ucMin))
                    {
                        return NULL;
                    }
                        
                    pstEle->ucMinRepCnt = ucMin;
                    pstEle->ucMaxRepCnt = ucMax;
                    (*ppDialStr)++;
                    return ccmDspNormal;
                }
                break;
                
            case EnParseFindOff:
                if (CcmDsIsDigit(**ppDialStr))
                {
                    ucMax = (ucMax << 3) + (ucMax << 1);
                    ucMax += (**ppDialStr - '0');
                }
                else 
                {
                    CcmDsSkipSpace(*ppDialStr);
                    if (CCM_DSP_REPEAT_END != (**ppDialStr))
                    {
                        return NULL;
                    }

                    ucMax = (0 != ucMax) ? (ucMin + cAdd * ucMax) 
                        : ((1 == cAdd) ? CCM_DSP_REPEAT_MAX : 0);
                    
                    pstEle->ucMinRepCnt = EOS_MIN(ucMin, ucMax);
                    pstEle->ucMaxRepCnt = EOS_MAX(ucMin, ucMax);
                    (*ppDialStr)++;    
                    return ccmDspNormal;
                }
                break;
                
            default:
                break;
        }

        (*ppDialStr)++;
    }

    return NULL;
}

_BOOL ccmDigitStringParser(ST_CCM_DIGITSTR* pstDgStr, const _CHAR8* pcDgStr)
{
    _CHAR8* pcStr = (_CHAR8*)pcDgStr;
    FP_CCM_DSPARSER fpParser = ccmDspNormal;

    pstDgStr->uiEleNum = 0;
    while (fpParser)
    {
        fpParser = (FP_CCM_DSPARSER)fpParser(pstDgStr, &pcStr, NULL);
    }

    return ('\0' == *pcStr);
}

_VOID ccmDseConstruct(ST_CCM_DIGITSTR_ELE* pstEls)
{
    pstEls->acRange[0] = '\0';
    pstEls->ucMatchType = EN_CCM_MATCHTYPE_BUTT;
    pstEls->ucMinRepCnt = 1;
    pstEls->ucMaxRepCnt = 1;
    pstEls->ucNotInRange = EOS_FALSE;
}

_VOID ccmDseDestruct(ST_CCM_DIGITSTR_ELE* pstEls){}

_NODE* ccmDsListInsertOrderly(_LIST* pLs, _NODE* pNode, FP_CCM_NODE_CMP fpCmp)
{
    _NODE* pstNode = eos_lst_first(pLs);

    while (pstNode)
    {
        if (fpCmp(pNode, pstNode))
        {
            eos_lst_insert(pLs, pstNode->pstPrev, pNode);
            return pNode;
        }

        pstNode = eos_lst_next(pstNode);
    }

    eos_lst_add(pLs, pNode);
    return pNode;
}

_BOOL ccmNodeMatchPriorMore(_NODE* pNode1, _NODE* pNode2)
{
    return (((ST_CCM_DIGITSTR_EX*)pNode1)->uiMatchPrior 
        > ((ST_CCM_DIGITSTR_EX*)pNode2)->uiMatchPrior);
}

_VOID ccmDigitStrNodeUpdate(ST_CCM_DIGITSTR_EX* pstDsn)
{
    ST_CCM_DIGITSTR_ELE* pstEle 
        = &pstDsn->pstDigitStr->astDigitStrEles[pstDsn->stStatus.ucDseNo];

    if (pstDsn->stStatus.ucRepCnt >= pstEle->ucMinRepCnt)
    {
        if (pstDsn->stStatus.ucRepCnt < pstEle->ucMaxRepCnt)
        {
            pstDsn->stStatus.ucMinRepCnt++;
            UT_QueuePush(pstDsn->hPartMatched, *(_VOID**)&pstDsn->stStatus);
        }

        pstDsn->stStatus.ucDseNo++;
        pstDsn->stStatus.ucMinRepCnt = (++pstEle)->ucMinRepCnt;
        pstDsn->stStatus.ucStart += pstDsn->stStatus.ucRepCnt;
        pstDsn->stStatus.ucRepCnt = 0;
    }
}

_VOID ccmDialStrShow(H_CCM_DIALSTR hDialStr)
{
    _INT32 i;
    ST_CCM_DIALSTR* pstDialStr = (ST_CCM_DIALSTR*)hDialStr;
    _NODE* node = eos_lst_first(&pstDialStr->lsDigitStr);    
    _INT32 size = eos_lst_count(&pstDialStr->lsDigitStr);
    
    UT_INFO("hDialStr = 0x%08x, iListSize = %d\r\n", 
        (_UINT32)hDialStr, size);

    for (i=0; i<size; i++)
    {
        _INT32 j;
        ST_CCM_DIGITSTR* pstDigitStr = (ST_CCM_DIGITSTR*)node;
        _CHAR8 acBuf[1024];

        acBuf[0] = '\0';
        for (j=0; j<pstDigitStr->uiEleNum; j++)
        {
            sprintf(acBuf+strlen(acBuf), "[%s]", 
                pstDigitStr->astDigitStrEles[j].acRange);
        }
        sprintf(acBuf+strlen(acBuf), "[%s]", "\r\n");
        UT_INFO("%s", acBuf);

        acBuf[0] = '\0';
        for (j=0; j<pstDigitStr->uiEleNum; j++)
        {
            sprintf(acBuf+strlen(acBuf), "[%d]", 
                pstDigitStr->astDigitStrEles[j].ucMatchType);
        }
        sprintf(acBuf+strlen(acBuf), "[%s]", "\r\n");
        UT_INFO("%s", acBuf);
        
        node = eos_lst_next(node);
    }
    
    return;
}

_VOID ccmNumAnaShow(ST_CCM_NUMANA* pstNumAna)
{
    _INT32 i;
    _NODE* apstNode[3];
    const _CHAR8* apcListName[3] = {"current", "matched", "backup"};

    apstNode[0] = eos_lst_first(&pstNumAna->lsCurrent);
    apstNode[1] = eos_lst_first(&pstNumAna->lsMatched);
    apstNode[2] = eos_lst_first(&pstNumAna->lsBackup);
    
    for (i=0; i<sizeof(apstNode)/sizeof(apstNode[0]); i++)
    {
        _NODE* pstNode;

        UT_INFO(" %s digit string list: %s", apcListName[i], "\r\n");
        pstNode = apstNode[i];
        while (pstNode)
        {
            ST_CCM_DIGITSTR_EX* pstDgsEx = (ST_CCM_DIGITSTR_EX*)pstNode;

            UT_INFO(" %s matched %d(%d,%d) digits\r\n", 
                pstDgsEx->pstDigitStr->pcDigitStr, 
                pstDgsEx->stStatus.ucStart + pstDgsEx->stStatus.ucRepCnt,
                pstDgsEx->stStatus.ucDseNo, pstDgsEx->stStatus.ucRepCnt);
        
            pstNode = eos_lst_next(pstNode);
        }
    }

    UT_INFO("\r\n");
    
    return;
}

_BOOL ccmDigitStrCmpByStr(_NODE * node, const _VOID * pKey)
{
    return (0 == strcmp(((ST_CCM_DIGITSTR*)node)->pcDigitStr, (_VOID*)pKey));
}

#endif

#if defined(__cplusplus)
}
#endif

