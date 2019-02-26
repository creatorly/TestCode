/********************************************************
* 
* Copyright (C) 2014-2014, All rights reserved
* Designed by EjoinTech XP Development Team.
* 
* 
* File:         utl_date.h
* PACKAGE:      libeut
*   Header of:          utl_date.c
*   Date:               2014.05.13
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_DATE_H__
#define  __UTL_DATE_H__

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlDate
{
    short year;
    char mon;
    char day;

    char hour;
    char min;
    char sec;
    char zone;
}utl_date_t;

#define UTL_DATE_EQU3(x1, y1, z1, x2, y2, z2) \
            (((x1) == (x2)) && ((y1) == (y2)) && ((z1) == (z2)))

#define UTL_DATE_LESS3(x1, y1, z1, x2, y2, z2) \
            (((x1) < (x2)) \
                || (((x1) == (x2)) && ((y1) < (y2))) \
                || (((x1) == (x2)) && ((y1) == (y2)) && ((z1) < (z2))))

#define UTL_DATE_DATE_EQU(d1, d2) \
            UTL_DATE_EQU3((d1)->year, (d1)->mon, (d1)->day, (d2)->year, (d2)->mon, (d2)->day)

#define UTL_DATE_DATE_LESS(d1, d2) \
            UTL_DATE_LESS3((d1)->year, (d1)->mon, (d1)->day, (d2)->year, (d2)->mon, (d2)->day)

#define UTL_DATE_TIME_EQU(d1, d2) \
            UTL_DATE_EQU3((d1)->hour, (d1)->min, (d1)->sec, (d2)->hour, (d2)->min, (d2)->sec)

#define UTL_DATE_TIME_LESS(d1, d2) \
            UTL_DATE_LESS3((d1)->hour, (d1)->min, (d1)->sec, (d2)->hour, (d2)->min, (d2)->sec)

#define UTL_DATE_EQU(d1, d2) \
            (UTL_DATE_DATE_EQU(d1, d2) && UTL_DATE_TIME_EQU(d1, d2))

#define UTL_DATE_LESS(d1, d2) \
            (UTL_DATE_DATE_LESS(d1, d2) \
                || (UTL_DATE_DATE_EQU(d1, d2) && UTL_DATE_TIME_LESS(d1, d2)))

bool utl_is_leapyear(int year);

int utl_get_date(int sec, utl_date_t* date);
int utl_mktime(utl_date_t* date);

int utl_get_days(int year, int month, int day);
int utl_get_last_day(int year, int month);

int utl_get_weekday(int year, int month, int day);
int utl_get_week(int year, int month, int day);
int utl_get_week_sunday(int year, int month, int day);

int utl_get_week_1970(int sec, int sunday_is_first);
int utl_get_weekday_1970(int sec, int sunday_is_first);

#ifdef __cplusplus
}
#endif

#endif

