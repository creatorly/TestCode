#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

static pid_t    *childpid = NULL;
                        /* ptr to array allocated at run-time */
static int        maxfd;    /* from our open_max(), {Prog openmax} */

#define    SHELL    "/bin/sh"

static int fdPopenGetPipeName(const char* spipe, char* cpipe, int size);
static int fdPopen(const char* cmd, const char* spipe);
static int utPopenCmd2Line(const char* cmd, char* cmd_line, int size);
static int utPopen(const char* cmd, bool trace, char* obuf, int size);
static FILE* eos_popen(const char *cmdstring, const char *type);
static int eos_pclose(FILE *fp);

static FILE* eos_popen(const char *cmdstring, const char *type)
{
    int        i, pfd[2];
    pid_t    pid;
    FILE    *fp;

            /* only allow "r" or "w" */
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
        errno = EINVAL;        /* required by POSIX.2 */
        return(NULL);
    }

    if (childpid == NULL) {        /* first time through */
                /* allocate zeroed out array for child pids */
        struct rlimit rlim;
        getrlimit(RLIMIT_NOFILE, &rlim);

        maxfd = rlim.rlim_cur;
        if ( (childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
            return(NULL);
    }

    if (pipe(pfd) < 0)
        return(NULL);    /* errno set by pipe() */

    if ( (pid = fork()) < 0)
        return(NULL);    /* errno set by fork() */
    else if (pid == 0) {                            /* child */
        /* to unblock the signals blocked by eos_sigstart() */
        //eos_sigclear();
        
        if (*type == 'r') {
            close(pfd[0]);
            if (pfd[1] != STDOUT_FILENO) {
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]);
            }
        } else {
            close(pfd[1]);
            if (pfd[0] != STDIN_FILENO) {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]);
            }
        }
            /* close all descriptors in childpid[] */
        for (i = 0; i < maxfd; i++)
            if (childpid[ i ] > 0)
                close(i);

        execl(SHELL, "sh", "-c", cmdstring, (char *) 0);
        _exit(127);
    }
                                /* parent */
    if (*type == 'r') {
        close(pfd[1]);
        if ( (fp = fdopen(pfd[0], type)) == NULL)
            return(NULL);
    } else {
        close(pfd[0]);
        if ( (fp = fdopen(pfd[1], type)) == NULL)
            return(NULL);
    }
    childpid[fileno(fp)] = pid;    /* remember child pid for this fd */

    return(fp);
}

static int eos_pclose(FILE *fp)
{
    int        fd, stat;
    pid_t    pid;

    if (childpid == NULL)
        return(-1);        /* popen() has never been called */

    fd = fileno(fp);
    if ( (pid = childpid[fd]) == 0)
        return(-1);        /* fp wasn't opened by popen() */

    childpid[fd] = 0;
    if (fclose(fp) == EOF)
        return(-1);

    while (waitpid(pid, &stat, 0) < 0)
        if (errno != EINTR)
            return(-1);    /* error other than EINTR from waitpid() */

    return(stat);    /* return child's termination status */
}

static int fdPopenGetPipeName(const char* spipe, char* cpipe, int size)
{
    int len;

    len = snprintf(cpipe, size, "%s", spipe);
    if (len >= size)
    {
        printf("spipe too long! filename = %s\n", spipe);
        return -1;
    }

    while (len > 0 && cpipe[len - 1] != '/')
    {
        --len;
    }

    pid_t tid = 11;
    
    len += snprintf(cpipe+len, sizeof(cpipe)-len, "%d", (int) tid);
    if (len >= size)
    {
        printf("spipe too long! filename = %s\n", spipe);
        return -1;
    }

    return len;
}

static int fdPopen(const char* cmd, const char* spipe)
{
    char cpipe[512];

    if (fdPopenGetPipeName(spipe, cpipe, sizeof(cpipe)) < 1)
    {
        return -1;
    }
    
    int svr_fd = open(spipe, O_WRONLY);
    if (svr_fd < 0)
    {
        printf("open server pipe failed! filename = %s\n", spipe);
        return -1;
    }

    unlink(cpipe);   /* delete the existed file first */
    if (mkfifo(cpipe, 0777) < 0)
    {
        printf("create client pipe failed! filename = %s\n", cpipe);
        close(svr_fd);
        
        return -1;
    }

    char msg[512];
    int mlen;

    mlen = snprintf(msg, sizeof(msg), 
        "cmd=%zd,%srsp=%zd,%s\n", strlen(cmd), cmd, strlen(cpipe), cpipe);
    
    if (mlen >= sizeof(msg))
    {
        printf("command too long! command = %s\n", cmd);
        
        unlink(cpipe);
        close(svr_fd);

        return -1;
    }

    write(svr_fd, msg, mlen);
    close(svr_fd);

    int cli_fd = open(cpipe, O_RDONLY);
    if (cli_fd < 0)
    {
        printf("open client pipe failed! filename = %s\n", cpipe);
        unlink(cpipe);

        return -1;
    }

    return cli_fd;
}

static int utPopenCmd2Line(const char* cmd, char* cmd_line, int size)
{
    const char* pCmd;
    int len = 0;

    for (pCmd = cmd; *pCmd && len + 2 < size; ++pCmd)
    {
        if ('\r' == *pCmd)
        {
            cmd_line[len++] = '\\';
            cmd_line[len++] = 'r';
        }
        else if ('\n' == *pCmd)
        {
            cmd_line[len++] = '\\';
            cmd_line[len++] = 'n';
        }
        else
        {
            cmd_line[len++] = *pCmd;
        }
    }

    cmd_line[len] = '\0';

    return len;
}

static int utPopen(const char* cmd, bool trace, char* obuf, int size)
{
    char cmd_out[2048];
    char outline[512];
    int len = 0;

    if (obuf != NULL && size > 0)
    {
        *obuf = '\0';
    }

    if (trace)
    {
        printf("popen run ... cmd = \r\n%s\r\n", cmd);
    }
    
#if 0
    char cmd_line[512];
    utPopenCmd2Line(cmd, cmd_line, sizeof(cmd_line));

    int ofd = fdPopen(cmd_line, "vshd");
    if (ofd < 0)
    {
        printf("popen failed! errno = %d, cmd = %s", trace ? cmd : "***");
        return ofd;
    }

    int llen = 0;       /* line length */
    while ((llen = read(ofd, outline, sizeof(outline) - 1)) > 0)
    {
        outline[llen] = '\0';
        
        if (len < sizeof(cmd_out))
        {
            len += snprintf(cmd_out+len, sizeof(cmd_out)-len, "%s", outline);
        }
    }

    close(ofd);
#else
    FILE* fp = eos_popen(cmd, "r");

    if (NULL == fp)
    {
        printf("popen failed! errno = %d, cmd = %s", trace ? cmd : "***");
        return -1;
    }

    while (fgets(outline, sizeof(outline), fp))
    {
        if (len < sizeof(cmd_out))
        {
            len += snprintf(cmd_out+len, sizeof(cmd_out)-len, "%s", outline);
        }
    }

    eos_pclose(fp);
#endif

    if (len >= sizeof(cmd_out))
    {
        len = sizeof(cmd_out) - 1;
    }

    cmd_out[len] = '\0';

    if (trace)
    {
        printf("popen OK! output = \r\n%s\r\n", cmd_out);
    }
    
    if (len > 0 && obuf != NULL && size > 0)
    {
        if (len >= size)
        {
            len = size - 1; 
        }

        memcpy(obuf, cmd_out, len);
        obuf[len] = '\0';
    }
    
    return len;
}

int UT_Popen(const char* cmd, char* obuf, int size)
{
    return utPopen(cmd, true, obuf, size);
}

int UT_PopenNolog(const char* cmd, char* obuf, int size)
{
    return utPopen(cmd, false, obuf, size);
}


