#pragma once
#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "BaseShm.h"

//存储节点共享内存信息
class NodeSHMInfo {
public:
	NodeSHMInfo() :status(0), seckeyID(0) {
		bzero(clientID, sizeof(clientID));
		bzero(serverID, sizeof(serverID));
		bzero(seckey, sizeof(seckey));
	}
	int status;//身份
	int seckeyID;//密钥ID
	char clientID[12];//客户端
	char serverID[12];//服务器
	char seckey[128];//密钥
};

class SecKeyShm :public BaseShm
{
public:
	//SecKeyShm(int key);
	SecKeyShm(int key, int maxNode);
	//SecKeyShm(string pathName);
	SecKeyShm(string pathName, int maxNode);
	~SecKeyShm();

	void shmInit();
	int shmWrite(NodeSHMInfo* pNodeInfo);
	NodeSHMInfo shmRead(string clientID, string serverID);
private:
	int m_maxNode;//共享内存最大长度
};

