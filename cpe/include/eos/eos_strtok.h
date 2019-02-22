
#ifndef EOS_STRTOK_H
#define EOS_STRTOK_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstEosStrTok
{
    const _CHAR8* delimiters;
    _CHAR8* p;
    _CHAR8* pEnd;

    _CHAR8 bRestore;
    _CHAR8 dlmt;
    _CHAR8 pads[2];     /* for 4-bytes align */
}ST_EOS_STRTOK, eos_token_t;

ST_EOS_STRTOK* EOS_StrTokConstruct(ST_EOS_STRTOK* t, const _CHAR8* delimiters, _CHAR8* str);
_VOID EOS_StrTokDestruct(ST_EOS_STRTOK* t);       
_CHAR8* EOS_StrTokGetToken(ST_EOS_STRTOK* t);

_VOID eos_token_init(eos_token_t* t, _CHAR8* s, const _CHAR8* dlmts, _BOOL restore);
_VOID eos_token_free(eos_token_t* t);       
const _CHAR8* eos_token_get(eos_token_t* t);

#if defined(__cplusplus)
}
#endif 

#endif

