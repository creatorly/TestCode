
#ifndef __UT_BASE64_H__
#define __UT_BASE64_H__

#include <utl_base64.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* UTF8 --> BASE64 */
#define UT_Base64Encode(u, l, b)    utl_base64_enc(u, l, b, UTL_BASE64_ENCODE_BYTES(l))

/* BASE64 --> UTF8 */
#define UT_Base64Decode(b, l, u)    utl_base64_dec(b, l, u, UTL_BASE64_DECODE_BYTES(l))

#if defined(__cplusplus)
}
#endif

#endif

