#pragma once
#include <iostream>
#include <string>
using namespace std;


//通信套接字类  
//超时的时间
static const int TIMEOUT = 10000;
class TcpSocket
{
public:
	enum ErrorType { ParamError = 3001, TimeOutError, PeerCloseError, MallocError };
	TcpSocket();
	//使用一个可以用于通信的套接字实例化套接字对象
	TcpSocket(int connfd);
	~TcpSocket();

	//连接服务器
	int connectToHost(string ip, unsigned short port, int timeOut = TIMEOUT);
	//发送数据
	int sendMsg(string sendData, int timeOut = TIMEOUT);
	//接收数据
	string recvMsg(int timeOut = TIMEOUT);
	//断开连接
	void disConnect();

private:
	//设置IO非阻塞模式
	int setNonBlock(int fd);
	//设置为阻塞模式
	int setBlock(int fd);
	//读超时检测	不含读操作
	int readTimeOut(unsigned int wait_seconds);
	//写超时检测函数	不含写操作
	int writeTimeOut(unsigned int wait_seconds);
	//连接超时的connect函数
	int connectTimeOut(struct sockaddr_in* addr, unsigned int wait_seconds);
	//每次从缓冲区读取n个字符
	int readn(void* buf, int count);
	//每次往缓冲区中写入n个字符
	int writen(const void* buf, int count);
private:
	int m_socket; //通信套接字
};