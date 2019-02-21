
#ifndef UT_FNCHK_H
#define UT_FNCHK_H

#if defined(__cplusplus)
extern "C" {
#endif

/* don't modify the magic cookie */
#define EJOIN_FILENAME_MAGIC_COOKIE "ejoin_filename_magic_cookie"

_CHAR8* UT_FilenameAddCookie(_CHAR8* filename);
_BOOL UT_FilenameVerify(_CHAR8* filename);

#if defined(__cplusplus)
}
#endif

#endif

