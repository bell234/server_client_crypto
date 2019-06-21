#pragma once
#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "BaseShm.h"

//�洢�ڵ㹲���ڴ���Ϣ
class NodeSHMInfo {
public:
	NodeSHMInfo() :status(0), seckeyID(0) {
		bzero(clientID, sizeof(clientID));
		bzero(serverID, sizeof(serverID));
		bzero(seckey, sizeof(seckey));
	}
	int status;//���
	int seckeyID;//��ԿID
	char clientID[12];//�ͻ���
	char serverID[12];//������
	char seckey[128];//��Կ
};

class SecKeyShm :public BaseShm
{
public:
	SecKeyShm(int key);
	SecKeyShm(int key, int maxNode);
	SecKeyShm(string pathName);
	SecKeyShm(string pathName, int maxNode);
	~SecKeyShm();

	void shmInit();
	int shmWrite(NodeSHMInfo* pNodeInfo);
	NodeSHMInfo shmRead(string clientID, string serverID);
private:
	int m_maxNode;//�����ڴ���󳤶�
};

