
#ifndef __EOS_SOCKET_H__
#define __EOS_SOCKET_H__

#include <eos_typedef.h>
#include <eos_socket.h>

#if defined(__cplusplus)
extern "C" {
#endif

_INT32 ut_socket_construct(eosSocket* s, _INT32 addressType,eosSocketProtocol protocolType);
_INT32 ut_socket_reuse_addr(eosSocket *s);
_INT32 ut_socket_set_blocking(eosSocket *s, EOSBool isBlocking);
_INT32 ut_socket_destruct(eosSocket *s,EOSBool cleanSocket,EOSPortRange* portRange);
_INT32 ut_socket_receive_buffer(eosSocket * s,_UCHAR8* buffer,_INT32 bytesToReceive,_INT32* bytesReceived,eosAddress* remoteAddress);
_INT32 ut_socket_send_buffer(eosSocket* s,_UCHAR8* buffer,
    EOSSize_t    bytesToSend,
    eosAddress*  remoteAddress,
    EOSSize_t*   bytesSent);
_INT32 ut_socket_bind(eosSocket* s,eosAddress* address,EOSPortRange* portRange);
_INT32 ut_socket_connect(eosSocket *s, eosAddress* address);
_INT32 ut_socket_accept(eosSocket* s,eosSocket* newSocket,eosAddress* remoteAddress);
_INT32 ut_socket_listen(eosSocket *s, _UINT32 queuelen);
_INT32 ut_socket_get_bytes_available(eosSocket* s, EOSSize_t* bytesAvailable);
_INT32  ut_socket_get_last_error(eosSocket* s, _INT32* lastError);
_INT32 ut_socket_get_local_address(eosSocket *s, eosAddress* address);
_INT32 ut_socket_get_remote_address(eosSocket *s, eosAddress* address);

#if defined(__cplusplus)
}
#endif 

#endif

