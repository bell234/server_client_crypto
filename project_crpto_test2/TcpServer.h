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

	//���������ü���
	int setListen(unsigned short port, const char* ip);
	//�ȴ������ܿͻ�����������Ĭ�����ӳ�ʱʱ��Ϊ10000s
	TcpSocket* acceptConnectSelect(int wait_seconds = 10000);
	TcpSocket* accpetConnectPoll(int wait_seconds = 10000);
	TcpSocket* accpetConnectEpoll(int wait_seconds = 10000);

	void closefd();

private:
	int m_lfd;//���ڼ������ļ�������

};