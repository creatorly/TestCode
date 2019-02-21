
#ifndef __UT_DIALSTR_H__
#define __UT_DIALSTR_H__

#ifdef ENABLE_UT_DIALSTR

typedef enum
{
    EN_CCM_NASTATUS_DONE = 0,
    EN_CCM_NASTATUS_CONTINUE,
    EN_CCM_NASTATUS_SKIP,
    EN_CCM_NASTATUS_FAIL,

    EN_CCM_NASTATUS_BUTT
}EN_CCM_NASTATUS;


typedef _VOID* H_CCM_DIALSTR;
typedef _VOID* H_CCM_DIGITSTR;
typedef _VOID* H_CCM_MATCHSTR;

typedef _UINT32 (*FP_CCM_DIGITSTR_CB)(H_CCM_MATCHSTR* phMatchStr, 
                                      H_CCM_DIGITSTR hDgStr, 
                                     _CHAR8* pcDialStr, 
                                     _VOID* data);

_UINT32 CCM_DialStrConstruct(H_CCM_DIALSTR* phDialStr);
_UINT32 CCM_DialStrDestruct(H_CCM_DIALSTR hDialStr);

_UINT32 CCM_DialStrAddPrefix(H_CCM_DIALSTR hDialStr, 
                             const _CHAR8* pcPrefix, 
                             const _CHAR8* pcReplace);

/* if argument pcDialPlan is equal NULL, default dialplan will be added! */
_UINT32 CCM_DialStrAddDialPlan(H_CCM_DIALSTR hDialStr, 
                              const _CHAR8* pcDialPlan, 
                               FP_CCM_DIGITSTR_CB fp, 
                              _VOID* data);

_UINT32 CCM_DialStrDelPrefix(H_CCM_DIALSTR hDialStr, _CHAR8 * pcPrefix);
_UINT32 CCM_DialStrDelDialPlan(H_CCM_DIALSTR hDialStr, _CHAR8 * pcDialPlan);

_UINT32 CCM_DialStrPrefixSize(H_CCM_DIALSTR hDialStr);
_UINT32 CCM_DialStrDialPlanSize(H_CCM_DIALSTR hDialStr);

_UINT32 CCM_DialStrRemoveDigitStr(H_CCM_DIALSTR hDialStr, H_CCM_DIGITSTR hDigitStr);
_UINT32 CCM_DialStrRegisterCB(H_CCM_DIALSTR hDialStr, 
                             _CHAR8* pcPattern, 
                              FP_CCM_DIGITSTR_CB fp, 
                             _VOID* data);

_CHAR8* CCM_DigitStrGetDialPlan(H_CCM_DIGITSTR hDigitStr);
FP_CCM_DIGITSTR_CB CCM_DigitStrGetFunc(H_CCM_DIGITSTR hDigitStr);
_VOID*  CCM_DigitStrGetData(H_CCM_DIGITSTR hDigitStr);

_UINT32 CCM_MatchStrConstruct(H_CCM_MATCHSTR* phMatchStr, H_CCM_DIALSTR hDialStr);
_UINT32 CCM_MatchStrDestruct(H_CCM_MATCHSTR hMatchStr);
_UINT32 CCM_MatchStrReset(H_CCM_MATCHSTR hMatchStr);

EN_CCM_NASTATUS CCM_MatchStrGetStatus(H_CCM_MATCHSTR hMatchStr);
EN_CCM_NASTATUS CCM_MatchStrUpdate(H_CCM_MATCHSTR hMatchStr, H_CCM_DIGITSTR* phDigitStr);

EN_CCM_NASTATUS CCM_MatchStrAddDigit(H_CCM_MATCHSTR hMatchStr, 
                                    _CHAR8 cDigit, 
                                     H_CCM_DIGITSTR* phDigitStr);

EN_CCM_NASTATUS CCM_MatchStrAddDigits(H_CCM_MATCHSTR hMatchStr, 
                                      const _CHAR8* pcDigits,
                                      H_CCM_DIGITSTR* phDigitStr);

#endif

#endif

