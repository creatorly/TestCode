
#define CCM_DIAL_DIGIT_NUM    32
#define CCM_DIAL_DIGIT_RANGE_NUM   32
#define CCM_DIAL_STAR           '*'
#define CCM_DIAL_DIGIT          'd'
#define CCM_DIAL_WILDCARD       'x'
#define CCM_DIAL_END            '#'

#define CCM_DIAL_WILDSET        "0123456789*"
#define CCM_DIAL_DIGITSET       "0123456789"

#define CCM_DSP_CHARSET         "0123456789*dx#"

#define CCM_DSP_RANGE_BEGIN     '['
#define CCM_DSP_RANGE_END       ']'
#define CCM_DSP_RANGE_NOT       '^'
#define CCM_DSP_AREA_SEP        ','
#define CCM_DSP_AREA_FLAG       '-'

#define CCM_DSP_REPEAT_BEGIN    '{'
#define CCM_DSP_REPEAT_END      '}'
#define CCM_DSP_REPEAT_NORO     '?'
#define CCM_DSP_REPEAT_OORM     '.'
#define CCM_DSP_ADDFLG  '+'
#define CCM_DSP_DECFLG  '-'

#define CCM_DSP_REPEAT_MAX      255

#define CCM_DSP_SPACE   " \t"

typedef enum
{
    /* don't change the order of the enum val!!! */
    EN_CCM_MATCHTYPE_WILD = 0,
    EN_CCM_MATCHTYPE_FUZZY,
    EN_CCM_MATCHTYPE_EXACT,

    EN_CCM_MATCHTYPE_BUTT
}EN_CCM_MATCHTYPE;

typedef struct
{
    _CHAR8 acRange[CCM_DIAL_DIGIT_RANGE_NUM];
    _UCHAR8 ucNotInRange;
    _UCHAR8 ucMatchType;    
    _UCHAR8 ucMinRepCnt;
    _UCHAR8 ucMaxRepCnt;
}ST_CCM_DIGITSTR_ELE;

typedef struct tagstCcmDigitString
{
    _NODE node;
    _UINT32 uiEleNum;
    H_UT_QUEUE hCurDses;    /* current digitstring elements */
    ST_CCM_DIGITSTR_ELE astDigitStrEles[CCM_DIAL_DIGIT_NUM];
    _CHAR8* pcDigitStr;
    FP_CCM_DIGITSTR_CB fpMatched;
    _VOID* cbData;
}ST_CCM_DIGITSTR;

typedef struct tagstCcmDialString
{
    _LIST lsDigitStr;
    _LIST lsPrefix;
}ST_CCM_DIALSTR;

struct tagstCcmNumAnanysys;
typedef struct tagstCcmNumAnanysys ST_CCM_NUMANA;
typedef _CHAR8* (*FP_CCM_NUMANA_CB)(ST_CCM_NUMANA* pstNumAna, 
                                   _CHAR8* pcNewNums, 
                                   _VOID* data);

typedef struct tagstCcmDigitStrEleStatus
{
    _UCHAR8 ucDseNo;
    _UCHAR8 ucStart;
    _UCHAR8 ucMinRepCnt;
    _UCHAR8 ucRepCnt;
}ST_CCM_DSE_STATUS;

typedef struct tagstCcmDigitStrEx
{
    _NODE node;
    _UINT32 uiMatchPrior;
    ST_CCM_DSE_STATUS stStatus;
    H_UT_QUEUE hPartMatched;    /* ST_CCM_DSE_STATUS queue */
    ST_CCM_DIGITSTR* pstDigitStr;
}ST_CCM_DIGITSTR_EX;

struct tagstCcmNumAnanysys
{
    _UINT32 uiNodeNum;
    ST_CCM_DIGITSTR_EX* pstNodeMem;
    _LIST lsCurrent;
    _LIST lsMatched;
    _LIST lsBackup;
    EN_CCM_NASTATUS enStatus;
    _UINT32 uiCurDialStrLen;
    _CHAR8 acCurDialStr[CCM_DIAL_DIGIT_NUM];
    FP_CCM_NUMANA_CB fpMatched;
    _VOID* data;
};

typedef struct tagstCcmNumPrefix
{
    _UINT32 uiPrefixLen;
    _CHAR8* pcPrefix;
    _CHAR8* pcReplace;
}ST_CCM_NUMPRE;

typedef struct tagstCcmNumAna
{
    ST_CCM_NUMANA stCcmNaNum;
    ST_CCM_NUMANA stCcmNaPrefix;
    ST_CCM_NUMPRE stPrefix;
}ST_CCM_MATCHSTR;

#define CcmDsMalloc(s)    UT_MALLOC(s)
#define CcmDsFree(p)     UT_FREE((_VOID*)(p))

#define CcmDsStrInRange(s, c) (strchr(s, c) && (c))
#define CcmDsIsDigit(c) (((_UCHAR8)((c)-'0')) < 10)
#define CcmDsIsSpace(c) CcmDsStrInRange(CCM_DSP_SPACE, c)
#define CcmDsSkipSpace(pc) while(CcmDsIsSpace(*(pc))) (pc)++

#define CcmDsLstExtractToHead(src, begin, end, dst) \
do{ \
    _LIST __tmp; \
    eos_lst_extract(src, begin, end, &__tmp); \
    eos_lst_concat(&__tmp, dst); \
    memcpy(dst, &__tmp, sizeof(_LIST)); \
}while(0)

_VOID ccmDseConstruct(ST_CCM_DIGITSTR_ELE* pstEls);
_VOID ccmDseDestruct(ST_CCM_DIGITSTR_ELE* pstEls);
_BOOL ccmDigitStrEleIsMatch(ST_CCM_DIGITSTR_ELE* pstEle, _CHAR8 cDigit);
_BOOL ccmDigitStrExMatching(ST_CCM_DIGITSTR_EX* pstDsEx, 
                           _CHAR8* pcDialStr, 
                           _UINT32 uiLen);

_BOOL ccmMatchStrUpdate(ST_CCM_NUMANA* pstNumAna);
_BOOL ccmDigitStrMatchEnded(ST_CCM_DIGITSTR_EX* pstDsEx);

_BOOL ccmDspIsValidDigit(_CHAR8 cDigit);
_BOOL ccmDigitStrEleAddDigit(ST_CCM_DIGITSTR_ELE* pstEls, _CHAR8 cDigit);
_BOOL ccmDigitStrEleAddDigits(ST_CCM_DIGITSTR_ELE* pstEls, _CHAR8 cMin, _CHAR8 cMax);

_BOOL ccmDigitStringParser(ST_CCM_DIGITSTR* pstDgStr, const _CHAR8* pcDgStr);

typedef _BOOL (*FP_CCM_NODE_CMP)(_NODE* pNode1, _NODE* pNode2);
typedef _VOID* (*FP_CCM_DSPARSER)(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data);
_VOID* ccmDspNormal(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data);
_VOID* ccmDspRange(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data);
_VOID* ccmDspRepeat(ST_CCM_DIGITSTR* pstDigitStr, _CHAR8** ppDialStr, _VOID** data);
_NODE* ccmDsListInsertOrderly(_LIST* pLs, _NODE* pNode, FP_CCM_NODE_CMP fpCmp);
_BOOL ccmNodeMatchPriorMore(_NODE* pNode1, _NODE* pNode2);
_VOID ccmDigitStrNodeUpdate(ST_CCM_DIGITSTR_EX* pstDsn);

_UINT32 CCM_DigitStrConstruct(H_CCM_DIGITSTR* phDigitStr, 
                             const _CHAR8* pcDialDigit, 
                              FP_CCM_DIGITSTR_CB fpMatch, 
                             _VOID* data);
_UINT32 CCM_DigitStrDestruct(H_CCM_DIGITSTR hDigitStr);

_UINT32 ccmNumAnaConstruct(ST_CCM_NUMANA* pstNumAna, 
                          _LIST* plsDigitStr, 
                          FP_CCM_NUMANA_CB fpMatched, 
                          _VOID* data);
_UINT32 ccmNumAnaDestruct(ST_CCM_NUMANA* pstNumAna);
_VOID ccmNumAnaReset(ST_CCM_NUMANA* pstNumAna);

EN_CCM_NASTATUS ccmDigitStrOnMatchEnd(ST_CCM_NUMANA* pstNumAna,
                                      ST_CCM_DIGITSTR_EX* pstDgsEx,
                                      H_CCM_DIGITSTR* phDigitStr);

EN_CCM_NASTATUS ccmNumAnaAddDigit(ST_CCM_NUMANA* pstNumAna, 
                                  _CHAR8 cDigit,
                                  H_CCM_DIGITSTR* phDigitStr);
_VOID ccmNumAnaShow(ST_CCM_NUMANA* pstNumAna);
_BOOL ccmDigitStrCmpByStr(_NODE* node, const _VOID* pKey);

