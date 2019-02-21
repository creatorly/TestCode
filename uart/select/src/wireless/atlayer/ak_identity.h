/**
 * @file ak_identity.h
 * @brief This header file defined AK Chip identity macros
 * @note This file shared in VSS, notify other LIB maintenance man before update it
 * 
 */
#ifndef __AK_IDENTITY_H__
#define __AK_IDENTITY_H__

/************************AK Chip Indentity*************************
@Macros:
    @IS_AK_CHIP:    judge if the chip is ANYKA chip
    @AK_CHIPID:        AK Chip ID register addr
*******************************************************************/
#ifdef OS_WIN32
    #define IS_AK_CHIP() (1)
#else
    #define AK322L_CHIPID_ADDR 0x20000010
    #define IS_AK322L (*((volatile int *)AK322L_CHIPID_ADDR)==0x414B3224)

    #define AK7801_CHIPID_ADDR 0x08000000
    #define IS_AK7801 (*((volatile int *)AK7801_CHIPID_ADDR)==0x33323236)

    #define IS_AK_CHIP() (IS_AK322L || IS_AK7801)    
#endif

#endif
