#pragma once 
#include <iostream>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "TcpSocket.h"
using namespace std;

class TcpServer
{
public:
	TcpServer();
	~TcpServer();

	//服务器设置监听
	int setListen(unsigned short port, const char* ip);
	//等待并接受客户端连接请求，默认连接超时时间为10000s
	TcpSocket* acceptConnectSelect(int wait_seconds = 10000);
	TcpSocket* accpetConnectPoll(int wait_seconds = 10000);
	TcpSocket* accpetConnectEpoll(int wait_seconds = 10000);

	void closefd();

private:
	int m_lfd;//用于监听的文件描述符

};