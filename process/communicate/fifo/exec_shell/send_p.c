#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include "ut_popen.h"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("pls usage:%s command\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    unsigned char buf[512];

    UT_Popen(argv[1], buf, sizeof(buf));

    printf("buf:%s\n", buf);
}
