/********************************************************
* 
*
* Cosmos-II SIP Proxy Lib
* Copyright (C) 2013-2013, All rights reserved
* Designed by Cosmos-II Development Team.
* 
* 
* File:         ut_numplan.c
* PACKAGE:      cosSpxLib
*   Implement of:       ut number translation
*   Date:               2013.03.05
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utl_typdef.h>
#include <utl_list.h>
#include <ut_mem.h>
#include <utils.h>

#include "ut_num_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UT_NUMPLAN_WILDCHAR    '*'

#define UT_DBG2(idx, msg, fmt, args...)

typedef struct tagstSpxNumPlanItem
{
    utl_node_t node;

    char srcnum[32];
    char srcdom[64];
    char dstnum[32];
    char dstdom[64];

    int nStripped;
    char numAdded[16];

    int nAdded;
}ST_UT_NUMPLAN_ITEM;

typedef struct tagstSpxNumPlan
{
    bool bTransCaller;

    utl_list_t lsNumPlans;
}ST_UT_NUMPLAN;

int utNumPlanItemCmp(ST_UT_NUMPLAN_ITEM* pItem1, ST_UT_NUMPLAN_ITEM* pItem2);
bool utNumPlanItemMatch(ST_UT_NUMPLAN_ITEM* pItem, 
                         const char* srcnum,
                         const char* srcdom,
                         const char* dstnum,
                         const char* dstdom);
void utNumPlanItemDestroy(ST_UT_NUMPLAN_ITEM* pItem);

int utNumPlanStrWildCmp(const char* pcPtn1, const char* pcPtn2);
bool utNumPlanStrWildMatch(const char* pcStr, const char* pcPtn);

PST_UT_NUMPLAN UT_NumPlanCreate(bool translate_caller)
{
    PST_UT_NUMPLAN pnp;

    pnp = UT_TALLOC(ST_UT_NUMPLAN);
    if (NULL == pnp)
    {
        printf("failed to allocate ST_UT_NUMPLAN!\r\n");
        return NULL;
    }

    memset(pnp, 0, sizeof(ST_UT_NUMPLAN));

    pnp->bTransCaller = translate_caller;

    utl_list_init(&pnp->lsNumPlans);

    return pnp;
}

void UT_NumPlanDestroy(PST_UT_NUMPLAN pnp)
{
    if (pnp != NULL)
    {
        UTL_LIST_FREE(&pnp->lsNumPlans, utNumPlanItemDestroy);
        UT_FREE(pnp);
    }
}

int UT_NumPlanAdd(PST_UT_NUMPLAN pnp, 
                   const char* srcnum,
                   const char* srcdom,
                   const char* dstnum,
                   const char* dstdom,
                   int nNumStripped, 
                   const char* numAdded)
{
    ST_UT_NUMPLAN_ITEM* pItem;
    utl_node_t* pn = utl_list_begin(&pnp->lsNumPlans);

    pItem = UT_TALLOC(ST_UT_NUMPLAN_ITEM);
    if (NULL == pnp)
    {
        printf("failed to allocate ST_UT_NUMPLAN_ITEM!\r\n");
        return -1;
    }

    memset(pItem, 0, sizeof(ST_UT_NUMPLAN_ITEM));

    UT_BUF_STRCPY(pItem->srcnum, srcnum);
    UT_BUF_STRCPY(pItem->srcdom, srcdom);
    UT_BUF_STRCPY(pItem->dstnum, dstnum);
    UT_BUF_STRCPY(pItem->dstdom, dstdom);

    pItem->nStripped = nNumStripped;
    UT_BUF_STRCPY(pItem->numAdded, numAdded);

    pItem->nAdded = strlen(pItem->numAdded);

    /* find the first node that is litter than the item to be inserted */
    while (pn != utl_list_end(&pnp->lsNumPlans))
    {
        ST_UT_NUMPLAN_ITEM* pnItem = (ST_UT_NUMPLAN_ITEM*)pn;

        if (utNumPlanItemCmp(pnItem, pItem) < 0)
        {
            break;
        }

        pn = utl_list_next(pn);
    }

    /* insert the item before the found node */
    utl_list_ins(&pnp->lsNumPlans, utl_list_prev(pn), &pItem->node);

    return UTL_OK;
}

void UT_NumPlanClear(PST_UT_NUMPLAN pnp)
{
    UTL_LIST_FREE(&pnp->lsNumPlans, utNumPlanItemDestroy);

    utl_list_init(&pnp->lsNumPlans);
}

int UT_NumPlanSize(PST_UT_NUMPLAN pnp)
{
    return utl_list_size(&pnp->lsNumPlans);
}

int UT_NumPlanDump(PST_UT_NUMPLAN pnp, char* buf, int size)
{
    utl_node_t* pn = utl_list_begin(&pnp->lsNumPlans);
    int len = 0;
    int ret = 0;

    buf[0] = '\0';

    while (pn != utl_list_end(&pnp->lsNumPlans))
    {
        ST_UT_NUMPLAN_ITEM* pItem = (ST_UT_NUMPLAN_ITEM*)pn;

        ret = snprintf(buf+len, size-len, "%16s %16s %16s %16s %8d %8s\r\n",
            pItem->srcnum, pItem->srcdom, pItem->dstnum, pItem->dstdom,
            pItem->nStripped, pItem->numAdded);

        if (ret < size - len)
        {
            len += ret;
        }
        else
        {
            len = size - 1;
            break;
        }

        pn = utl_list_next(pn);
    }

    return len;
}

int UT_NumPlanMatch(PST_UT_NUMPLAN pnp, 
                     const char* srcnum,
                     const char* srcdom,
                     const char* dstnum,
                     const char* dstdom,
                     char* newnum, int size)
{
    utl_node_t* pn = utl_list_begin(&pnp->lsNumPlans);

    while (pn != utl_list_end(&pnp->lsNumPlans))
    {
        ST_UT_NUMPLAN_ITEM* pItem = (ST_UT_NUMPLAN_ITEM*)pn;

        if (utNumPlanItemMatch(pItem, srcnum, srcdom, dstnum, dstdom))
        {
            const char* orignum = pnp->bTransCaller ? srcnum : dstnum;
            int len = 0;

            if (orignum != NULL)
            {
                int origlen = strlen(orignum);

                if (pItem->nAdded > 0)
                {
                    len = utl_min(pItem->nAdded, size - 1);
                    memcpy(newnum, pItem->numAdded, len);
                }

                if ((pItem->nStripped < origlen) && (len < size - 1))
                {
                    orignum += pItem->nStripped;
                    origlen -= pItem->nStripped;

                    if (origlen >= size - len)
                    {
                        origlen = size - len - 1;
                    }
                    
                    memcpy(&newnum[len], orignum, origlen);
                    len += origlen;
                }
            }
            
            newnum[len] = '\0';

            UT_DBG2(-1, "number plan matched successfully",
                "src = %s@%s, dst = %s@%s, newnum = %s", 
                srcnum, srcdom, dstnum, dstdom, newnum);
            
            return len;
        }

        pn = utl_list_next(pn);
    }

    UT_DBG2(-1, "number plan failed to match",
        "src = %s@%s, dst = %s@%s", 
        srcnum, srcdom, dstnum, dstdom);

    return -1;
}

int utNumPlanItemCmp(ST_UT_NUMPLAN_ITEM* pItem1, ST_UT_NUMPLAN_ITEM* pItem2)
{
    int res;

    res = utNumPlanStrWildCmp(pItem1->srcnum, pItem2->srcnum);
    if (res != 0)
    {
        return res;
    }

    res = utNumPlanStrWildCmp(pItem1->srcdom, pItem2->srcdom);
    if (res != 0)
    {
        return res;
    }

    res = utNumPlanStrWildCmp(pItem1->dstnum, pItem2->dstnum);
    if (res != 0)
    {
        return res;
    }
    
    return utNumPlanStrWildCmp(pItem1->dstdom, pItem2->dstdom);
}

bool utNumPlanItemMatch(ST_UT_NUMPLAN_ITEM* pItem, 
                         const char* srcnum,
                         const char* srcdom,
                         const char* dstnum,
                         const char* dstdom)
{
    if (!utNumPlanStrWildMatch(srcnum, pItem->srcnum))
    {
        return false;
    }

    if (!utNumPlanStrWildMatch(srcdom, pItem->srcdom))
    {
        return false;
    }
    
    if (!utNumPlanStrWildMatch(dstnum, pItem->dstnum))
    {
        return false;
    }
    
    return utNumPlanStrWildMatch(dstdom, pItem->dstdom);
}

void utNumPlanItemDestroy(ST_UT_NUMPLAN_ITEM* pItem)
{
    if (pItem != NULL)
    {
        UT_FREE(pItem);
    }
}

int utNumPlanStrWildCmp(const char* pcPtn1, const char* pcPtn2)
{
    while (*pcPtn1 && *pcPtn2 && (*pcPtn1 == *pcPtn2))
    {
        pcPtn1++;
        pcPtn2++;
    }

    if (UT_NUMPLAN_WILDCHAR == *pcPtn1)
    {
        return -1;
    }
    else if (UT_NUMPLAN_WILDCHAR == *pcPtn2)
    {
        return 1;
    }
    else
    {
        return (*pcPtn1 - *pcPtn2);
    }
}

bool utNumPlanStrWildMatch(const char* pcStr, const char* pcPtn)
{
    const char* pcHdr = NULL;
    const char* pcPtHdr = NULL;

    /* the pcPtn should not be NULL */

    if (pcStr != NULL)
    {
        while (*pcStr)
        {
            /* only one continuous '*', others deleted before */
            if (UT_NUMPLAN_WILDCHAR == *pcPtn)
            {
                while (UT_NUMPLAN_WILDCHAR == *(++pcPtn));

                if ('\0' == *pcPtn)
                {
                    return true;
                }
                
                pcPtHdr = pcPtn;
                
                /* find the first character not eqaul to '*' */
                pcStr = strchr(pcStr, *pcPtHdr);
                if (NULL == pcStr)
                {
                    return false;
                }
                
                pcHdr = pcStr;
            }
            else if (*pcStr != *pcPtn)
            {
                if (NULL == pcHdr)
                {
                    /* no wild character */
                    return false;
                }
                
                /* rematch the wild character */
                pcStr = strchr(pcHdr + 1, *pcPtHdr);
                if (NULL == pcStr)
                {
                    return false;
                }
                
                pcHdr = pcStr;
                pcPtn = pcPtHdr;
            }
            
            ++pcStr;
            ++pcPtn;
        }
    }

    while (UT_NUMPLAN_WILDCHAR == *pcPtn) ++pcPtn;
    
    return ('\0' == *pcPtn);
}

#ifdef __cplusplus
}
#endif

