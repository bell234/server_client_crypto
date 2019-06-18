#include "TcpSocket.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>


//int m_socket; //ͨ���׽���
TcpSocket::TcpSocket() {
	
}
//ʹ��һ����������ͨ�ŵ��׽���ʵ�����׽��ֶ���
TcpSocket::TcpSocket(int connfd) {
	this->m_socket = connfd;
}
TcpSocket::~TcpSocket() {

}
//���ӷ�����
int TcpSocket::connectToHost(string ip, unsigned short port, int timeOut) {
	int ret = 0;
	if (port <= 0 || port > 65535 || timeOut < 0) {
		printf("(port <= 0 || port > 65535 || timeOut < 0) error!\n");
		return ret = 0;
	}
	//�½��׽���
	if (0 > (this->m_socket = socket(AF_INET, SOCK_STREAM, 0))) {
		ret = errno;
		printf("socket(AF_INET, SOCK_STREAM, 0)) error : %d\n", ret);
		return ret;
	}
	////�˿ڸ���
	//int op = 0;
	//if (-1 == (ret = setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int)))) {
	//	ret = errno;
	//	printf("setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int))error : %d\n", ret);
	//	return ret;
	//}
	//�󶨶˿ڼ���ַ
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip.data());
	//��������
	if (0 > (ret = connectTimeOut((struct sockaddr_in*) & serverAddr, timeOut))) {
		//��ʱ
		if (ret == -1 && errno == ETIMEDOUT) {
			ret = TimeOutError;
			return ret;
		}
		else {
			printf("connectTimeOut((struct sockaddr_in*) & serverAddr, timeOut) errno: %d\n", errno);
			return errno;
		}
	}
	return ret;	
}
//��������
int TcpSocket::sendMsg(string sendData, int timeOut) {

	//����0==>û��ʱ   -1==>��ʱ
	int ret = writeTimeOut(timeOut);

	if (ret == 0) {
		int writed = 0;
		int writeDataLen = sendData.size() + 4;
		unsigned char* netData = (unsigned char*)malloc(writeDataLen);
		if (netData == NULL) {
			ret = MallocError;
			printf("func sendMsg malloc(writeDataLen) netData == NULL error");
			return ret;
		}
		//ת�������ֽ���
		int netLen = htonl(sendData.size());
		strncpy((char*)netData, (const char*)&netLen, 4);//ע��
		memcpy(netData + 4, sendData.data(), sendData.size());

		//û���ⷢ��ʵ���ֽ��� ==dataLen
		writed = writen(netData, writeDataLen);
		if (writed < writeDataLen) {
			if (netData != NULL) {
				free(netData);
				netData = NULL;
			}
			printf("TcpSocket::sendMsg writen writed < total length!\n");
			return writed;
		}
		//д���ͷ��ڴ�
		if (netData != NULL) {
			free(netData);
			netData = NULL;
		}
	}
	else {
		if (ret == -1 && errno == ETIMEDOUT) {
			ret = TimeOutError;
			printf("func sendMsg writeTimeOut(timeOut) error:%d\n", ret);
		}
	}

	return ret;
}
//��������
string TcpSocket::recvMsg(int timeOut) {
	//����0 û�г�ʱ���յ����ݣ�  -1��ʱ�����쳣
	int ret = readTimeOut(timeOut);
	if (ret != 0) {
		if (ret == -1 || errno == ETIMEDOUT) {
			printf("func TcpSocket::recvMsg readTimeOut ETIMEDOUT error\n");
			return string();
		}
		else {
			printf("TcpSocket::recvMsg readTimeout(timeout) err:%d\n", ret);
			return string();
		}
	}

	int recvDataLen = 0;
	ret = readn(&recvDataLen, 4);//����ͷ�ĸ��ֽ�
	if (ret == -1) {
		printf("func readn() err:%d\n", ret);
		return string();
	}
	else if (ret < 4) {
		printf("func readn() err closed:%d \n", ret);
		return string();
	}
	//���ݶ���ͷǰ�ĸ��ֽ�ת���ĳ���  �����ڴ�
	int n = ntohl(recvDataLen);
	char* tmpBuf = (char*)malloc(n + 1);
	if (tmpBuf == NULL) {
		ret = MallocError;
		printf("malloc() err\n");
		return NULL;
	}
	ret = readn(tmpBuf, n);//���ݳ��ȶ�ȡ����
	if(ret == -1){
		printf("func TcpSocket::recvMsg readn err: %d\n", ret);
		return string();
	}
	else if (ret < n) {
		printf("func TcpSocket::recvMsg readn() peer closed:%d\n", ret);
		return string();
	}
	tmpBuf[n] = '\0';//����β����0
	string data = string(tmpBuf);
	return data;
}
//�Ͽ�����
void TcpSocket::disConnect() {
	if (this->m_socket >= 0) {
		close(this->m_socket);
	}
}


//private:
//����IO������ģʽ
int TcpSocket::setNonBlock(int fd) {
	int ret = 0, flags = 0;

	if (-1 == (flags = fcntl(fd, F_GETFL))) {
		return flags;
	}
	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);

	return ret;
}
//����Ϊ����ģʽ
int TcpSocket::setBlock(int fd) {
	int ret = 0, flags = 0;

	if (-1 == (flags = fcntl(fd, F_GETFL))) {
		return flags;
	}
	flags &= ~O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);

	return ret;
}
//����ʱ���	����������
int TcpSocket::readTimeOut(unsigned int wait_seconds) {
	int ret = 0;
	if (wait_seconds > 0) {
		fd_set readTimeOutSet;
		FD_ZERO(&readTimeOutSet);
		FD_SET(this->m_socket, &readTimeOutSet);

		struct timeval timeout;
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do {
			ret = select(this->m_socket + 1, NULL, &readTimeOutSet, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		
		if (ret == -1) {
			return 0;
		}
		else if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
	}
	return ret;
}
//д��ʱ��⺯��	����д����
int TcpSocket::writeTimeOut(unsigned int wait_seconds) {
	int ret = 0;
	if (wait_seconds > 0) {

		fd_set writeTimeOutSet;
		FD_ZERO(&writeTimeOutSet);
		FD_SET(this->m_socket, &writeTimeOutSet);

		struct timeval timeout;
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		
		do {
			ret = select(this->m_socket + 1, NULL, &writeTimeOutSet, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		//û�г�ʱ
		if (ret == 1) {
			ret = 0;
		}
		else if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
	}
	return ret;
}
//���ӳ�ʱ��connect����
int TcpSocket::connectTimeOut(struct sockaddr_in* addr, unsigned int wait_seconds) {
	int ret = 0, sockLen = 0;
	if (addr == NULL || wait_seconds <= 0) {
		ret = errno;
		printf("(addr == NULL || wait_seconds < 0) errno : %d\n", ret);
		return ret;
	}
	//�ж�ʱ������Ϊ������
	if (wait_seconds > 0) {
		setNonBlock(this->m_socket);
	}
	sockLen = sizeof(sockaddr_in);
	ret = connect(this->m_socket, (struct sockaddr*)addr, sockLen);
	//����Ϊ���������ӷ���-1����errno=EINPROGRESS��ʾ���ӽ�����
	if (ret < 0 && errno == EINPROGRESS) {

		fd_set connect_fdset;		
		FD_ZERO(&connect_fdset);
		FD_SET(this->m_socket, &connect_fdset);

		struct timeval timeout;
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;

		do {
			ret = select(this->m_socket + 1, NULL, &connect_fdset, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		//��ʾ�׽��ֿ�д��1�ɹ����ӣ�2�׽��ֲ�������
		//������Ϣ���ᱣ����errno�У���Ҫ����getsockopt��ȡ
		if (ret == 1) {
			int err = 0, sockOptRet = 0;
			socklen_t sockLen = sizeof(err);
			//��ȡ�Ƿ���ִ���״̬
			if(-1==(sockOptRet = getsockopt(this->m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen))){		
				printf("func TcpSocket::connectTimeOut getsockopt(this->m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen) error!\n");
				return -1;
			}
			//�ɹ���������
			if (err == 0) {
				ret = 0;
			}
			else {//����ʧ��
				errno = err;
				ret = -1;
			}
		}
		//��ʱ
		else if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret < 0) {
			return -1;
		}
	}
	if (wait_seconds > 0) {
		setBlock(this->m_socket);
	}
	return ret;
}
//ÿ�δӻ�������ȡn���ַ�
int TcpSocket::readn(void* buf, int count) {
	size_t dataLeft = count;
	size_t dataRead;
	char* tmpBuf = (char*)buf;
	while (dataLeft > 0) {
		if ((dataRead = read(this->m_socket, tmpBuf, dataLeft)) < 0) {
			//������ֵС��ָ�����ֽ���ʱ
			//������ζ�Ŵ���; ���������Ϊ��ǰ�ɶ�ȡ���ֽ���С��ָ����
				//�ֽ���(�����Ѿ��ӽ��ļ���β, �������ڴӹܵ������ն˶�ȡ����, ����
					//read()���ź��ж�).
			//��ȡ������ǰ���ź��ж�
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		else if (dataRead == 0) {
			return count - dataLeft;
		}
		tmpBuf += dataRead;
		dataLeft -= dataRead;
	}
	return count;
}
//ÿ������������д��n���ַ�

int TcpSocket::writen(const void* buf, int count) {
	size_t dataLeft = count;
	size_t dataWrite;
	char* tmpBuf = (char*)buf;
	while (dataLeft > 0) {
		if ((dataLeft = write(this->m_socket, tmpBuf, dataLeft))) {
			if (errno == EINTR) {
				continue;
			}
			return -1;
		}
		else if (dataLeft == 0) {
			return count - dataLeft;
		}
		tmpBuf += dataWrite;
		dataLeft = dataWrite;
	}
	return count;
}
