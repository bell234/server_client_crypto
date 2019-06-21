#pragma once
#include <string>
#include <iostream>
using namespace std;

// 存储磁盘配置文件信息
struct ClientInfo
{
	/*string serverID;
	string dbUser;
	string dbPasswd;
	string dbSid;*/

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
	//初始化数据，从磁盘的配置文件中
	ClientOP(string fileName);//jsonfile
	~ClientOP();

	//密钥协商
	bool seckeyAgree();

	//密钥校验
	void seckeyCheck();

	//密钥注销
	void seckeyLogout();

private:
	ClientInfo m_info;
};


