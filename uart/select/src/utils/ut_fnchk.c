
#include "eos.h"
#include "ut_debug.h"
#include "ut_udp.h"

#include "ut_fnchk.h"

#if defined(__cplusplus)
extern "C" {
#endif

_CHAR8* UT_FilenameAddCookie(_CHAR8* filename)
{
    _USHORT16 len = strlen(filename);
    _USHORT16 sum = UT_UdpChkSum(filename, len);

    sum -= (EJOIN_FILENAME_MAGIC_COOKIE[0] & (-(len&1)));
    sum += UT_UdpChkSum(&EJOIN_FILENAME_MAGIC_COOKIE[len&1], 
        sizeof(EJOIN_FILENAME_MAGIC_COOKIE)-1-(len&1));

    sprintf(&filename[len], "-%04X", sum);

    return filename;
}

_BOOL UT_FilenameVerify(_CHAR8* filename)
{
    _CHAR8* lastColon = 0;
    _CHAR8* colon = strchr(filename, '-');

    while(colon)
    {
        lastColon = colon;
        colon = strchr(colon+1, '-');
    }

    if (lastColon)
    {
        _UINT32 chksum = 0;
        _USHORT16 len = lastColon - filename;
        _USHORT16 sum = UT_UdpChkSum(filename, len);

        sum -= ((EJOIN_FILENAME_MAGIC_COOKIE[0]) & (-(len&1)));
        sum += UT_UdpChkSum(&EJOIN_FILENAME_MAGIC_COOKIE[len&1], 
            sizeof(EJOIN_FILENAME_MAGIC_COOKIE)-1-(len&1));        
        sscanf(lastColon+1, "%x", &chksum);

        return (sum == (_USHORT16)chksum);
    }

    return EOS_FALSE;
}

#if defined(__cplusplus)
}
#endif

