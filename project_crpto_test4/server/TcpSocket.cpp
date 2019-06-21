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


//int m_socket; //通信套接字
TcpSocket::TcpSocket() {
	
}
//使用一个可以用于通信的套接字实例化套接字对象
TcpSocket::TcpSocket(int connfd) {
	this->m_socket = connfd;
}
TcpSocket::~TcpSocket() {

}
//连接服务器
int TcpSocket::connectToHost(string ip, unsigned short port, int timeOut) {
	int ret = 0;
	if (port <= 0 || port > 65535 || timeOut < 0) {
		printf("(port <= 0 || port > 65535 || timeOut < 0) error!\n");
		return ret = 0;
	}
	//新建套接字
	if (0 > (this->m_socket = socket(AF_INET, SOCK_STREAM, 0))) {
		ret = errno;
		printf("socket(AF_INET, SOCK_STREAM, 0)) error : %d\n", ret);
		return ret;
	}
	////端口复用
	//int op = 0;
	//if (-1 == (ret = setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int)))) {
	//	ret = errno;
	//	printf("setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int))error : %d\n", ret);
	//	return ret;
	//}
	//绑定端口及地址
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ip.data());
	//建立连接
	if (0 > (ret = connectTimeOut((struct sockaddr_in*) & serverAddr, (unsigned int)timeOut))) {
		//超时
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
//发送数据
int TcpSocket::sendMsg(string sendData, int timeOut) {

	//返回0==>没超时   -1==>超时
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
		//转换网络字节序
		int netLen = htonl(sendData.size());
		//strncpy((char*)netData, (const char*)&netLen, 4);//注意
		memcpy(netData, &netLen, 4);
		memcpy(netData + 4, sendData.data(), sendData.size());

		//没问题发送实际字节数 ==dataLen
		writed = writen(netData, writeDataLen);
		if (writed < writeDataLen) {
			if (netData != NULL) {
				free(netData);
				netData = NULL;
			}
			printf("TcpSocket::sendMsg writen writed < total length!\n");
			return writed;
		}
		//写完释放内存
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
//接收数据
string TcpSocket::recvMsg(int timeOut) {
	//返回0 没有超时就收到数据，  -1超时或有异常
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
	ret = readn(&recvDataLen, 4);//读包头四个字节
	if (ret == -1) {
		printf("func readn() err:%d\n", ret);
		return string();
	}
	else if (ret < 4) {
		printf("func readn() err closed:%d \n", ret);
		return string();
	}
	//根据读包头前四个字节转换的长度  申请内存
	int n = ntohl(recvDataLen);
	char* tmpBuf = (char*)malloc(n + 1);
	if (tmpBuf == NULL) {
		ret = MallocError;
		printf("malloc() err\n");
		return NULL;
	}
	ret = readn(tmpBuf, n);//根据长度读取数据
	if(ret == -1){
		printf("func TcpSocket::recvMsg readn err: %d\n", ret);
		return string();
	}
	else if (ret < n) {
		printf("func TcpSocket::recvMsg readn() peer closed:%d\n", ret);
		return string();
	}
	tmpBuf[n] = '\0';//数组尾部补0
	string data = string(tmpBuf);
	return data;
}
//断开连接
void TcpSocket::disConnect() {
	if (this->m_socket >= 0) {
		close(this->m_socket);
	}
}


//private:
//设置IO非阻塞模式
int TcpSocket::setNonBlock(int fd) {
	int ret = 0, flags = 0;

	if (-1 == (flags = fcntl(fd, F_GETFL))) {
		return flags;
	}
	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);

	return ret;
}
//设置为阻塞模式
int TcpSocket::setBlock(int fd) {
	int ret = 0, flags = 0;

	if (-1 == (flags = fcntl(fd, F_GETFL))) {
		return flags;
	}
	flags &= ~O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);

	return ret;
}
//读超时检测	不含读操作
int TcpSocket::readTimeOut(unsigned int wait_seconds) {
	cout << "TcpSocket::readTimeOut wait_second:" << wait_seconds << endl;
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
//写超时检测函数	不含写操作
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
		//没有超时
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
//连接超时的connect函数
int TcpSocket::connectTimeOut(struct sockaddr_in* addr, unsigned int wait_seconds) {
	int ret = 0, sockLen = 0;
	if (addr == NULL || wait_seconds <= 0) {
		ret = errno;
		printf("(addr == NULL || wait_seconds < 0) errno : %d\n", ret);
		return ret;
	}
	//判断时长设置为非阻塞
	if (wait_seconds > 0) {
		setNonBlock(this->m_socket);
	}
	sockLen = sizeof(sockaddr_in);
	ret = connect(this->m_socket, (struct sockaddr*)addr, sockLen);
	//设置为非阻塞连接返回-1并且errno=EINPROGRESS表示连接进行中
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
		//表示套接字可写，1成功连接，2套接字产生错误
		//错误信息不会保存在errno中，需要调用getsockopt获取
		if (ret == 1) {
			int err = 0, sockOptRet = 0;
			socklen_t sockLen = sizeof(err);
			//获取是否出现错误及状态
			if(-1==(sockOptRet = getsockopt(this->m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen))){		
				printf("func TcpSocket::connectTimeOut getsockopt(this->m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen) error!\n");
				return -1;
			}
			//成功建立连接
			if (err == 0) {
				ret = 0;
			}
			else {//连接失败
				errno = err;
				ret = -1;
			}
		}
		//超时
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
//每次从缓冲区读取n个字符
int TcpSocket::readn(void* buf, int count) {
	size_t dataLeft = count;
	size_t dataRead;
	char* tmpBuf = (char*)buf;
	while (dataLeft > 0) {
		if ((dataRead = read(this->m_socket, tmpBuf, dataLeft)) < 0) {
			//当返回值小于指定的字节数时
			//并不意味着错误; 这可能是因为当前可读取的字节数小于指定的
				//字节数(比如已经接近文件结尾, 或者正在从管道或者终端读取数据, 或者
					//read()被信号中断).
			//读取数据以前被信号中断
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
//每次往缓冲区中写入n个字符

int TcpSocket::writen(const void* buf, int count) {
	size_t dataLeft = count;
	size_t dataWrite;
	char* tmpBuf = (char*)buf;
	while (dataLeft > 0) {
		if ((dataLeft = write(this->m_socket, tmpBuf, dataLeft))< 0) {
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
