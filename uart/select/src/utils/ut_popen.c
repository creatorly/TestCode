/********************************************************
*
* Ejoin XP-workroom platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
*
*
* File:         ut_popen.c
* PACKAGE:
*   Implement of:       popen in variable uuwifi platform
*   Date:               2018.07.19
*   Author:             PengJian
*   Version:            1.0
*
* Change
*
*********************************************************/

#include <eos.h>
#include <eos_popen.h>
#include <utl_typdef.h>
#include <ut_popen.h>

#ifdef ANDROID
#include <eou_jni_2_java.h>
#endif

#include "ut_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VAD_HOME                "/opt/ejoin"
#define VAD_PIPE_DIR            VAD_HOME"/var/pipe"
#define VAD_PIPE_SHELLD         VAD_PIPE_DIR"/vshd"

int utPopenReadFile(const char* filename, char* buf, int size)
{
    int fd;
    int len;
    
    fd = open(filename, O_CREAT|O_RDONLY);
    if (-1 == fd)
    {
        eos_snprintf(buf, size, "open file failed! filename = %s", filename);
        return -1;
    }

    len = read(fd, buf, size);
    close(fd);
    
    if (len >= size)
    {
        len = size - 1;
    }

    buf[len] = '\0';

    return len;
}

#ifdef ANDROID

int utPopenInDir(const char* dir, const char* cmd, bool trace, char* outbuf, int osize, char* errbuf, int esize)
{
    const char* pCmd = cmd;
    char cmd2[512];
    char cmd_out[2048];
    char efile[256];
    int ret = -1;

    if (NULL == outbuf)
    {
        outbuf = cmd_out;
        osize = sizeof(cmd_out);
    }
    
    *outbuf = '\0';

    if (errbuf != NULL)
    {
        *errbuf = '\0';
        
        eos_snprintf(efile, sizeof(efile), "%s/var/cmd_err_%d.txt", VAD_HOME, rand());
        eos_snprintf(cmd2, sizeof(cmd2), "(%s) 2>%s", cmd, efile);

        pCmd = cmd2;
    }

    if (trace)
    {
        UT_INFO("popen run ... dir = %s, cmd = \r\n%s\r\n", dir, cmd);
    }
    
#ifdef VAD_CHANGE_DIR_ENABLED
    if (dir && dir[0])
    {
        ret = UFI_SysExecCommandWithDir(dir, pCmd, outbuf, osize);
    }
    else
    {
        ret = UFI_SysExecCommandWithOutput(pCmd, outbuf, osize);
    }
#else
    ret = UFI_SysExecCommandWithOutput(pCmd, outbuf, osize);
#endif

    if (ret != 0)
    {
        eos_snprintf(errbuf, esize, "exec command failed! cmds = %s", trace ? pCmd : "******");
        return -1;
    }

    if (trace)
    {
        UT_INFO("popen OK! output = \r\n%s\r\n", outbuf);
    }
    
    if (errbuf != NULL)
    {
        utPopenReadFile(efile, errbuf, esize);
        remove(efile);
    }

    return 0;
}

int UT_Popen(const char* cmd, char* outbuf, int osize)
{
    return utPopenInDir(NULL, cmd, true, outbuf, osize, NULL, 0);
}

int UT_PopenNolog(const char* cmd, char* outbuf, int osize)
{
    return utPopenInDir(NULL, cmd, false, outbuf, osize, NULL, 0);
}

int UT_PopenInDir(const char* dir, const char* cmd, char* outbuf, int osize, char* errbuf, int esize)
{
    return utPopenInDir(dir, cmd, true, outbuf, osize, errbuf, esize);
}

#else

int utPopenCmd2Line(const char* cmd, char* cmd_line, int size)
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

int utPopen(const char* cmd, bool trace, char* obuf, int size)
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
        UT_INFO("popen run ... cmd = \r\n%s\r\n", cmd);
    }
    
#if 0//(ACOM_BOARD_TYPE == MDM9X07)
    char cmd_line[512];
    utPopenCmd2Line(cmd, cmd_line, sizeof(cmd_line));

    int ofd = eos_fd_popen(cmd_line, VAD_PIPE_SHELLD);
    if (ofd < 0)
    {
        UT_ERROR(EOS_ERROR, "popen failed! errno = %d, cmd = %s", errno, trace ? cmd : "***");
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
        UT_ERROR(EOS_ERROR, "popen failed! errno = %d, cmd = %s", errno, trace ? cmd : "***");
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
        UT_INFO("popen OK! output = \r\n%s\r\n", cmd_out);
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

int UT_PopenInDir(const char* dir, const char* cmd, char* obuf, int size, char* ebuf, int esize)
{
    char cmd2[512];
    char efile[256];
    int len = 0;
    int ret = -1;
    
    if (dir && dir[0])
    {
        len = sprintf(cmd2, "cd %s; ", dir);
    }

    if (ebuf != NULL)
    {
        *ebuf = '\0';
        
        eos_snprintf(efile, sizeof(efile), "%s/var/cmd_err_%d.txt", VAD_HOME, rand());
        eos_snprintf(cmd2+len, sizeof(cmd2)-len, "(%s) 2>%s", cmd, efile);
    }
    else
    {
        eos_snprintf(cmd2+len, sizeof(cmd2)-len, "%s", cmd);
    }

    if ((ret = utPopen(cmd2, true, obuf, size)) < 0)
    {
        return ret;
    }

    if (ebuf != NULL)
    {
        utPopenReadFile(efile, ebuf, esize);
        remove(efile);
    }
    
    return ret;
}

#endif

#ifdef __cplusplus
}
#endif

