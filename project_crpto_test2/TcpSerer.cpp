#include "TcpServer.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


//int m_lfd;//用于监听的文件描述符
TcpServer::TcpServer() {

}

TcpServer::~TcpServer() {

}

//服务器设置监听
int TcpServer::setListen(unsigned short port, const char* ip) {
	int ret = 0;
	if (port > 65535 || port < 0) {
		ret = -1;
		printf("func TcpServer::setListen (port > 65535 || port < 0) err\n");
		
		return ret;
	}
	//创建套接字
	if(-1 == (this->m_lfd = socket(AF_INET, SOCK_STREAM, 0))){
		ret = errno;
		printf("func TcpServer::setListen socket(AF_INET, SOCK_STREAM, 0) err： %d\n", ret);
		return ret;
	}
	//端口复用
	int op = 0;
	if (-1 == (ret = setsockopt(this->m_lfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int)))) {
		ret = errno;
		printf("func func TcpServer::setListen setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int))error : %d\n", ret);
		return ret;
	}
	//绑定端口
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	if (ip != NULL)
	{
		if (-1 == (ret = inet_pton(AF_INET, ip, &serveraddr.sin_addr.s_addr))) {
			printf("func TcpServer::setListen inet_pton(AF_INET, ip, &serveraddr.sin_addr.s_addr) err:%d\n", ret);
			return ret;
		}
	}
	else {
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	////开始绑定
	if (-1 == (ret = bind(this->m_lfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)))) {
		ret = errno;
		printf("func TcpServer::setListen bind(this->m_lfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr) err:%d\n", ret);
		return ret;
	}

	//监听
	if (-1 == (ret = listen(this->m_lfd, 128))) {
		ret = errno;
		printf("func TcpServer::setListen listen(this->m_lfd, 128) err: %d\n", ret);
		return ret;
	}
	return ret;
}

//等待并接受客户端连接请求，默认连接超时时间为10000s
TcpSocket* TcpServer::acceptConnectSelect(int wait_seconds) {
	int ret = 0;
	//判断select时间判定，三次握手完成， 客户端有新的连接
	if (wait_seconds > 0) {
		fd_set accept_fdset;
		FD_ZERO(&accept_fdset);
		FD_SET(this->m_lfd, &accept_fdset);

		struct timeval timeout;
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;

		do {
			ret = select(this->m_lfd + 1, &accept_fdset, NULL, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret <= 0) {
			return NULL;
		}
	}

	//客户端新的连接 此时调用accpet 将不会阻塞
	struct sockaddr_in clientaddr;
	socklen_t clientaddrLen = sizeof(struct sockaddr_in);//sizeof(clientaddr)
	int connfd = accept(this->m_lfd, (struct sockaddr*) & clientaddr, &clientaddrLen);
	if (connfd == -1) {
		printf("func TcpServer::acceptConnect accept(this->m_lfd, (struct sockaddr*) & clientaddr, &clientaddrLen) connfd\n");
		return NULL;
	}

	return new TcpSocket(connfd);
}
TcpSocket* TcpServer::accpetConnectPoll(int wait_seconds) {
	int ret = 0;

	return NULL;
}
TcpSocket* TcpServer::accpetConnectEpoll(int wait_seconds) {
	int ret = 0;


	return NULL;
}
void TcpServer::closefd() {
	close(this->m_lfd);
	
}


	