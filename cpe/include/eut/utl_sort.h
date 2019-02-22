/********************************************************
* 
* Copyright (C) 2015-2015, All rights reserved
* Designed by EjoinTech XP Development Team.
* 
* 
* File:         utl_sort.h
* PACKAGE:      libeut
*   Header of:          utl_sort.c
*   Date:               2015.09.06
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_SORT_H__
#define  __UTL_SORT_H__

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * the COMPARE function
 * return: -1 when obj1 < obj2, 1 when obj1 > obj2, 0 when obj1 == obj2
 */
typedef int (*FP_UTL_CMP)(void* obj1, void* obj2);

/***************************************************************************
 * the SWAP function for the object
 */
typedef int (*FP_UTL_SWAP)(void* obj1, void* obj2);

/***************************************************************************
 * SELECT SORT ascending, if you want descending sort, 
 * please change the "cmp" implement(return 1 when obj1 is less than obj2)
 *
 * @param   array:  the array need to sort
 * @param   size:   the size of array member
 * @param   n:      the count of array member
 * @param   cmp:    the compare function
 * @param   swap:   the swap function, if null the default swap function wiil be used
 */
void utl_select_sort(void* array, size_t size, int n, FP_UTL_CMP cmp, FP_UTL_SWAP swap);

#ifdef __cplusplus
}
#endif

#endif

