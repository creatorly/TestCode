
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char* argv[])
{
    printf("main\n");
    return 0;
}

#ifdef __cplusplus
}
#endif

