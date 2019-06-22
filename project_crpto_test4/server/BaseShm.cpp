#include "BaseShm.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>



const char RandX = 'x';
BaseShm::BaseShm(int key)
{
	getShmID(key, 0, 0);
}


BaseShm::BaseShm(int key, int size)
{
	getShmID(key, size, IPC_CREAT | 0664);
}

BaseShm::BaseShm(string name)
{//把一个已存在的路径名和一个整数标识符转换成一个key_t值，称为IPC键值
	key_t key = ftok(name.data(), RandX);
	getShmID(key, 0, 0);
}

BaseShm::BaseShm(string name, int size)
{
	key_t key = ftok(name.data(), RandX);
	//创建共享内存
	getShmID(key, size, IPC_CREAT | 0664);
}

void* BaseShm::mapShm()
{
	//关联当前进程和共享内存
	m_shmAddr = shmat(m_shmID, NULL, 0);
	if (m_shmAddr == (void*)-1) {
		cout << "关联共享内存失败!" << endl;
		return NULL;
	}
	return m_shmAddr;
}

int BaseShm::unmapShm()
{	//共享内存与当前进程分离
	int ret = shmdt(m_shmAddr);
	return ret;
}

int BaseShm::delShm()
{	//删除共享内存
	int ret = shmctl(m_shmID, IPC_RMID, NULL);
	return ret;
}

BaseShm::~BaseShm()
{
}
//共享内存创建private
int BaseShm::getShmID(key_t key, int shmSize, int flag)
{
	cout << "共享内存尺寸为：" << shmSize << endl;
	m_shmID = shmget(key, shmSize, flag);
	if (m_shmID == -1) {
		cout << "shmget失败！" << endl;
	}
	////共享内存地址初始化
	//if (shmSize > 0) {
	//	//当前进程与共享内存建立关联
	//	void* addr = shmat(m_shmID, NULL, 0);
	//	memset(addr, 0, shmSize);
	//	//共享内存与当前进程分离 进程就不能操作这块内存了
	//	//是不能写  只能读吗？==>答案：不是的，这只是初始化，每个函数调用都独自有封装
	//	shmdt(addr);//成功0  失败-1
	//}
	//cout << "共享内存创建成功..." << endl;
	return m_shmID;
}
