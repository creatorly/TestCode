
#ifndef __EOS_TICK_H
#define __EOS_TICK_H

#include <eos_typedef.h>

#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
#include <sysLib.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define EOS_MS2TICK(ms) \
    (((ms)/1000)*sysClkRateGet()+ ((ms)%1000)*sysClkRateGet()/1000)

#define EOS_TICK2MS(tk) \
    (((tk)/sysClkRateGet())*1000 + ((tk)%sysClkRateGet())*1000/sysClkRateGet())

#if (EJOIN_OS_TYPE != EJOIN_OS_TYPE_VXWORKS)

_ULONG32 tickGet();

/* used when updating the system time */
_VOID tickSet(_ULONG32 tick);

int sysClkRateGet();
int sysClkRateSet(int ticksPerSecond);

#endif

#if defined(__cplusplus)
}
#endif

#endif

