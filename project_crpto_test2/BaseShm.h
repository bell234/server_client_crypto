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
	//通过key打开共享内存
	BaseShm(int key);
	//通过传递进来的key创建/打开共享内存
	BaseShm(int key, int size);
	//通过路径打开共享内存
	BaseShm(string name);
	//通过路径创建/打开共享内存
	BaseShm(string name, int size);

	//关联共享内存
	void* mapShm();
	//使共享内存与当前进程分离
	int unmapShm();
	//删除共享内存
	int delShm();
	~BaseShm();

private:
	int getShmID(key_t key, int shmSize, int flag);
private:
	int m_shmID;
protected:
	void* m_shmAddr = NULL;
};

