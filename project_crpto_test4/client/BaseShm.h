#pragma once
#include <iostream>
#include <sys/shm.h>
#include <sys/ipc.h>

#include <string.h>
#include <string>

using namespace std;

class BaseShm
{
public:
	//ͨ��key�򿪹����ڴ�
	BaseShm(int key);
	//ͨ�����ݽ�����key����/�򿪹����ڴ�
	BaseShm(int key, int size);
	//ͨ��·���򿪹����ڴ�
	BaseShm(string name);
	//ͨ��·������/�򿪹����ڴ�
	BaseShm(string name, int size);

	//���������ڴ�
	void* mapShm();
	//ʹ�����ڴ��뵱ǰ���̷���
	int unmapShm();
	//ɾ�������ڴ�
	int delShm();
	~BaseShm();

private:
	int getShmID(key_t key, int shmSize, int flag);
private:
	int m_shmID;
protected:
	void* m_shmAddr = NULL;
};

