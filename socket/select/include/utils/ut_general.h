#define    UT_DEFAULT_STR_MAX                    32
#define    UT_NULL_CHECK(_x)            \
            if((_VOID*)(_x)==(_VOID*)NULL)    return EOS_ERROR;
            
#define    UT_NULL_STR_CHECK(_x)        \
        if(*(_CHAR8*)(_x)==0)    return EOS_ERROR;
