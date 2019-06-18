
#include "ServerOP.h"
#include "RespondFactory.h"
#include "RequestFactory.h"
#include "RsaCrypto.h"
#include "Hash.h"
#include <pthread.h>
#include <json/json.h>//json c++�ļ�
#include <fstream>
#include <string.h>
#include <string>
#include <ctime>
#include <unistd.h>



ServerOP::ServerOP(string jsonfile)
{
	//���������ļ�
	//1�Ӵ����ж��ļ�
	ifstream ifs(jsonfile);
	Json::Reader jsonReader;
	Json::Value jsonValue;
	jsonReader.parse(ifs, jsonValue);
	//�����ݴ�Value��ȡ��
	m_info.dbPasswd = jsonValue["dbPasswd"].asString();
	m_info.dbSid = jsonValue["dbSID"].asString();
	m_info.dbUser = jsonValue["dbUser"].asString();
	m_info.maxNode = jsonValue["maxnode"].asInt();
	m_info.port = jsonValue["port"].asInt();
	m_info.serverID = jsonValue["serverID"].asString();
	m_info.shmKey = jsonValue["shmkey"].asString();
}

ServerOP::~ServerOP()
{
	delete m_server;
}

void ServerOP::startServer()
{
	//����tcpserver����
	m_server = new TcpServer();
	m_server->setListen(m_info.port, NULL);
	//������������
	while (true) {
		TcpSocket* pSocket = m_server->acceptConnectSelect();
		if (!pSocket) {
			cout << "accept ���󣺼�������������" << endl;
			continue;
		}
		cout << "accept sucess!" << endl;
		//ͨ��
		pthread_t tid;
		pthread_create(&tid, NULL, working, this);
		pthread_detach(tid);
		//��ͨ���׽��ַ���������
		m_socket.insert(make_pair(tid, pSocket));
	}

}

void* working(void* arg)
{
	usleep(100);
	//�õ�ͨ���׽���
	ServerOP* op = (ServerOP*)arg;
	//�õ�ͨ�ŵ��׽��ֶ���
	TcpSocket* pSocket = op->m_socket[pthread_self()];
	//��������-->����֮��
	string recvMsg = pSocket->recvMsg();

	//�Խ��յ����ݽ��н���
	CodecFactory* factory = new RequestFactory(recvMsg);
	Codec* codec = factory->createCodec();
	RequestMsg* reqMsg = (RequestMsg*)codec->decodeMsg();

	//����cmd�жϿͻ�������
	string str;
	switch (reqMsg->cmdtype())
	{
	case 1:
		//��ԿЭ��
		str = op->seckeyArgee(reqMsg);
		break;
	case 2:
		//��ԿУ��
		break;
	case 3:
		//��Կע��
		break;
	default:
		break;
	}
	pSocket->sendMsg(str);
	cout << "�����Ѿ����͸��˿ͻ��ˡ�����" << endl;

	pSocket->disConnect();
	//��psoket�����map��ɾ��
	op->m_socket.erase(pthread_self());
	//��Դ�ͷ�
	delete factory;
	delete codec;
	delete pSocket;

	return nullptr;
}
//��ԿЭ��
string ServerOP::seckeyArgee(RequestMsg* msg)
{
	//1���õ��Ĺ�Կд�����������
	//д�ļ����ļ������ͻ���ID
	cout << "���յĹ�Կ�ļ�����Ϊ��" <<  msg->data() << endl;
	//���ݿͻ���id���pem�� ��Ҫ���ɵ��ļ�
	string fileName = msg->clientid() + ".pem";
	//���ļ���ˢ�������
	ofstream ofs(fileName);
	ofs << msg->data();
	//����ˢ������flush()
	ofs.close();

	cout << "�����������˹�Կ�ļ�������" << endl;
	RSACrypto rsa(fileName, false);
	//2У��ǩ��
	//��֤rsa�����й�Կ�ǿ��õ�
	RespondInfo info;
	Hash sha1(T_SHA1);
	sha1.addData(msg->data());
	bool b1 = rsa.rsaVertifySign(sha1.result(), msg->sign());
	if (b1 == false) {
		cout << "ǩ��У��ʧ�ܡ�����" << endl;
		info.status = false;
	}
	else {
		info.status = true;
		cout << "ǩ��У��ɹ�������" << endl;
		//3����һ������ַ��� 16/24/32λ �ԳƼ��ܵ���Կ
		string seckey = getRandStr(LenNormal);
		//4�ظ����ݡ����ṹ��
		//5���ݳ�ʼ��--����Կʹ�ù�Կ����
		info.clientID = msg->clientid();
		info.serverID = m_info.serverID;
		//��Ҫ���ݲ�����Ŀǰֱ�Ӹ�ֵ����
		info.seckeyID = 123;//�ԳƼ��ܵ���ԿID
		//�ԳƼ��ܵ���Կ��ʹ�ù�Կ����
		cout << "�ԳƼ�����Կ��" << seckey << endl;
		info.data = rsa.rsaPubKeyEncrypt(seckey);
	}
	//�������л�����
	CodecFactory* factory = new RespondFactory(&info);
	Codec* codec = factory->createCodec();
	//6ͨ���������������������-->����
	string sendMsg = codec->encodeMsg();
	//7�ͷ���Դ
	delete factory;
	delete codec;

	return sendMsg;
}
//������ num == 16 24 32
//�ĳ�ö��
string ServerOP::getRandStr(KeyLen num)
{	//�Ե�ǰʱ��Ϊ����
	srand((unsigned int)time(NULL));
	string retStr = string();
	char* buf = "~^@#$&*()_+=-{}[];':";
	for (int i = 0; i < num; ++i) {
		int flag = rand() % 4;
		switch (flag)
		{
		case 0:
			retStr.append(1, rand() % 10 + '0');
			break;
		case 1:
			retStr.append(1, rand() % 26 + 'a');
		case 2:
			retStr.append(1, rand() % 26 + 'A');
		default:
			retStr.append(1, buf[rand() % strlen(buf)]);
			break;
		}
	}
	return string();
}
