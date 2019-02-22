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

