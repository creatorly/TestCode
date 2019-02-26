
#ifndef EOS_CLIPAR_H
#define EOS_CLIPAR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstEosCliVa
{
    _LIST lsParas;
}ST_EOS_CLIVA;

ST_EOS_CLIVA* EOS_CliVaBegin(ST_EOS_CLIVA* va, const _CHAR8* delimiters, _CHAR8* str);
ST_EOS_CLIVA* EOS_CliVaBeginEx(ST_EOS_CLIVA* va, 
                              const _CHAR8* delimiters, 
                              const _CHAR8* equalSigns,
                              _CHAR8* str);

_VOID EOS_CliVaEnd(ST_EOS_CLIVA* va);    
EOSSize_t EOS_CliVaSize(ST_EOS_CLIVA* va);

/* get current argval with the argname returned */
const _CHAR8* EOS_CliVaArg(ST_EOS_CLIVA* va, const _CHAR8** argname);
const _CHAR8* EOS_CliVaPop(ST_EOS_CLIVA* va, const _CHAR8** argname);   /* if found, delete it */

/* get the specified arg value by it's name */
const _CHAR8* EOS_CliVaGetArg(ST_EOS_CLIVA* va, const _CHAR8* argname);
const _CHAR8* EOS_CliVaGetPop(ST_EOS_CLIVA* va, const _CHAR8* argname); /* if found, delete it */

#if defined(__cplusplus)
}
#endif 

#endif

