﻿#pragma once
#include "TcpServer.h"
#include "TcpSocket.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "msg.pb.h"
using namespace std;

// 存储磁盘配置文件信息
struct ServerInfo
{
	string serverID;
	string dbUser;
	string dbPasswd;
	string dbSid;
	unsigned short port;
	int maxNode;
	string shmKey;
};

enum KeyLen
{
	LenSmall = 16,
	LenNormal = 24,
	LenLarge = 32
};

class ServerOP
{
public:
	ServerOP(string jsonfile);
	~ServerOP();

	// 操作
	void startServer();
	//密钥协商
	string seckeyArgee(RequestMsg* msg);

	// 根据协商结果创建 线程的回调函数2种方式
	friend void* working(void* arg);
	static void* hardWork(void* arg) {}

	//密钥校验
	string seckeyVertify(RequestMsg* msg);//check;

	//密钥注销
	string seckeyLogout(RequestMsg* msg);

	

private:
	string getRandStr(KeyLen num);

private:
	map<pthread_t, TcpSocket*> m_socket;
	ServerInfo m_info;
	TcpServer* m_server;
};

// 子线程的回调函数
void* working(void* arg);
