/********************************************************
 * File:         main.c
 * PACKAGE:
 *   Date:               2019.02.19
 *   Author:             Linye
 *   Version:            1.0
 *
 * Change
 *
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <eos.h>
#include <asys_init.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VAD_VAR_SELECT_PID         "/opt/ejoin/var/select.pid"
static int g_selectShutdown = 0;

void selectMainOnSigInt(int sig) {
    g_selectShutdown = 1;
}

void selectMainOnSigPipe(int sig) {
}

int main(int argc, char *argv[]) 
{
    static struct sigaction action;
    int bFork;

    if (bFork) {
        int cpid = fork();

        if (cpid)
            exit(0);

        /* child process */
    }

    action.sa_handler = selectMainOnSigPipe;
    sigemptyset(&action.sa_mask); /* only block the signal itself */
    action.sa_flags = 0; /* don't interrupt the blocking I/O */

    /* the signal action will be always efficient */
    sigaction(SIGPIPE, &action, NULL);
    signal(SIGINT, selectMainOnSigInt);
    
    eos_init();
    eos_task_set_pidfile(VAD_VAR_SELECT_PID);
    
    if (Asys_Start()) {
        printf("failed to start select!\n");
        Asys_Delete();
        return -3;
    }

    while (!g_selectShutdown) {
        sleep(1);
    }

    Asys_Delete();

    return 0;
}

#ifdef __cplusplus
}
#endif

