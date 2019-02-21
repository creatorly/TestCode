
#include <eos.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utils.h>
#include <ut_mem_pool.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_MEM_POOL_DEBUG
#undef UT_MEM_POOL_COPY_FILENAME

#define UT_MEM_POOL_FILENAME_LEN    32
#define UT_MEM_POOL_LINK_SIZE       10

typedef enum
{
    UT_MP_FLAG_USED = (1 << 0),
    UT_MP_FLAG_DEBUG = (1 << 1),
}EN_UT_MP_FLAG;

typedef struct tagstUtMemNode
{
    struct tagstUtMemNode *    nextnode;
    struct tagstUtMemNode *    prevnode;

#ifdef UT_MEM_POOL_DEBUG
    int flags;
    unsigned short line;
    unsigned short line2;
#ifdef UT_MEM_POOL_COPY_FILENAME
    char file[UT_MEM_POOL_FILENAME_LEN];
    char file2[UT_MEM_POOL_FILENAME_LEN];
#else
    const char* file;
    const char* file2;
#endif    
    time_t tmAlloc;
#endif
}ST_UT_MNOD,*PST_UT_MNOD;

typedef struct tagstUtMemLink
{
    PST_UT_MNOD    full;
    PST_UT_MNOD    empty;
    PST_UT_MNOD    empty_tail;
    int        usage;
    int        used;
    int        free;
    int        nomem;
    int        maxused;
    unsigned int        startaddr;
    unsigned int        stopaddr;
    /* v.106    */
    int        total;
    struct tagstUtMemLink*    nextlink;
    unsigned int        nodesize;
    unsigned int        memsize;
}ST_UT_MLNK,*PST_UT_MLNK;

typedef struct tagstUtMemPool
{
    int bInitialized;
    int bDebugOn;

    SEM_ID mSemID;

    int         link_nums;
    ST_UT_MLNK  links[UT_MEM_POOL_LINK_SIZE];
}ST_UT_MP;

#define UT_MP_LOCK(pmp) \
do { \
    if ((pmp)->mSemID != NULL) \
        semTake((pmp)->mSemID, WAIT_FOREVER); \
} while(0)
 
#define UT_MP_UNLOCK(pmp) \
do { \
    if ((pmp)->mSemID != NULL) \
        semGive((pmp)->mSemID); \
} while(0)

#define UT_MP_FLAG_SET(obj, flgs)   ((obj)->flags |= flgs)
#define UT_MP_FLAG_CLR(obj, flgs)   ((obj)->flags &= ~flgs)
#define UT_MP_FLAG_ISSET(obj, flgs) (((obj)->flags & flgs) == flgs)

int utMemPoolNodeLinkInit(PST_UT_MLNK pl, int size, int number);
void* utMemPoolAlloc(PST_UT_MP pmp, int size, const char* file, int line);
void utMemPoolFree(PST_UT_MLNK pl, unsigned addr, const char* file, int line);
int utMemPoolToStdout(void* out, const char* str);

PST_UT_MP ut_mp_create(int bUseLock)
{
    PST_UT_MP pmp;
    
    pmp = (PST_UT_MP)malloc(sizeof(ST_UT_MP));
    if(pmp == NULL)
    {
        printf("ut_mem_init error\r\n");
        return NULL;    
    }

    memset(pmp, 0, sizeof(ST_UT_MP));

    if (bUseLock)
    {
        pmp->mSemID = semMCreate(SEM_Q_PRIORITY);
        if(NULL == pmp->mSemID)
        {
            free(pmp);
            
            printf("ut_mem_init error(no sem)\r\n");
            return NULL;
        }
    }

    pmp->bInitialized = EOS_TRUE;

    return pmp;
}

void ut_mp_destroy(PST_UT_MP pmp) {}

int ut_mp_add(PST_UT_MP pmp, ut_mem_size_t size, int cnt)
{
    PST_UT_MLNK pLnk;

    UT_MP_LOCK(pmp);
    
    pLnk = &pmp->links[pmp->link_nums];

    if (utMemPoolNodeLinkInit(pLnk, size, cnt) != EOS_OK)
    {
        UT_MP_UNLOCK(pmp);

        printf("initmemnodlin error(size=%d,number=%d)\r\n",size, cnt);
        return EOS_ERROR;
    }

    if (pmp->link_nums > 0)
    {
        pmp->links[pmp->link_nums - 1].nextlink = pLnk;
    }
            
    pmp->link_nums++;

    UT_MP_UNLOCK(pmp);

    return EOS_OK;
}

void* ut_mp_alloc(PST_UT_MP pmp, int size, const char* file, int line)
{
    void* p;

    UT_MP_LOCK(pmp);
    
    p = utMemPoolAlloc(pmp, size, file, line);
    
    UT_MP_UNLOCK(pmp);

    return p;
}

void ut_mp_free(PST_UT_MP pmp, void* ptr, const char* file, int line)
{
    PST_UT_MLNK pl = &pmp->links[0];
    unsigned addr = (unsigned)ptr;
    
    if (ptr == NULL)
    {
        return;
    }

    UT_MP_LOCK(pmp);
    
    while (pl)
    {
        if((addr>=pl->startaddr)&&(addr<pl->stopaddr))
            break;
        else
            pl=pl->nextlink;                
    }
    
    if (pl==NULL)
    {
        printf("%s:%d free invalid memory 0x%x!\r\n", file, line, addr);
        UT_MP_UNLOCK(pmp);
        return;
    }

    utMemPoolFree(pl, addr, file, line);
    
    UT_MP_UNLOCK(pmp);
    
    return;
}

void* ut_mp_realloc(PST_UT_MP pmp, void* ptr, int size, const char* file, int line)
{
    PST_UT_MLNK pl = &pmp->links[0];
    unsigned addr = (unsigned)ptr;
    char * p;
    
    if (ptr == NULL)           /* allocate a new */
    {
        return ut_mp_alloc(pmp, size, file, line);
    }

    UT_MP_LOCK(pmp);
    
    while(pl)
    {
        if((addr>=pl->startaddr)&&(addr<pl->stopaddr))
            break;
        else
            pl=pl->nextlink;                
    }
    
    if (pl == NULL)             /* allocate a new */
    {
        p = utMemPoolAlloc(pmp, size, file, line);
        UT_MP_UNLOCK(pmp);
        return p;
    }

    if (pl->memsize >= size)     /* no need to realloc */
    {
        UT_MP_UNLOCK(pmp);
        return ptr;
    }

    /* allocate a new memory */
    p = utMemPoolAlloc(pmp, size, file, line);
    if (p == NULL)
    {
        UT_MP_UNLOCK(pmp);
        printf("ut_mem_realloc error\r\n");
        return NULL;
    }

    memcpy(p, ptr, pl->memsize);
        
    utMemPoolFree(pl, addr, file, line);
    
    UT_MP_UNLOCK(pmp);
    
    return p;
}

void ut_mp_debug(PST_UT_MP pmp, int enable)
{
    UT_MP_LOCK(pmp);
    pmp->bDebugOn = (enable > 0);
    UT_MP_UNLOCK(pmp);
}

int ut_mp_show(PST_UT_MP pmp, ut_mem_print_fp print, void* out)
{
    PST_UT_MLNK pl = &pmp->links[0];
    PST_UT_MNOD pn;
    char acBuf[1024];

    if (NULL == print)
    {
        print = utMemPoolToStdout;
    }

    print(out, "utils memory management information:\r\n");

    sprintf(acBuf, "%5s|%8s|%8s|%8s|%10s|%12s|%8s|%10s|%10s|\r\n",
        "Size", "Total", "Used", "Free",
        "Usage", "Max(%)", "Full", "Start(0x)", "End(0x)");
      print(out, acBuf);
    
    while(pl)
    {
        /* 2006-02-20 Updated by yangfan */
        char acMaxUsed[32];

        sprintf(acMaxUsed, "%d(%2d)", pl->maxused, (pl->maxused*100/pl->total));
        sprintf(acBuf, "%5d|%8d|%8d|%8d|%10d|%12s|%8d|%10x|%10x|\r\n",
            pl->memsize, pl->total, pl->used, pl->free, 
            pl->usage, acMaxUsed, pl->nomem, pl->startaddr, pl->stopaddr);

        print(out, acBuf);

    #ifdef UT_MEM_POOL_DEBUG
        UT_MP_LOCK(pmp);
    
        pn = pl->full;
        while (pn)
        {
            if (UT_MP_FLAG_ISSET(pn, UT_MP_FLAG_DEBUG))
            {
                sprintf(acBuf, "%s addr(%p %s:%d) not already freed!\r\n",
                    UT_FmtTime(pn->tmAlloc, NULL, NULL),
                    pn + 1, pn->file, pn->line);
                   print(out, acBuf);
            }
            
            pn=pn->nextnode;
        }
        
        UT_MP_UNLOCK(pmp);
    #endif
    
        pl=pl->nextlink;
    }

    return 0;
}

int utMemPoolNodeLinkInit(PST_UT_MLNK pl, int size, int number)
{
    PST_UT_MNOD pn;
    PST_UT_MNOD next;
    int i;
    int node_size = size;

    if(number <= 0)
    {
        pl->empty = NULL;
        return EOS_ERROR;
    }

#ifdef UT_MEM_POOL_DEBUG
    node_size += sizeof(ST_UT_MNOD);
#else
    if (size < sizeof(ST_UT_MNOD))
    {
        node_size = sizeof(ST_UT_MNOD);
    }
#endif

    pl->empty = (PST_UT_MNOD)malloc(node_size * number);
    if(pl->empty == NULL)
    {
        printf("malloc 1 error(size=%d,number=%d)\r\n",size,number);
        return EOS_ERROR;
    }

    memset(pl->empty, 0, (node_size * number));
    
    pl->nodesize = node_size;
    pl->memsize = size;
    pl->startaddr = (unsigned int)pl->empty;
    pl->stopaddr = (unsigned int)(pl->startaddr + node_size*number);

    pn = pl->empty;
        
    for(i=1; i<number; i++)
    {
        next = (PST_UT_MNOD)((unsigned int)pn + node_size);
        
        pn->nextnode = next;
        pn = next;
    }

    pn->nextnode = NULL;
    pl->empty_tail = pn;

    pl->nextlink = NULL;
    pl->full = NULL;
    pl->free = number;
    pl->total = number;

    return EOS_OK;
}

void* utMemPoolAlloc(PST_UT_MP pmp, int size, const char* file, int line)
{
    PST_UT_MLNK pl = &pmp->links[0];
    PST_UT_MNOD pn;
        
    while (pl != NULL)
    {
        if (pl->memsize >= size)
        {
            if (pl->empty)
            {
                break;
            }
            
            pl->nomem++;

            /* do not allocate from the larger memory link */
            return NULL;
        }

        pl = pl->nextlink;
    }
    
    if (pl==NULL)
    {
        /* printf("%s:%d no memory %d\r\n", file, line, size); */
        return NULL;
    }
    
    pn = pl->empty;
    pl->empty = pn->nextnode;

#ifdef UT_MEM_POOL_DEBUG    
    if(pl->full == NULL)
    {
        pl->full = pn;
        pn->nextnode = NULL;
        pn->prevnode = NULL;
    }
    else
    {
        pn->prevnode = NULL;
        pn->nextnode = pl->full;
        pl->full->prevnode = pn;
        pl->full = pn;
    }

    pn->tmAlloc = time(NULL);
    
#ifdef UT_MEM_POOL_COPY_FILENAME
    UT_BUF_STRCPY(pn->file, file);
    pn->file2[0] = '\0';
#else    
    pn->file = file;
    pn->file2 = "";
#endif    
    pn->line = line;
    pn->line2 = 0;
    
    UT_MP_FLAG_SET(pn, UT_MP_FLAG_USED);
    
    if (pmp->bDebugOn) {
        UT_MP_FLAG_SET(pn, UT_MP_FLAG_DEBUG);
    } else {
        UT_MP_FLAG_CLR(pn, UT_MP_FLAG_DEBUG);
    }
#endif

    pl->used++;
    if (pl->used > pl->maxused)
    {
        pl->maxused = pl->used;
    }
    
    pl->free--;
    pl->usage++;
    
#ifdef UT_MEM_POOL_DEBUG    
    return pn + 1;
#else
    return pn;
#endif
}

void utMemPoolFree(PST_UT_MLNK pl, unsigned addr, const char* file, int line)
{
    PST_UT_MNOD pn;

    pn = (PST_UT_MNOD)(pl->startaddr + (addr - pl->startaddr)/pl->nodesize*pl->nodesize);

#ifdef UT_MEM_POOL_DEBUG    
    if (!UT_MP_FLAG_ISSET(pn, UT_MP_FLAG_USED))
    {
        printf("%s:%d free memory 0x%x(%s:%d->%s:%d) repeatedly!\r\n", 
            file, line, addr, pn->file, pn->line, pn->file2, pn->line2);
        return;
    }

    if (pn->prevnode == NULL)
    {
        pl->full = pn->nextnode;
        
        if (pn->nextnode != NULL)
        {
            pl->full->prevnode = NULL;
        }
    }
    else if (pn->nextnode != NULL)
    {
        pn->nextnode->prevnode = pn->prevnode;
        pn->prevnode->nextnode = pn->nextnode;
    }
    else
    {
        pn->prevnode->nextnode=NULL;
    }

#ifdef UT_MEM_POOL_COPY_FILENAME
    UT_BUF_STRCPY(pn->file2, file);
#else    
    pn->file2 = file;
#endif    
    pn->line2 = line;

    UT_MP_FLAG_CLR(pn, UT_MP_FLAG_USED);
    UT_MP_FLAG_CLR(pn, UT_MP_FLAG_DEBUG);
#endif

    if (pl->empty != NULL)
    {
        pl->empty_tail->nextnode = pn;
    }
    else
    {
        pl->empty = pn;
    }
    
    pn->nextnode = NULL;
    pl->empty_tail = pn;
    
    pl->used--;
    pl->free++;
}

int utMemPoolToStdout(void* out, const char* str)
{
    int len = 0;

    len = printf("%s", str);
    
    return len;
}

#if defined(__cplusplus)
}
#endif

