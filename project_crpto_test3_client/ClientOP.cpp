
#include "ClientOP.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include "TcpSocket.h"
#include <json/json.h>//json c++�ļ�
#include <fstream>
#include "Hash.h"
#include <sstream>
#include <string.h>
#include <string>

ClientOP::ClientOP(string fileName)
{
	//1�����̵�JSON�ļ�
	//value-->m_info
	//����������-->���ļ�ifstream
	ifstream ifs(fileName);
	//ʹ��jsoncpp reader -->value
	Json::Reader jsonReader;
	Json::Value jsonValue;
	jsonReader.parse(ifs, jsonValue);
	//�����ݴ�value�ж�����
	m_info.clientID = jsonValue["clientID"].asString();
	m_info.serverID = jsonValue["serverID"].asString();
	m_info.serverIP = jsonValue["serverIP"].asString();
	m_info.shmKey = jsonValue["shmKey"].asString();

	m_info.port = jsonValue["serverPort"].asInt();
	m_info.maxNode = jsonValue["maxNode"].asInt();
}

ClientOP::~ClientOP()
{
	
}
/*
	׼�����ԳƼ��ܵ���Կ��ͨ���ǶԳƼ��ܵķ�ʽ�����Կ����
		���ݷ��ͣ� ͨ��protobuf���л�֮��Ľṹ��A
		���ݽ��գ� ͨ��protobuf���л�֮��Ľṹ��B
*/
bool ClientOP::seckeyAgree()
{
	/*
		1׼�����ݣ������л�
		���л��ദ��ʲô���ͣ���׼��ʲô���͵�����
		���л��������ͨ��--��������Ĺ�������������
	*/
	//1������������
	RequestInfo reqInfo;
	reqInfo.clientID = m_info.clientID;
	reqInfo.serverID = m_info.serverID;
	reqInfo.cmd = 1;

	//2����rsa��Կ��
	RSACrypto crypto;
	crypto.generateKeyFile(1024);//128byte
	//��ȡǩ������--����rsa��Կǩ��
	//3�������ļ��еĹ�Կ����
	ifstream ifs("public.pem");
	//ͨ���ļ���--���ַ�����
	stringstream strBuf;
	strBuf << ifs.rdbuf();
	//������ϣ����
	Hash hash(T_SHA1);
	hash.addData(strBuf.str());
	string sh1 = hash.result();
	reqInfo.sign = crypto.rsaSign(sh1);
	cout << "ǩ���ɹ�" << "��Կ��Ϣ" << endl;
	//��ȡ�����������͵����� rsa��Կ
	reqInfo.data = strBuf.str();
	cout << reqInfo.data << endl;
	CodecFactory* factory = new RequestFactory(&reqInfo);
	Codec* codec = factory->createCodec();
	string encodeMsg = codec->encodeMsg();
	cout << "���ݱ�����ɡ�������" << endl;
	delete factory;
	delete codec;

	//������֮������ݷ���
	//���ӷ�����
	TcpSocket* socket = new TcpSocket();
	//����
	socket->connectToHost(m_info.serverIP, m_info.port);
	//��������
	socket->sendMsg(encodeMsg);
	//�ȴ���������
	string recvMsg = socket->recvMsg();

	//3����������ظ�������
	//31�����������ݶ���
	factory = new RespondFactory(recvMsg);
	codec = factory->createCodec();
	RespondMsg* resMsg = (RespondMsg*)codec->decodeMsg();

	//4�ж�״��
	bool ret = true;
	if (!resMsg->status()) {
		cout << "��������������ʧ�ܡ�����" << endl;
		ret = false;
	}
	else {
		//ͨ��˽Կ��data�е����ݽ��ܳ���
		string aesKey = crypto.rsaPriKeyDecrypt(resMsg->data());
		cout << "�ԳƼ�����Կ�� " << aesKey << endl;
		//6��aeskey �洢�������ڴ���
	}
	//7�ͷ��ڴ�
	delete factory;
	delete codec;
	//8�ر��׽���
	socket->disConnect();
	delete socket;


	return ret;
}

void ClientOP::seckeyCheck()
{
}

void ClientOP::seckeyLogout()
{
}




