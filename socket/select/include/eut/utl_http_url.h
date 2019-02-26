/******************************************************************************
                Copyright (c) 2018-2018 Ejoin-Tech Inc.
                
Filename   :    utl_http_url.h
Description:    define the interface of http url
Revision:       1.0
Author:         pengjian
Date:           2018.06.12
******************************************************************************/

#ifndef __UTL_HTTP_URL_H__
#define __UTL_HTTP_URL_H__

#ifdef __cplusplus
extern "C" {
#endif

int utl_http_url_encode(const char* url, char* encoded_url, int size);
int utl_http_url_decode(const char* url, char* decoded_url, int size);

#ifdef __cplusplus
}
#endif

#endif

