/********************************************************
* 
*
* Ejoin SIM Cloud platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:                 ut_fselect.c
* PACKAGE:      
*   Implement of:       ut file monitor
*   Date:               2018.08.04
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_SYS_INOTIFY_H
#include <sys/inotify.h>
#endif

#include <eos.h>
#include <utl_string.h>
#include <utl_list.h>
#include <utl_pipe.h>

#include <ut_mem.h>
#include "ut_debug.h"
#include "ut_fselect.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtilsFileSelect
{
    _INT32 fs_inotify_fd;
    _INT32 fs_max_fd;

    _INT32 fs_priority;
    _INT32 fs_stack_size;

    SEM_ID fs_mutex;

    /* used to wakeup the main task */
    utl_sock fs_sock_tx;
    utl_sock fs_sock_rx;
    
    utl_list_t fs_fds;
}ST_UT_FSELECT;

typedef struct tagstUtilsFselectFile
{
    utl_node_t fs_node;
    
    _INT32 fs_fd;               /* file descriptor for UT_FSelectAddFd() */
    _INT32 fs_wd;               /* Watch descriptor for UT_FSelectAddFile() */
    _CHAR8* fs_name;
    
    FP_UT_FSELECT_READ fs_fp_read;
    _VOID* fs_usr_data;
}ST_UT_FSELECT_FILE;

static ST_UT_FSELECT g_wrlFSelect;

#define UT_FSELECT_LOCK(fs)     semTake((fs)->fs_mutex, WAIT_FOREVER)
#define UT_FSELECT_UNLOCK(fs)   semGive((fs)->fs_mutex)

ST_UT_FSELECT_FILE* utFSelectFileCreate(FP_UT_FSELECT_READ fpRead, _VOID* usrData)
{
    ST_UT_FSELECT_FILE* file;

    file = UT_TALLOC(ST_UT_FSELECT_FILE);
    if (NULL == file)
    {
        UT_WARN(-1, EOS_ERROR, "memory alloc failed!", "type = %s", "ST_UT_FSELECT_FILE");
        return NULL;
    }

    memset(file, 0, sizeof(*file));

    file->fs_fp_read = fpRead;
    file->fs_usr_data = usrData;

    return file;
}

_VOID utFSelectFileDelete(ST_UT_FSELECT_FILE* file)
{
    if (file->fs_name != NULL)
    {
        utl_free(file->fs_name);
    }
    
    UT_FREE(file);
}

_VOID utFSelectFileOnRead(ST_UT_FSELECT_FILE* file)
{
    if (file->fs_fp_read != NULL)
    {
        file->fs_fp_read(file->fs_fd, file->fs_name, file->fs_usr_data);
    }
}

bool utFSelectFileFdEqu(ST_UT_FSELECT_FILE* file, _INT32* pfd)
{
    return (*pfd == file->fs_fd);
}

bool utFSelectFileNameEqu(ST_UT_FSELECT_FILE* file, const char* fname)
{
    return (file->fs_name && !strcmp(fname, file->fs_name));
}

#ifdef HAVE_SYS_INOTIFY_H

bool utFSelectFileWdEqu(ST_UT_FSELECT_FILE* file, _INT32* pwd)
{
    return (file->fs_wd >= 0 && *pwd == file->fs_wd);
}

ST_UT_FSELECT_FILE* utFSelectFindByWd(ST_UT_FSELECT* fsel, _INT32 wd)
{
    return (ST_UT_FSELECT_FILE*) utl_list_findif(
                &fsel->fs_fds, (utl_list_cond_f) utFSelectFileWdEqu, &wd);
}

_VOID utFSelectOnINotifyEvent(ST_UT_FSELECT* fsel)
{
    _CHAR8 buf[1024];
    _INT32 len;

    len = read(fsel->fs_inotify_fd, buf, sizeof(buf));
    if (len > 0)
    {
        _CHAR8* p = buf;
        
        while (p < buf + len)
        {
            struct inotify_event* inEvt = (struct inotify_event*) p;
            ST_UT_FSELECT_FILE* file = utFSelectFindByWd(fsel, inEvt->wd);

            if (file != NULL)
            {
                UT_INFO("file or directory \"%s\" got events %x", file->fs_name, inEvt->mask);

                if (inEvt->mask & IN_MODIFY)
                {
                    /* a file is modified */
                    utFSelectFileOnRead(file);
                }
                else if (inEvt->mask & IN_IGNORED)
                {
                    /* a file is deleted */
                    utFSelectFileOnRead(file);
                }
            }
            
            p += (sizeof(*inEvt) + inEvt->len);
        }
    }
}

#endif

_VOID utFSelectWake(ST_UT_FSELECT* fsel)
{
    /* wakeup the main task */
    utl_sock_send(fsel->fs_sock_tx, "X", 1, NULL);
}

_INT32 utFSelectAdd(ST_UT_FSELECT* fsel, ST_UT_FSELECT_FILE* file)
{
    UT_FSELECT_LOCK(fsel);
    utl_list_add(&fsel->fs_fds, &file->fs_node);
    UT_FSELECT_UNLOCK(fsel);

    UT_INFO("fselect add file! fd = %d, filename = %s", file->fs_fd, file->fs_name);

    utFSelectWake(fsel);

    return EOS_OK;
}

ST_UT_FSELECT_FILE* utFSelectDel(ST_UT_FSELECT* fsel, utl_list_cond_f fpCond, const void* key)
{
    ST_UT_FSELECT_FILE* file;
    
    utFSelectWake(fsel);

    UT_FSELECT_LOCK(fsel);
    
    file = (ST_UT_FSELECT_FILE*) utl_list_findif(&fsel->fs_fds, fpCond, (void*) key);
    if (file != NULL)
    {
        UT_INFO("fselect del file! fd = %d, filename = %s", file->fs_fd, file->fs_name);
        
        utl_list_del(&fsel->fs_fds, &file->fs_node);
    }
    
    UT_FSELECT_UNLOCK(fsel);

    return file;
}

_VOID utFSelectMain(ST_UT_FSELECT* fsel)
{
    UT_INFO("file select task is started! "
        "priority = %d, stack_size = %d", fsel->fs_priority, fsel->fs_stack_size);

    while (1)
    {
        fd_set rdFds;

        FD_ZERO(&rdFds);

        FD_SET(utl_sock_fd(fsel->fs_sock_rx), &rdFds);

    #ifdef HAVE_SYS_INOTIFY_H
        FD_SET(fsel->fs_inotify_fd, &rdFds);
    #endif

        UT_FSELECT_LOCK(fsel);

        utl_node_t* pn = utl_list_begin(&fsel->fs_fds);
    
        while (pn != utl_list_end(&fsel->fs_fds))
        {
            ST_UT_FSELECT_FILE* file = (ST_UT_FSELECT_FILE*) pn;

            if (file->fs_fd >= 0)
            {
                FD_SET(file->fs_fd, &rdFds);
            }

            pn = utl_list_next(pn);
        }

        UT_FSELECT_UNLOCK(fsel);

        _INT32 fds = select(fsel->fs_max_fd + 1, &rdFds, NULL, NULL, NULL);

        if (fds > 0 && FD_ISSET(utl_sock_fd(fsel->fs_sock_rx), &rdFds))
        {
            char buf[1024];     /* a pipe message received, just read the data */
            
            utl_sock_recv(fsel->fs_sock_rx, buf, sizeof(buf), NULL);
            --fds;
        }

    #ifdef HAVE_SYS_INOTIFY_H
        if (fds > 0 && FD_ISSET(fsel->fs_inotify_fd, &rdFds))
        {
            /* inotify event is received */
            utFSelectOnINotifyEvent(fsel);
        
            --fds;
        }
    #endif

        if (fds > 0)
        {
            UT_FSELECT_LOCK(fsel);

            pn = utl_list_begin(&fsel->fs_fds);
            while (fds > 0 && pn != utl_list_end(&fsel->fs_fds))
            {
                ST_UT_FSELECT_FILE* file = (ST_UT_FSELECT_FILE*) pn;

                if (file->fs_fd >= 0 && FD_ISSET(file->fs_fd, &rdFds))
                {
                    utFSelectFileOnRead(file);

                    --fds;
                }

                pn = utl_list_next(pn);
            }

            UT_FSELECT_UNLOCK(fsel);
        }
    }

    UT_INFO("file select task is stopped!");
}

_INT32 UT_FSelectInit(_INT32 priority, _INT32 stackSize)
{
    ST_UT_FSELECT* fsel = &g_wrlFSelect;
    
    memset(fsel, 0, sizeof(*fsel));
    
    fsel->fs_priority = priority;
    fsel->fs_stack_size = stackSize;
    fsel->fs_inotify_fd = -1;

    utl_list_init(&fsel->fs_fds);

    fsel->fs_mutex = semMCreate(SEM_EMPTY);
    if (NULL == fsel->fs_mutex)
    {
        UT_WARN(-1, EOS_ERROR, "mutex create failed", "error = %d %s", errno, strerror(errno));
        return EOS_ERROR;
    }

    utl_pipe pipe;

    if (utl_pipe_init(&pipe))
    {
        UT_WARN(-1, EOS_ERROR, "pipe create failed", "error = %d %s", errno, strerror(errno));
        return EOS_ERROR;
    }

    fsel->fs_sock_rx = utl_pipe_pop_rdr(pipe);
    fsel->fs_sock_tx = utl_pipe_pop_wtr(pipe);

    utl_pipe_free(pipe);

#ifdef HAVE_SYS_INOTIFY_H
    fsel->fs_inotify_fd = inotify_init();
    if (-1 == fsel->fs_inotify_fd)
    {
        UT_WARN(-1, EOS_ERROR, "inotify_init failed", "error = %d %s", errno, strerror(errno));
        return EOS_ERROR;
    }
#endif

    fsel->fs_max_fd = utl_max(utl_sock_fd(fsel->fs_sock_rx), fsel->fs_inotify_fd);

    return EOS_OK;
}

_VOID  UT_FSelectFree()
{
    /* DO NOTHING now */
}

_INT32 UT_FSelectAddFd(_INT32 fd, FP_UT_FSELECT_READ fpRead, _VOID* usrData)
{
    ST_UT_FSELECT* fsel = &g_wrlFSelect;
    ST_UT_FSELECT_FILE* file;

    file = utFSelectFileCreate(fpRead, usrData);
    if (NULL == file)
    {
        return EOS_ERROR;
    }

    file->fs_fd = fd;
    file->fs_wd = -1;

    if (fd > fsel->fs_max_fd)
    {
        fsel->fs_max_fd = fd;
    }

    utFSelectAdd(fsel, file);

    return EOS_OK;
}

_INT32 UT_FSelectDelFd(_INT32 fd)
{
    ST_UT_FSELECT* fsel = &g_wrlFSelect;
    ST_UT_FSELECT_FILE* file;
    
    file = utFSelectDel(fsel, (utl_list_cond_f) utFSelectFileFdEqu, &fd);
    if (NULL == file)
    {
        UT_WARN(-1, EOS_ERROR, "file not found", "fd = %d", fd);
        return EOS_ERROR;
    }
    
    utFSelectFileDelete(file);

    return EOS_OK;
}

_INT32 UT_FSelectAddFile(const _CHAR8* fname, FP_UT_FSELECT_READ fpRead, _VOID* usrData)
{
#ifdef HAVE_SYS_INOTIFY_H
    ST_UT_FSELECT* fsel = &g_wrlFSelect;
    ST_UT_FSELECT_FILE* file;

    file = utFSelectFileCreate(fpRead, usrData);
    if (NULL == file)
    {
        return EOS_ERROR;
    }

    file->fs_fd = -1;
    file->fs_wd = inotify_add_watch(fsel->fs_inotify_fd, fname, IN_MODIFY|IN_DELETE_SELF|IN_ALL_EVENTS);
    
    if (-1 == file->fs_wd)
    {
        UT_WARN(-1, EOS_ERROR, "inotify_add_watch failed!",
            "filename = %s, error = %d %s", fname, errno, strerror(errno));

        utFSelectFileDelete(file);
        return EOS_ERROR;
    }

    file->fs_name = utl_strdup(fname);

    utFSelectAdd(fsel, file);

    return EOS_OK;
#else
    return EOS_ERROR;
#endif
}

_INT32 UT_FSelectDelFile(const _CHAR8* fname)
{
#ifdef HAVE_SYS_INOTIFY_H
    ST_UT_FSELECT* fsel = &g_wrlFSelect;
    ST_UT_FSELECT_FILE* file;
    
    file = utFSelectDel(fsel, (utl_list_cond_f) utFSelectFileNameEqu, fname);
    if (NULL == file)
    {
        UT_WARN(-1, EOS_ERROR, "file not found", "filename = %s", fname);
        return EOS_ERROR;
    }
    
    utFSelectFileDelete(file);

    return EOS_OK;
#else
    return EOS_ERROR;
#endif
}

_INT32 UT_FSelectStart(const _CHAR8* name)
{
    ST_UT_FSELECT* fsel = &g_wrlFSelect;

    eos_task_start(name, fsel->fs_priority, fsel->fs_stack_size, utFSelectMain, fsel);

    return EOS_OK;
}

#ifdef __cplusplus
}
#endif

