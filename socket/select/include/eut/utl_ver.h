/********************************************************
* 
*
* Ejoin utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_ver.h
* PACKAGE:      libeut
*   Date:               2016.01.12
*   Author:             PengJian
*   Version:            2.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_VER_H__
#define  __UTL_VER_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 201 add version for utl
 * 202 fixed the issue of memory overflow when error fragment packet received 
 * 203 add the feature of queue mechnizm for utl_tractl
 * 204 add the api of utl_tractl_del()
 * 205 add the api of utl_trm_conn_get_rem()
 * 206 create the log directory if not exists
 * 207 add the macro UTL_STR()
 * 208 add the large(more than 255) packet support for udptl connection
       reserve the logfiles created before as many as possible when rebooting
       fixed the issue of dead loop when udp socket received zero bytes data
       fixed the issue of no udptl SYN packet sent after rebooting
 * 209 add the api utl_cfg_clr()
       add the api utl_select_del_safe()
       clear the records when reloading config if NULL cfgfile is used
       fixed the issue of crash if transaction message received when closing conn
 * 210 change the param "tr_data" of FP_UTL_TR_HOOK_REQ to local address "to"
       add the API of utl_trm_set_hook_msg()
 * 211 fixed the issue of no host set when set the url of http message
 * 212 add the implement for key val pair(key1=val1,key2="val2"...)
 * 213 fixed the issue of overflow in function utl_httpd_sess_get_progress()
 * 214 add the API of utl_udp_chksum() in header file utl_udp.h
 * 215 fixed the issue of crash because of not initialization of the msg handler
 * 216 add the implement of multiplex connection
 * 217 fixed the issue of failure to send data to multiplex connection behind NAT
 * 218 add some trace information for multiplex connections
 * 219 add the http client session cache mechnizm
 * 220 add the api of utl_addr_ipv4_net_same()
 * 221 fixed the issue of escaping error in utl_json_get_str*()
 * 222 fixed the issue of failure to restore the last delimeter of token string
 * 223 add the api utl_httpd_create_qlen()
 *     fixed the issue of failure to accept too many connections for utl_httd_t
 * 230 add the feature of DB connection pool for write operation
 * 231 add the apis utl_tractl_set_timers and utl_tractl_set_hook_avail
 * 232 add the api utl_tractl_create
 * 233 wait the session stopped when utl_sess_stop() is invoked
 * 234 add the feature of source identification for UDP log server
 * 235 close the feature of sending log to server when zero port given
 * 236 fixed the memory overlow when try to get a zero json attribute
 * 237 add the api of utl_dba_pause(), utl_dba_resume() and utl_dba_paused()
 * 238 fixed the issue of no log source for api utl_log_printf()
 * 239 add the api of utl_httpd_set_max_hdrsize()
 * 240 add the api of utl_trm_conn_set_natexp()
 * 241 fixed the issue of mixing the send/receive buffer for crypted packets
 * 242 add TCP support for utl transaction
 * 243 add the feature of sending data via TCP connection of transaction
       add the feature of connection OTR(on transaction layer)
 * 244 fixed the issue of not 8-bytes alignment when compiled in 64-bit machine
 * 245 fixed the issue of dead-lock when invoking utl_log_set_server()
 * 246 fixed the issue of failure to find connection when invoking utl_trm_dblk_send()
 * 247 add the feature of block mode for the transport layer of transaction
 * 248 fixed the issue of failure to parse http response without reason phrase
 * 249 add the api of utl_objs_get_id() for some special application
 * 250 add the api of utl_msgq_recv_ms()
 * 251 add the api of utl_tlv_add_mac() and utl_tlv_get_mac()
 * 252 add the implement of tlv iterator
 * 253 add the api of utl_tlv_get_int_if() and utl_tlv_get_str_if()
 * 254 fixed the issue of failure to get a TLV string without '\0'
 * 255 fixed the issue of dead loop because the api utl_tlv_get_next() always got a value
 * 256 fixed the issue of memory overflow when msgq size is less than 32
 * 257 add the api of utl_log_newfile_fp()
 * 258 fixed the issue of utl_sock_set_linger()
 * 259 add the implement of utl_timer_ex_xxx()
 * 260 only listen localhost for utl select message center
 * 261 fixed the issue of failure to receive a response with short length and FRAG flag
 * 262 fixed the issue of socket statistic error
 * 270 add the implement for tcp transaction
 * 271 fixed the issue of crash when KEEPACK received on UDPS connection
 * 272 fixed the issue of crash when retry the httpc instanse if connection failed last time
 * 273 fixed the issue of failure to change the trpipe expires
 * 274 fixed the issue of failure to send packets if the socket of trconn returned -1
 *     fixed the issue that the trconn send KEEPALIVE always in expires/2
 * 275 add the feature of connection bridging
 * 276 add the api of utl_tpool_set_threads()
 * 277 add the feature of closing connection after 60s when the last KEEPALIVE received
 * 278 fixed the issue of crash when forwarding data to an unexisted pipe
 * 279 fixed the issue of no log again if too many fds are opened
 *
 * 300 add the support of epoll for utl_select
 * 301 replace all system invoking of mem, str and fmt inoutput to eos_xxx()
 * 302 fixed the issue of crash when socket is deleted from epoll mode utl_select
 * 303 replace all system invoking of mem, str and fmt inoutput to eos_xxx()
 * 304 add trace information for the system function epoll_ctl() 
 * 305 fixed the issue of failure to accept if two or more utl_select used
 * 306 fixed the issue of failure to read data when socket is closed too fast
 * 307 add the api of utl_httpc_get_progress()
 * 308 fixed the issue failure to get large file
 * 309 fixed the issue failure to get date
 * 310 fixed the issue of crash when received invalid NTP response
 * 311 use the optimization flag -Os
 * 312 add a default UDP server address for utl_transaction_t
 * 313 compiled in android
 * 314 add the fuantion of utl_mktime()
 * 315 fixed the issue of failure to send tcp request of transaction since 312
 * 316 fixed the issue of failure to send tcp request of transaction since 312
 * 317 fixed the issue of crash when invoking utl_dns_init() two or more times
 * 318 fixed the issue of failure to cut the path of win-style file in log message 
 * 319 enabled the flag UTL_LOG_NOPATH of utl_log_t for ANDROID
 * 320 add the api of utl_qbuf_pop_if()
 * 321 fixed the issue of crash when a new socket using the fd of last deleted socket
 * 322 fixed the issue of socket packets lost if too many data received one time
 * 323 fixed the issue of failure to send message via tcps pipe if remote address not specified
 * 324 add the api of utl_ptr_get_cnt() and utl_ptr_ref_cnt()
 * 325 fixed the issue of failure to change the expires of the pipe for utl transaction
 * 326 fixed the issue of dead loop if the socket is closed after a receiving operation
 * 327 fixed the issue of failure to continue receiving if too many UDP packets received in a short time
 *     fixed the issue of failure to convert a string ip with a space ended to ipv4
 * 328 add the feature of tick dump(UTL_LOG_TICK) in utl log file
 * 329 set the socket to non-block when it is added to select
 * 330 add the lock operation for the handle of utl_cfg_t
 * 331 add the api of utl_addr_init_ipv4()
 * 332 enabled the socket flag FD_CLOEXEC default
 * 333 fixed the issue of crash when opening a space config file
 *     stop a timer automatically if it is running before start it
 * 334 get the uptick from file "/proc/uptime" when dumping a timer
 * 335 print warn and error log to syslog
 * 336 fixed the issue of failure to get a tlv string with one character
 * 337 fixed the issue of get a suffix '\r' when invoking utl_kval_get_r()
 * 338 add the api of utl_log_get_file()
 * 339 fixed the issue of crash when get more logfiles via utl_log_get_file()
 * 340 add the APIs of utl_bin_get_type() and utl_bin_get_len()
 *
 * 400 add the support of unix socket
 * 401 add the API of utl_que_concat()
 * 402 modified the API of utl_ptr_put()
 * 403 add the API of utl_log_set_maxbuf()
 *     add the API of utl_log_set_minnodes()
 * 404 add the header file utl_xor.h
 * 405 auto verify the license when parsing it's data
 * 406 fixed the issue of failure to use http long connection for httpd
 * 407 fixed the issue of failure to encrypt the messages of tcp connection
 * 408 fixed the issue of failure to send long http response
 * 409 fixed the issue of failure to save configuration file
 * 410 add the support for json iteration
 *     fixed the issue of not ending with json end char '}'
 *     removed '=' from the delimiters of configuration item
 * 411 fixed the issue of failure to gethostbyname sometimes
 * 412 add the API of utl_tractl_test()
 *     add the API of utl_tractl_get_offtime()
 *     fixed the issue of failure when too small capability is passed to utl_tractl_create()
 *     fixed the issue of failure when utl_tractl_inc() is invoked too slowly
 *     fixed the issue of no url params if '/' is missed
 * 413 add the API of utl_strwrd()
 * 414 fixed the issue of reporting event UTL_SOEVT_CLOSE twice for TCP
 * 415 fixed the issue of failure to clear the space string using utl_strnospace()
 * 416 fixed the issue of failure to decrypt the packet with the length litter than 12
 * 417 add the API of utl_httpc_wait_resp()
 * 418 fixed the issue of getting invalid logfile name via utl_log_get_file()
 * 419 add the API of utl_hash_for_each_if()
 * 420 add the support of SSL for https
 * 421 add the support of SSL for httpc
 * 422 add the API of utl_http_url_encode() and utl_http_url_decode()
 * 423 fixed the issue of failure to distingurish -1 or none for integer tlv value
 * 424 fixed the issue of crash when received redundancy data for a http client session
 * 425 add the support for http chunk body(the length of body can reach to maximum of 5M)
 * 426 fixed the issue of failure to upload big file via httpc if network speed is too slow
 * 427 fixed the issue of failure to accept a http connection in android platform
 *     add the API of utl_httpd_set_max_bodysize()
 * 428 fixed the issue of still working even if utl_tpool_start() not being invoked
 * 429 add the API of utl_sock_connect_errno()
 * 430 change the retry after time of httpc to one second
 * 431 add the TLS expanding "server name" for some https servers
 * 432 add the support for adding attributes to json object
 * 433 add the support of blocking work mode for httpc, httpd and message queue
 * 434 fixed the issue of failure to answer a https request sometimes
 * 435 fixed the issue of delay releasing of socket if the select block too long time
 *     fixed the issue of the negotiation error of HTTP permanent connection
 * 436 fixed the issue of failure to connect the server which doesn't support TLSv1.2
 * 437 add the support of auto-expanding for utl message queue
 * 438 fixed the issue of crash when httpd listen failed
 * 439 fixed the issue of crash if failure to connect to a https server
 * 440 add the support of utl string
 * 441 add the APIs of utl_sock_open_xxx()
 * 442 disable the KEEPALIVE for transaction udpc connection whose server is not set
 * 443 fixed the issue of no CRLF when saving the invalid configuration line
 * 444 add the API of utl_json_get_par2()
 * 445 add the API of utl_tlv_add_hex()
 * 446 increase the length of filename in logfile
 * 447 fixed the issue of failure to parse the http message which has all lowercase header
 * 448 add the APIs of utl_addr_get_route() and utl_addr_get_ifbytes()
 * 449 add the API of utl_msgq_set_max_msgs()
 * 450 fixed the issue that we can send twice of limitation of message queue
 * 451 optimized the performance of UTL timer
 * 452 add the string end char '\0' to result string in API utl_md5_get_digest()
 * 453 fixed the issue of compileing error with openssl-1.1.x version
 * 454 add the API of utl_conn_type2str()
 * 455 fixed the issue of printing syslog event if it's level is lower than utl itself
 *     add the support of setting syslog level
 * 456 fixed the issue that the md5 digest is truncated by utl_md5_get_digest()
 * 457 add the API of utl_str_pop()
 * 458 fixed the issue that the api utl_mktime() may return a DST timestamp
 * 459 add the API of utl_isalphas(), utl_isuppers and utl_islowers()
 * 460 add the API of UTL_DATE_EQU() and UTL_DATE_LESS()
 * 461 add some license keys
 * 462 optimized the API of utl_str_add_int()
 * 463 add the API of utl_json_pop_str()
 *     add the API of utl_json_strlen()
 * 464 changed the prototype of utl_crc32()
 * 465 add the API of utl_md5_calc_digest()
 * 466 modified the error string in utl_base64_decode()
 * 467 defined the invalid tlv integer value UTL_TLV_INVAL_INT
 * 468 add the API of utl_json_add_fmt()
 * 469 add the API of utl_json_add_nstr()
 * 470 add the API of utl_str_del()
 *
 *******************************************************************************************/

#define     UTL_VERSION         "470"

#if defined(__cplusplus)
}
#endif

#endif

