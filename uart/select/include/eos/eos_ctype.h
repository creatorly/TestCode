#ifndef __ECTYPE_H
#define __ECTYPE_H

#ifdef __cplusplus
extern "C" {
#endif


#define _EC_UPPER                0x1
#define _EC_LOWER                0x2
#define _EC_NUMBER                0x4
#define _EC_WHITE_SPACE            0x8
#define _EC_PUNCT                0x10
#define _EC_CONTROL                0x20
#define _EC_HEX_NUMBER            0x40
#define _EC_B                    0x80

__EXTERN__    __CONST__ _UCHAR8* __ectype;

#define _e_isalpha(c)    (__ectype[c] & (_EC_UPPER | _EC_LOWER))
#define _e_isupper(c)    (__ectype[c] & (_EC_UPPER))
#define _e_islower(c)    (__ectype[c] & (_EC_LOWER))
#define _e_isdigit(c)    (__ectype[c] & (_EC_NUMBER))
#define _e_isxdigit(c)   (__ectype[c] & (_EC_HEX_NUMBER))
#define _e_isspace(c)    (__ectype[c] & (_EC_WHITE_SPACE | _EC_CONTROL))
#define _e_ispunct(c)    (__ectype[c] & (_EC_PUNCT))
#define _e_isalnum(c)    (__ectype[c] & (_EC_UPPER | _EC_LOWER | _EC_NUMBER))
#define _e_isprint(c)    (__ectype[c] & (_EC_PUNCT | _EC_UPPER | _EC_LOWER | \
                       _EC_WHITE_SPACE | _EC_NUMBER))
#define _e_isgraph(c)    (__ectype[c] & (_EC_PUNCT | _EC_UPPER | _EC_LOWER | \
                       _EC_NUMBER))
#define _e_iscntrl(c)    (__ectype[c] & (_EC_CONTROL | _EC_B))
#define _e_toupper(c)    ((('a' <= (c))&&((c) <= 'z')) ? ((c) - 'a' + 'A') : (c))
#define _e_tolower(c)    ((('A' <= (c))&&((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))

#define eosisalpha(c)      _e_isalpha(c)
#define eosisupper(c)      _e_isupper(c)
#define eosislower(c)      _e_islower(c)
#define eosisdigit(c)      _e_isdigit(c)
#define eosisxdigit(c)     _e_isxdigit(c)
#define eosisspace(c)      _e_isspace(c)
#define eosispunct(c)      _e_ispunct(c)
#define eosisalnum(c)      _e_isalnum(c)
#define eosisprint(c)      _e_isprint(c)
#define eosisgraph(c)      _e_isgraph(c)
#define eosiscntrl(c)      _e_iscntrl(c)
#define eostoupper(c)      _e_toupper(c)
#define eostolower(c)      _e_tolower(c)

#ifdef __cplusplus
}
#endif

#endif 


