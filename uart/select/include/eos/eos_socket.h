#ifndef __EOS_SOCKET_H
#define __EOS_SOCKET_H


#if defined(__cplusplus)
extern "C" {
#endif

/* young for linux    */
#if ((EJOIN_OS_TYPE==EJOIN_OS_TYPE_VXWORKS)||(EJOIN_OS_TYPE==EJOIN_OS_TYPE_LINUX))
    typedef _INT32 eosSocket; 
#elif (EJOIN_OS_TYPE==EJOIN_OS_TYPE_WIN32)
    typedef SOCKET eosSocket;
#endif

#define EOS_ADDRESS_TYPE_IPV4        1
#define EOS_ADDRESS_TYPE_IPV6        2

typedef enum
{
    eosSocketProtocolUdp = 0,
    eosSocketProtocolTcp
} eosSocketProtocol;

#if EOS_SOCKET_HAS_IPV6
#define EOS_SOCKET_SOCKADDR_SIZE sizeof(struct sockaddr_storage)
#else
#define EOS_SOCKET_SOCKADDR_SIZE sizeof(struct sockaddr_in)
#endif

_INT32 eos_socket_construct(eosSocket* s, _INT32 addressType,eosSocketProtocol protocolType);
_INT32 eos_socket_set_linger(eosSocket *s, _INT32 lingerTime);
_INT32 eos_socket_reuse_addr(eosSocket *s);

_BOOL eos_socket_is_valid(eosSocket *s);

_INT32 eos_socket_set_blocking(eosSocket *s, EOSBool isBlocking);
/* close the socket when create a sub-process by exec() */
_INT32 eos_socket_set_cloexec(eosSocket *s, EOSBool isCloExec);

_INT32 eos_socket_set_buffers(eosSocket *s, _INT32 sendSize, _INT32 recvSize);
_INT32 eos_socket_shutdown(eosSocket* s,_INT32 cleanSocket);
_INT32 eos_socket_destruct(eosSocket *s,EOSBool cleanSocket,EOSPortRange* portRange);
_INT32 eos_socket_receive_buffer(eosSocket * s,_VOID* buffer,_INT32 bytesToReceive,_INT32* bytesReceived,eosAddress* remoteAddress);
_INT32 eos_socket_send_buffer(eosSocket* s,const _VOID* buffer,
    EOSSize_t    bytesToSend,
    eosAddress*  remoteAddress,
    EOSSize_t*   bytesSent);
_INT32 eos_socket_bind(eosSocket* s,eosAddress* address,EOSPortRange* portRange);
_INT32 eos_socket_connect(eosSocket *s, eosAddress* address);
_INT32 eos_socket_accept(eosSocket* s,eosSocket* newSocket,eosAddress* remoteAddress);
_INT32 eos_socket_listen(eosSocket *s, _UINT32 queuelen);
_INT32 eos_socket_get_bytes_available(eosSocket* s, EOSSize_t* bytesAvailable);
_INT32  eos_socket_get_last_error(eosSocket* s, _INT32* lastError);
_INT32 eos_socket_get_local_address(eosSocket *s, eosAddress* address);
_INT32 eos_socket_get_remote_address(eosSocket *s, eosAddress* address);

/* young 20060917    */
_INT32    eos_sock_init(_INT32    iMax);
_INT32 eos_sock_show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);


#if defined(__cplusplus)
}
#endif 
#endif

