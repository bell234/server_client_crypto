#pragma once
#include <string>
#include <iostream>

using namespace std;

struct ClientInfo
{
	string serverID;
	string clientID;
	string serverIP;
	unsigned short port;
	int maxNode;
	string shmKey;
};

class ClientOP
{
public:
	// 初始化数据, 从磁盘的配置文件中
	ClientOP(string fileName);//"client.json"
	~ClientOP();

	// 秘钥协商
	bool seckeyAgree();

	// 秘钥校验
	void seckeyCheck();

	// 秘钥注销
	void seckeyLogout();

private:
	ClientInfo m_info;
};

