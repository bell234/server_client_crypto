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
	//关联当前进程 共享内存
	NodeSHMInfo* pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL) {
		return ret;
	}
	
	NodeSHMInfo* pNode = NULL;

	for (int i = 0; i < m_maxNode; ++i) {
		//pNode依次指向每个节点首地址
		pNode = pAddr + i;
		cout << i << endl;
		cout << pNode->clientID << ", " << pNodeInfo->clientID << endl;
		cout << pNode->serverID << ", " << pNodeInfo->serverID << endl;
		//判断传入网点的密钥是否已经存在 
		if ((strcmp(pNode->clientID, pNodeInfo->clientID) == 0) &&
			(strcmp(pNode->serverID, pNodeInfo->serverID) == 0)) {
			//使用新密钥覆盖旧的密钥值
			memcpy(pNode, pNodeInfo, sizeof(pNodeInfo));
			unmapShm();
			cout << "写数据成功：原数据被覆盖" << endl;
			return 0;
		}
	}
	//如果没有找到对应信息，找到一个空节点将密钥信息写入
	int i = 0;
	NodeSHMInfo tmpNodeInfo;//空节点 默认构造函数已经初始化了
	for ( i = 0; i < m_maxNode; ++i) {
		pNode = pAddr + i;
		//与空节点对比
		if (memcmp(&tmpNodeInfo, pNode, sizeof(pNode))==0) {
			ret = 0;
			memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			cout << "写数据成功：在新的节点上添加了数据" << endl;
			break;
		}		
	}
	if (i == m_maxNode) {
		cout << "写数据失败： 没有新的节点可用了" << endl;
		ret = -1;
	}
	//共享内存与当前线程分离
	unmapShm();
	return ret;

}
NodeSHMInfo SecKeyShm::shmRead(string clientID, string serverID) {
	int ret = 0;
	//关联共享内存
	NodeSHMInfo* pAddr = NULL;
	//关联
	pAddr = static_cast<NodeSHMInfo*>(mapShm());
	if (pAddr == NULL) {
		cout << "关联共享内存失败！" << endl;
		return NodeSHMInfo();
	}
	cout << "关联共享内存成功" << endl;
	//遍历网点信息
	int i = 0;
	NodeSHMInfo info;
	NodeSHMInfo* pNode = NULL;
	//通过clientID  serverID 查找结点
	cout << "maxNode: " << m_maxNode << endl;
	for (int i = 0; i < m_maxNode; ++i) {
		pNode = pAddr + i;
		cout << i << endl;
		cout << pNode->clientID << ", " << clientID.data() << endl;
		cout << pNode->serverID << ", " << serverID.data() << endl;
		if ((strcmp(pNode->clientID, clientID.data()) == 0) &&
			(strcmp(pNode->serverID, serverID.data()) == 0)) {
			//找到节点信息，拷贝到传出参数
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