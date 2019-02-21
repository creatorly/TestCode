/**
 * @file dependence.h
 * @brief Define AT Server interface
 *
 * Copyright (C) 2007 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author Junhua.Zhao
 * @date 2007-11-27
 * @version 1.0
*/
#ifndef __AT_DEPENDENCE__H__
#define __AT_DEPENDENCE__H__


typedef _INT32                T_hTask;
typedef _INT32                T_hQueue;
typedef _INT32                T_hSemaphore;
typedef _INT32                T_hHisr;
typedef _UCHAR8                T_OPTION;
typedef _INT32                T_hMailbox;
typedef _INT32                T_hEventGroup;
typedef _INT32                T_hTimer;

/** Define constants for use in service parameters.  */

#define         AK_AND                          2
#define         AK_AND_CONSUME                  3
#define         AK_DISABLE_TIMER                4
#define         AK_ENABLE_TIMER                 5
#define         AK_FIFO                         6
#define         AK_FIXED_SIZE                   7
#define         AK_NO_PREEMPT                   8
#define         AK_NO_START                     9
#define         AK_NO_SUSPEND                   0
#define         AK_OR                           0
#define         AK_OR_CONSUME                   1
#define         AK_PREEMPT                      10
#define         AK_PRIORITY                     11
#define         AK_START                        12
#define         AK_SUSPEND                      0xFFFFFFFFUL
#define         AK_VARIABLE_SIZE                13

/** Define task suspension constants.  */

#define         AK_DRIVER_SUSPEND               10
#define         AK_EVENT_SUSPEND                7
#define         AK_FINISHED                     11
#define         AK_MAILBOX_SUSPEND              3
#define         AK_MEMORY_SUSPEND               9
#define         AK_PARTITION_SUSPEND            8
#define         AK_PIPE_SUSPEND                 5
#define         AK_PURE_SUSPEND                 1
#define         AK_QUEUE_SUSPEND                4
#define         AK_READY                        0
#define         AK_SEMAPHORE_SUSPEND            6
#define         AK_SLEEP_SUSPEND                2
#define         AK_TERMINATED                   12

/** Define task status. */
#define            AK_TASK_READY                    1
#define         AK_TASK_SUSPEND                 2
#define         AK_TASK_WAITING                 3
#define            AK_TASK_TERMINATED                4
#define            AK_TASK_FINISHED                5

/** memory manager*/
_VOID*    mallocAndTrace(_UINT32 size, T_S8* file, _UINT32 line);
_VOID*    freeAndTrace(_VOID* var, T_S8* file, _UINT32 line);
#define free(var)            freeAndTrace(var, ((T_S8*)(__FILE__)), ((_UINT32)__LINE__))
/**
_VOID* WIRELESS_MALLOC(_UINT32 size);
_VOID* free(_VOID* var);
*/



/**
@defgroup btypes Basic Data Types
@ingroup scheduler

These are the basic VME data types which are used by all VME functions.
It is strongly recommended that these data types be used in every VME based userware
application.
*/
/*@{ */

/**semaphore manager*/
T_hSemaphore AK_Create_Semaphore(_UINT32 initial_count, T_OPTION suspend_type);
_INT32 AK_Obtain_Semaphore(T_hSemaphore semaphore, _UINT32 suspend);
_INT32 AK_Release_Semaphore(T_hSemaphore semaphore);
_INT32 AK_Delete_Semaphore(T_hSemaphore semaphore);

/**string operation*/
_CHAR8*    Utl_StrCat(_CHAR8* strDest, const _CHAR8* strSrc);
_BOOL     Utl_StrIsHex(const _CHAR8* strMain);
_CHAR8*    Utl_StrCatChr(_CHAR8* strDest, T_S8 chr, _USHORT16 count);
T_S8    Utl_StrCmp(const _CHAR8* str1, const _CHAR8* str2);
_CHAR8*    strcpyN(_CHAR8* strDest, const _CHAR8* strSour, _USHORT16 length);
_CHAR8*    Utl_StrCatN(_CHAR8* strDest, const _CHAR8* strSrc, _USHORT16 length);
_SHORT16    Utl_StrFndLC(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 length);
_SHORT16    Utl_StrFndL(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 length);
_SHORT16   Utl_StrFndNL(const _CHAR8* strMain, const _CHAR8* strSub, _SHORT16 offset, _USHORT16 count, _USHORT16 length);
//_CHAR8*    Utl_StrMid(_CHAR8* strDest, const _CHAR8* strMain, _SHORT16 offset, _SHORT16 end);
_CHAR8*  Utl_StrMidL(_CHAR8* strDest, const _CHAR8* strSour, _SHORT16 offset, _SHORT16 end, _USHORT16 strlength);
_BOOL  Utl_StrDigital(const _CHAR8* strMain);
_SHORT16    Utl_StrFndChr(const _CHAR8* strMain, T_S8 chr, _SHORT16 offset);
_BOOL    Utl_StrIsEmpty(const _CHAR8* strMain);
_INT32    Utl_Atoi(const _CHAR8* strMain);
_INT32    AToI16(const _CHAR8* string);
_CHAR8*    Utl_Itoa(_INT32 intNum, _CHAR8* strDest, _UCHAR8 flag);


/**time operation*/
_VOID Time_AddHours(RIL_SYSTIME *dtTime, _USHORT16 hours);


/**data convert*/
_VOID  ConvUCS2ToSTK(const _CHAR8* strUCS2, _CHAR8* strSTK);  
_SHORT16   Conv8BitDataToGSM(const _CHAR8* str8BitData, _CHAR8* strGSM);
_VOID     DecodeWavecomText(const _CHAR8* strInput, _CHAR8* strOutput);
//_BOOL     ConvUCS2StrToUnicode(const _CHAR8* strUCS2, T_pTSTR strUnicode);
//_SHORT16     Conv8BitDataToUnicode(_CHAR8* str8BitData, T_pTSTR strUnicode);
//_VOID    ConvertTimeS2C(RIL_SYSTIME *SysTime, _CHAR8* strDate, _CHAR8* strTime); 

/**queue manager*/
T_hQueue AK_Create_Queue(_VOID *start_address, _UINT32 queue_size, 
                         T_OPTION message_type, _UINT32 message_size, 
                         T_OPTION suspend_type);
_INT32 AK_Delete_Queue(T_hQueue queue);
_INT32 AK_Receive_From_Queue(T_hQueue queue, _VOID *message, _UINT32 size,
                            _UINT32 *actual_size, _UINT32 suspend);
_INT32 AK_Send_To_Queue(T_hQueue queue, _VOID *message,
                              _UINT32 size, _UINT32 suspend);
_INT32 AK_Send_To_Front_of_Queue(T_hQueue queue, _VOID *message, _UINT32 size, _UINT32 suspend);

_INT32 AK_Queue_Information(T_hQueue queue_ptr, _VOID **start_address, _UINT32 *queue_size, 
    _UINT32 *available, _UINT32 *messages, T_OPTION *message_type, _UINT32 *message_size, 
    T_OPTION *suspend_type, _UINT32 *tasks_waiting, T_hTask *first_task);

/**task manager*/                      
T_hTask AK_Create_Task(_VOID *task_entry,_UCHAR8 *name,_UINT32 argc, _VOID *argv,
                        _VOID *stack_address, _UINT32 stack_size, 
                        T_OPTION priority, _UINT32 time_slice, 
                        T_OPTION preempt, T_OPTION auto_start);
_INT32 AK_Terminate_Task(T_hTask task);
_INT32 AK_Resume_Task(T_hTask task);    
_INT32 AK_Delete_Task(T_hTask task);


_VOID AK_Sleep(_UINT32 ticks);

_SHORT16 AK_vatcReadALine(_SHORT16 ChanId, T_pDATA data, _SHORT16 count);

_SHORT16 AK_vatcWrite(_SHORT16 ChanId, const _UCHAR8* data, _SHORT16 count);

/**
*@brief 给模块点火
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 IFX_IgniteModule(_VOID);
/**
*@brief 查询模块点火结果
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL IFX_IgniteResult(_VOID);
/**
*@brief 硬件方式给模块断电，该接口一般在AT关机无效时使用
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 IFX_PowerOffModule(_VOID);
/**
*@brief 查询硬件方式给模块断电的结果
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL IFX_PowerOffResult(_VOID);
/**
*@brief 重启模块
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 IFX_ResetModule(_VOID);
/**
*@brief 查询模块重启的结果
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL IFX_ResetResult(_VOID);
/**
*@brief 让模块睡眠
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_VOID IFX_SleepModule(_VOID);
/**
*@brief 唤醒模块
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_VOID IFX_WakeupModule(_VOID);

/*@}*/
/**
 *@brief Convert the UNICODE name to UCS2
 *
 *@param  T_pCTSTR strText    source string
 *@param  _CHAR8*   strUCS2   destiminate string
 *@return the length of the UCS2 string
 **/
_SHORT16 ConvUnicodeToUCS2Str(_SHORT16 *strText, _CHAR8* strUCS2);

/**
 *@brief Convert Unicode string to GSM string
 *
 *@param T_pTSTR strUnicode: source string
 *@param _CHAR8* strGSM: destiminate string
 *@return _BOOL
 */
_BOOL ConvUnicodeToGSM(_SHORT16 *strUnicode, _CHAR8* strGSM);

/**
  * @brief Convert default GSM alphabet to UCS2 string
  *
  * @param[in]  const _CHAR8* strGSM    source string
  * @param[out] _CHAR8*   strUCS2   destiminate string
  *@return the length of the UCS2 string
  */
_SHORT16 ConvGSMToUCS2Str(const _CHAR8* strGSM, _CHAR8* strUCS2);

/**
 *@brief Convert UCS2 mode string to UNICODE mode string
 *
 *@param  const _CHAR8* strUCS2  source string
 *@param  T_pTSTR strUnicode destiminate string for phonebook entry
 *return bool value
 **/
_BOOL ConvUCS2StrToUnicode(const _CHAR8* strUCS2, _USHORT16 *strUnicode);

_SHORT16 Conv8BitDataToUnicode(_CHAR8* str8BitData, _SHORT16 *strUnicode);


/**
*@brief 给模块点火
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 SPR_IgniteModule(_VOID);
/**
*@brief 查询模块点火结果
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL SPR_IgniteResult(_VOID);
/**
*@brief 硬件方式给模块断电，该接口一般在AT关机无效时使用
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 SPR_PowerOffModule(_VOID);
/**
*@brief 查询硬件方式给模块断电的结果
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL SPR_PowerOffResult(_VOID);
/**
*@brief 重启模块
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_UINT32 SPR_ResetModule(_VOID);
/**
*@brief 查询模块重启的结果
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_BOOL SPR_ResetResult(_VOID);
/**
*@brief 让模块睡眠
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_VOID SPR_SleepModule(_VOID);
/**
*@brief 唤醒模块
*@return
*@author  MIAOBAOLI
*@date    2007.11.26
*/
_VOID SPR_WakeupModule(_VOID);

/*==============================================================================*/
/*------------------->   Fwl_Print Level defination begin   <------------------*/
#define C1 1    /*Fatal error message*/
#define C2 2    /*Error message*/
#define C3 3    /*Common message*/

/**
 * @brief Output the trace info to platform trace port
 * @author Ljh
 * @date 2005-05-09
 * @param[in] s 
 * @param[in] ...
 * @return >=0 if printf success, <0 failed
**/
_INT32 Fwl_Print(_UCHAR8 level, const _CHAR8* mStr, const _CHAR8* s, ...);

#endif

//the end of files
