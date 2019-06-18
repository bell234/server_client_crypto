#include "TcpServer.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


//int m_lfd;//���ڼ������ļ�������
TcpServer::TcpServer() {

}

TcpServer::~TcpServer() {

}

//���������ü���
int TcpServer::setListen(unsigned short port, const char* ip) {
	int ret = 0;
	if (port > 65535 || port < 0) {
		ret = -1;
		printf("func TcpServer::setListen (port > 65535 || port < 0) err\n");
		
		return ret;
	}
	//�����׽���
	if(-1 == (this->m_lfd = socket(AF_INET, SOCK_STREAM, 0))){
		ret = errno;
		printf("func TcpServer::setListen socket(AF_INET, SOCK_STREAM, 0) err�� %d\n", ret);
		return ret;
	}
	//�˿ڸ���
	int op = 0;
	if (-1 == (ret = setsockopt(this->m_lfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int)))) {
		ret = errno;
		printf("func func TcpServer::setListen setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int))error : %d\n", ret);
		return ret;
	}
	//�󶨶˿�
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

	////��ʼ��
	if (-1 == (ret = bind(this->m_lfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)))) {
		ret = errno;
		printf("func TcpServer::setListen bind(this->m_lfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr) err:%d\n", ret);
		return ret;
	}

	//����
	if (-1 == (ret = listen(this->m_lfd, 128))) {
		ret = errno;
		printf("func TcpServer::setListen listen(this->m_lfd, 128) err: %d\n", ret);
		return ret;
	}
	return ret;
}

//�ȴ������ܿͻ�����������Ĭ�����ӳ�ʱʱ��Ϊ10000s
TcpSocket* TcpServer::acceptConnectSelect(int wait_seconds) {
	int ret = 0;
	//�ж�selectʱ���ж�������������ɣ� �ͻ������µ�����
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

	//�ͻ����µ����� ��ʱ����accpet ����������
	struct sockaddr_in clientaddr;
	socklen_t clientaddrLen = sizeof(clientaddr);//sizeof(sockadd_in)
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


	