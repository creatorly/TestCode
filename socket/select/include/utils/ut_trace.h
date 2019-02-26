
#ifndef __UT_TRACE_H__
#define __UT_TRACE_H__

#include <eos_statistics.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_UT_TRACE

/**
 * Initialize the system trace
 *
 * @param level:    the default trace level for all modules
 * @param max_bytes: specify the max bytes of all log data
 *
 * @return:         OK(0) on success or ERROR(-1) otherwise.
 */
_INT32 ut_trace_init(_INT32 level, _INT32 max_bytes);

/**
 * Register a trace module with specified module name
 *
 * @param pcMName:  the specified module name
 * @param pcSMName: the specified submodule name, maybe a levelname
 * @param level:    the specified level
 *
 * @return:         the module id on success or ERROR otherwise.
 */
_INT32 ut_trace_registration(const _CHAR8* pcMName, const _CHAR8* pcSMName, _INT32 level);

/**
 * Unregister a trace module by the specified module name
 *
 * @param mid:      the module id returned by ut_trace_registration()
 *
 * @return:         OK on success or ERROR otherwise.
 */
_INT32 ut_trace_unregistration(_INT32 mid);

/**
 * Trace a log information with specified module name
 *
 * @param mid:      the specified module id
 * @param pcSMName: the specified submodule name, maybe a levelname
 * @param iLevel:   the specified trace level
 * @param pcInfo:   the information to be logout
 * @param lErrorNum:    the errno for level ERROR or WARN
 * @param cFile:    the filename
 * @param lLine:    the line number in file
 *
 * @return:         the log length on success or ERROR otherwise.
 */
_INT32 ut_trace_out(_INT32 mid, const _CHAR8* pcSMName, _INT32 iLevel, 
                    const _CHAR8* pcInfo, _INT32 lErrorNum, const _CHAR8* cFile, _INT32 lLine);

/**
 * Print all the current trace informations
 *
 * @param pPrintFunc:   the user print function pointer
 * @param pData:        the user data
 *
 * @return:         the print length on success or ERROR otherwise.
 */
_INT32 ut_trace_print(EOS_INFO_PRINTF pPrintFunc, _VOID* pData);

/**
 * Show the system trace information
 *
 * @param pPrintFunc:   the user print function pointer
 * @param pData:        the user data
 *
 * @return:         the print length on success or ERROR otherwise.
 */
_INT32 ut_trace_show(EOS_INFO_PRINTF pPrintFunc, _VOID* pData);

/**
 * Set the trace level of specified module
 *
 * @param mid:      the module id
 * @param iLevel:   the module log level
 *
 * @return:         OK on success or ERROR otherwise.
 */
_INT32 ut_trace_set_level(_INT32 mid, _INT32 iLevel);

#define UT_TRACE_OUT(Name, sName, Level, Info, Err) \
            ut_trace_out(Name, sName, Level, Info, Err, __FILE__, __LINE__)    

#endif

#if defined(__cplusplus)
}
#endif

#endif

