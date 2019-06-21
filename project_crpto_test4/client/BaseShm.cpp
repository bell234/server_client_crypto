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
{//��һ���Ѵ��ڵ�·������һ��������ʶ��ת����һ��key_tֵ����ΪIPC��ֵ
	key_t key = ftok(name.data(), RandX);
	getShmID(key, 0, 0);
}

BaseShm::BaseShm(string name, int size)
{
	key_t key = ftok(name.data(), RandX);
	//���������ڴ�
	getShmID(key, size, IPC_CREAT | 0664);
}

void* BaseShm::mapShm()
{
	//������ǰ���̺͹����ڴ�
	m_shmAddr = shmat(m_shmID, NULL, 0);
	if (m_shmAddr == (void*)-1) {
		cout << "���������ڴ�ʧ��!" << endl;
		return NULL;
	}
	return m_shmAddr;
}

int BaseShm::unmapShm()
{	//�����ڴ��뵱ǰ���̷���
	int ret = shmdt(m_shmAddr);
	return ret;
}

int BaseShm::delShm()
{	//ɾ�������ڴ�
	int ret = shmctl(m_shmID, IPC_RMID, NULL);
	return ret;
}

BaseShm::~BaseShm()
{
}

//�����ڴ洴��private
int BaseShm::getShmID(key_t key, int shmSize, int flag)
{
	cout << "�����ڴ�ߴ�Ϊ��" << shmSize << endl;
	m_shmID = shmget(key, shmSize, flag);
	if (m_shmID == -1) {
		cout << "shmgetʧ�ܣ�" << endl;
	}
	////�����ڴ��ַ��ʼ��
	//if (shmSize > 0) {
	//	//��ǰ�����빲���ڴ潨������
	//	void* addr = shmat(m_shmID, NULL, 0);
	//	memset(addr, 0, shmSize);
	//	//�����ڴ��뵱ǰ���̷��� ���̾Ͳ��ܲ�������ڴ���
	//	//�ǲ���д  ֻ�ܶ���==>�𰸣����ǵģ���ֻ�ǳ�ʼ����ÿ���������ö������з�װ
	//	shmdt(addr);//�ɹ�0  ʧ��-1
	//}
	//cout << "�����ڴ洴���ɹ�..." << endl;
	return m_shmID;
}
