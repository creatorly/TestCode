#include <eos.h>
#include <eos_timer.h>
#include "wireless_interface.h"

_INT32 Asys_Start(_VOID)
{
    _INT32 ret; 
    printf("asys start\r\n");

    if (EOS_OK != UT_Init())
    {
        printf("init utils failed!\r\n");
        return EOS_ERROR;
    }

    if (EOS_OK != UT_Start((EOS_TIMER_ID)1))
    {
        printf("init utils failed!\r\n");
        return EOS_ERROR;
    }
    
    if (EOS_OK != WIRELESS_Init(1))
	{
	    printf("WIRELESS_Init failed!\r\n");
		return EOS_ERROR;
	}
    
	if (EOS_OK != WIRELESS_Start())
	{
	    printf("WIRELESS_Start failed!\r\n");
		return EOS_ERROR;
	}

    return EOS_OK;
}

_VOID Asys_Delete(_VOID)
{
    printf("asys delete\r\n");
}

#ifdef __cplusplus
}
#endif

