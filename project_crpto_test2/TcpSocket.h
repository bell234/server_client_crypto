#pragma once
#include <iostream>
#include <string>
using namespace std;


//ͨ���׽�����  
//��ʱ��ʱ��
static const int TIMEOUT = 10000;
class TcpSocket
{
public:
	enum ErrorType { ParamError = 3001, TimeOutError, PeerCloseError, MallocError };
	TcpSocket();
	//ʹ��һ����������ͨ�ŵ��׽���ʵ�����׽��ֶ���
	TcpSocket(int connfd);
	~TcpSocket();

	//���ӷ�����
	int connectToHost(string ip, unsigned short port, int timeOut = TIMEOUT);
	//��������
	int sendMsg(string sendData, int timeOut = TIMEOUT);
	//��������
	string recvMsg(int timeOut = TIMEOUT);
	//�Ͽ�����
	void disConnect();

private:
	//����IO������ģʽ
	int setNonBlock(int fd);
	//����Ϊ����ģʽ
	int setBlock(int fd);
	//����ʱ���	����������
	int readTimeOut(unsigned int wait_seconds);
	//д��ʱ��⺯��	����д����
	int writeTimeOut(unsigned int wait_seconds);
	//���ӳ�ʱ��connect����
	int connectTimeOut(struct sockaddr_in* addr, unsigned int wait_seconds);
	//ÿ�δӻ�������ȡn���ַ�
	int readn(void* buf, int count);
	//ÿ������������д��n���ַ�
	int writen(const void* buf, int count);
private:
	int m_socket; //ͨ���׽���
};