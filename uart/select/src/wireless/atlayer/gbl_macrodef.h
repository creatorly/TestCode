
#ifndef GBL_MACRODEF_H
#define GBL_MACRODEF_H

#include "eos.h"

#define MAX_INFO_LEN            50        /**< define maximum string buffer length (bytes) */
#define MAX_TELE_LEN            50        /**< maximum telephone number length*/
#define MIN_TELE_LEN            1        /**< GSM protocol defined that telenumber with length of 1 can be dialed in some cases */
#define MAX_TIME_LEN            40        /**< maximum time length*/
#define MAX_NAME_LEN            16        /**< maximum name length*/
#define MAX_PSWD_LEN            8        /**< maximum password length*/
#define MIN_PSWD_LEN            4        /**< minimum password length*/
#define MAX_FILEPATH_LEN        255        /**< max filepath string len (include path, and not include '\0')*/
#define MAX_USER_PROMPT_LENGTH  40        /**< maximum length of user prompt */



#define FONT_SIZE_NUM            5            /**< number of font size*/
#define    INITIALIZED_FLAG        0x51fa        /**< a random value for identify initialized or not */
#define MAX_DS_SETTING            5            /**< maximum quantity of data service setting*/
#define GBL_RAWSUM                0x5a5a5a5a

typedef _CHAR8    T_STR_INFO[MAX_INFO_LEN+1];            /**< type definition for information string */
typedef _CHAR8    T_STR_TELE[MAX_TELE_LEN+1];            /**< type definition for telenumber string */
typedef _CHAR8    T_STR_NAME[MAX_NAME_LEN+1];            /**< type definition for telenumber string */
typedef _CHAR8    T_STR_TIME[MAX_TIME_LEN+1];            /**< type definition for time string */
typedef _CHAR8    T_STR_PSWD[MAX_PSWD_LEN+1];            /**< type definition for password string */
typedef _CHAR8    T_STR_FILEPATH[ MAX_FILEPATH_LEN + 1 ];        /**< type definition for file path string*/
typedef _CHAR8      T_STR_PROMPT[MAX_USER_PROMPT_LENGTH+1]; /**< type definition for user prompt string*/
typedef _CHAR8    T_STR_20[21];                        /**< type definition for string of 20 characters*/
typedef _CHAR8    T_STR_50[51];                        /**< type definition for string of 50 characters*/
typedef _CHAR8    T_STR_100[101];                        /**< type definition for string of 100 characters*/
typedef _CHAR8    T_STR_200[201];                        /**< type definition for string of 200 characters*/
typedef _CHAR8    T_STR_300[301];                        /**< type definition for string of 300 characters*/
typedef _CHAR8    T_STR_400[401];                        /**< type definition for string of 400 characters*/
typedef _CHAR8    T_STR_500[501];                        /**< type definition for string of 500 characters*/
typedef _CHAR8    T_STR_600[601];                        /**< type definition for string of 600 characters*/
typedef _CHAR8    T_STR_800[801];                        /**< type definition for string of 800 characters*/
typedef _CHAR8    T_STR_1024[1025];                    /**< type definition for string of 1024 characters*/
typedef _CHAR8    T_STR_8000[8001];                    /**< type definition for string of 8000 characters*/
typedef _CHAR8    T_STR_10240[10241];                    /**< type definition for string of 10240 characters*/
    
typedef _USHORT16    T_WSTR_NAME[MAX_NAME_LEN+1];

/** @}*/

#define AK_ASSERT_PTR(_ptr_, _msg_, _retval_)    if (!(_ptr_)) { printf("ASSERT:%s, file=%s, line=%d\n",_msg_, __FILE__, (_UINT32)__LINE__); for(;;); return (_retval_); }
#define AK_ASSERT_PTR_VOID(_ptr_, _msg_)    if (!(_ptr_)) { printf("ASSERT:%s, file=%s, line=%d\n",_msg_, __FILE__, (_UINT32)__LINE__); for(;;); }
#define AK_FUNCTION_ENTER(_funcname_)
#define AK_FUNCTION_LEAVE(_funcname_)
#define AK_FUNCTION_RET_INT(_funcname_, _retval_)
#define Fwl_Printf printf

#define _stk_Trace

#endif


