#include "cwa_sock.h"

int cwaSock_SendData(int send_port, char * data, int len, char *resultData, int retBufSize)
{
     	int sendSocket;			/*cocket句柄和接受到连接后的句柄 */
    	struct sockaddr_in dest_addr;	/*目标地址信息*/

    	sendSocket = socket(AF_INET, SOCK_STREAM, 0);/*建立socket*/
    	if(sendSocket == -1){
        	CPE_LOG_ERROR("socket create failed:%d", errno);
		return -1;
    	}

	struct timeval sendSocketTimeout = {3, 0};
	setsockopt(sendSocket, SOL_SOCKET, SO_SNDTIMEO, &sendSocketTimeout, sizeof(sendSocketTimeout));
    	dest_addr.sin_family = AF_INET;
    	dest_addr.sin_port = htons(send_port);
	inet_pton(AF_INET, ANDROID_SOCK_IP, &dest_addr.sin_addr.s_addr);
	
    
    	if(connect(sendSocket, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == -1){		//连接方法，传入句柄，目标地址和大小 
        	CPE_LOG_ERROR("connect failed:%d", errno);		//失败时可以打印errno 
		close(sendSocket);		//关闭socket
		return -1;
    	}

    	int finish_len = 0, sendedLen = 0;
    	do
    	{
        	sendedLen = send(sendSocket, data + finish_len, len -finish_len, 0);

        	if(sendedLen > 0)
        	{
            		finish_len += sendedLen;
        	}
        	else
        	{
            		close(sendSocket);
            		CPE_LOG_ERROR("sock send failed, len = %d, sendedLen = %d", len, sendedLen);
            		return -1;
        	}
    	} while(finish_len < len);
	CPE_LOG_DEBUG("send data finished, sendSocket = %d, data = %s", sendSocket, data);

    	int recvLen = 0, rs = 1;
    	finish_len = 0;
    	while(rs)
	{
		recvLen = recv(sendSocket, resultData+finish_len, retBufSize-finish_len, 0);
		CPE_LOG_DEBUG("recv once data finished, sendSocket = %d, resultData = %s, recvLen = %d", sendSocket, resultData, recvLen);   	
		if(recvLen < 0)
		{
			// 由于是非阻塞的模式,所以当errno为EAGAIN时,表示当前缓冲区已无数据可读
			// 在这里就当作是该次事件已处理
			if(errno == EAGAIN) {
				break;
			} else {
				close(sendSocket);//关闭socket
				return -1;
			}
		}
		else if(recvLen == 0)
		{
			// 这里表示对端的socket已正常关闭.
		}
		finish_len += recvLen;
		if(finish_len < retBufSize)
			rs = 0;
		else
			rs = 1;// 需要再次读取
	}

    	CPE_LOG_DEBUG("cwaSock_SendData recv data, sendSocket = %d, resultData = %s", sendSocket, resultData);
    
    	close(sendSocket);	//关闭socket

    	return 0; 
}
