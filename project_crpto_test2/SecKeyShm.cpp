#include "SecKeyShm.h"

SecKeyShm::SecKeyShm(int key) :BaseShm(key) {

}
SecKeyShm::SecKeyShm(int key, int maxNode):
	BaseShm(key, maxNode*sizeof(NodeSHMInfo)),
		m_maxNode(maxNode){

}
SecKeyShm::SecKeyShm(string pathName):
	BaseShm(pathName) {

}
SecKeyShm::SecKeyShm(string pathName, int maxNode):
		BaseShm(pathName, maxNode*sizeof(NodeSHMInfo)),
		m_maxNode(maxNode){

}
SecKeyShm::~SecKeyShm() {

}

void SecKeyShm::shmInit()
{
	if (m_shmAddr != NULL) {
		memset(m_shmAddr, 0, m_maxNode * sizeof(NodeSHMInfo));
	}
}

int SecKeyShm::shmWrite(NodeSHMInfo* pNodeInfo) {
	int ret = -1;
	//������ǰ���� �����ڴ�
	NodeSHMInfo* pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL) {
		return ret;
	}
	
	NodeSHMInfo* pNode = NULL;

	for (int i = 0; i < m_maxNode; ++i) {
		//pNode����ָ��ÿ���ڵ��׵�ַ
		pNode = pAddr + i;
		cout << i << endl;
		cout << pNode->clientID << ", " << pNodeInfo->clientID << endl;
		cout << pNode->serverID << ", " << pNodeInfo->serverID << endl;
		//�жϴ����������Կ�Ƿ��Ѿ����� 
		if ((strcmp(pNode->clientID, pNodeInfo->clientID) == 0) &&
			(strcmp(pNode->serverID, pNodeInfo->serverID) == 0)) {
			//ʹ������Կ���Ǿɵ���Կֵ
			memcpy(pNode, pNodeInfo, sizeof(pNodeInfo));
			unmapShm();
			cout << "д���ݳɹ���ԭ���ݱ�����" << endl;
			return 0;
		}
	}
	//���û���ҵ���Ӧ��Ϣ���ҵ�һ���սڵ㽫��Կ��Ϣд��
	int i = 0;
	NodeSHMInfo tmpNodeInfo;//�սڵ� Ĭ�Ϲ��캯���Ѿ���ʼ����
	for ( i = 0; i < m_maxNode; ++i) {
		pNode = pAddr + i;
		//��սڵ�Ա�
		if (memcmp(&tmpNodeInfo, pNode, sizeof(pNode))==0) {
			ret = 0;
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			cout << "д���ݳɹ������µĽڵ������������" << endl;
			break;
		}		
	}
	if (i == m_maxNode) {
		cout << "д����ʧ�ܣ� û���µĽڵ������" << endl;
		ret = -1;
	}
	//�����ڴ��뵱ǰ�̷߳���
	unmapShm();
	return ret;

}
NodeSHMInfo SecKeyShm::shmRead(string clientID, string serverID) {
	int ret = 0;
	//���������ڴ�
	NodeSHMInfo* pAddr = NULL;
	//����
	pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL) {
		cout << "���������ڴ�ʧ�ܣ�" << endl;
		return NodeSHMInfo();
	}
	cout << "���������ڴ�ɹ�" << endl;
	//����������Ϣ
	int i = 0;
	NodeSHMInfo info;
	NodeSHMInfo* pNode = NULL;
	//ͨ��clientID  serverID ���ҽ��
	cout << "maxNode: " << m_maxNode << endl;
	for (int i = 0; i < m_maxNode; ++i) {
		pNode = pAddr + i;
		cout << i << endl;
		cout << pNode->clientID << ", " << clientID.data() << endl;
		cout << pNode->serverID << ", " << serverID.data() << endl;
		if ((strcmp(pNode->clientID, clientID.data()) == 0) &&
			(strcmp(pNode->serverID, serverID.data()) == 0)) {
			//�ҵ��ڵ���Ϣ����������������
			info = *pNode;
			cout << "*****************************" << endl;
			cout << info.clientID << ", " << info.serverID << ", "
				<< info.seckeyID << ", " << info.status << ", "
				<< info.seckey << endl;
			cout << "*****************************" << endl;
			break;
		}
	}
	unmapShm();
	return info;
}