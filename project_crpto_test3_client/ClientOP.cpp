
#include "ClientOP.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include "TcpSocket.h"
#include <json/json.h>//json c++文件
#include <fstream>
#include "Hash.h"
#include <sstream>
#include <string.h>
#include <string>

ClientOP::ClientOP(string fileName)
{
	//1读磁盘的JSON文件
	//value-->m_info
	//创建流对象-->读文件ifstream
	ifstream ifs(fileName);
	//使用jsoncpp reader -->value
	Json::Reader jsonReader;
	Json::Value jsonValue;
	jsonReader.parse(ifs, jsonValue);
	//将数据从value中读出来
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
	准备：对称加密的密钥，通过非对称加密的方式完成密钥交换
		数据发送： 通过protobuf序列化之后的结构体A
		数据接收： 通过protobuf序列化之后的结构体B
*/
bool ClientOP::seckeyAgree()
{
	/*
		1准备数据，并序列化
		序列化类处理什么类型，就准备什么类型的数据
		序列化的类对象通过--》工厂类的工厂函数创建的
	*/
	//1创建工厂对象
	RequestInfo reqInfo;
	reqInfo.clientID = m_info.clientID;
	reqInfo.serverID = m_info.serverID;
	reqInfo.cmd = 1;

	//2生成rsa密钥对
	RSACrypto crypto;
	crypto.generateKeyFile(1024);//128byte
	//获取签名数据--》对rsa公钥签名
	//3将磁盘文件中的公钥读出
	ifstream ifs("public.pem");
	//通过文件流--》字符串流
	stringstream strBuf;
	strBuf << ifs.rdbuf();
	//创建哈希对象
	Hash hash(T_SHA1);
	hash.addData(strBuf.str());
	string sh1 = hash.result();
	reqInfo.sign = crypto.rsaSign(sh1);
	cout << "签名成功" << "公钥信息" << endl;
	//获取给服务器发送的数据 rsa公钥
	reqInfo.data = strBuf.str();
	cout << reqInfo.data << endl;
	CodecFactory* factory = new RequestFactory(&reqInfo);
	Codec* codec = factory->createCodec();
	string encodeMsg = codec->encodeMsg();
	cout << "数据编码完成。。。。" << endl;
	delete factory;
	delete codec;

	//将解码之后的数据发送
	//连接服务器
	TcpSocket* socket = new TcpSocket();
	//连接
	socket->connectToHost(m_info.serverIP, m_info.port);
	//发送数据
	socket->sendMsg(encodeMsg);
	//等待接收数据
	string recvMsg = socket->recvMsg();

	//3解码服务器回复的数据
	//31创建解码数据对象
	factory = new RespondFactory(recvMsg);
	codec = factory->createCodec();
	RespondMsg* resMsg = (RespondMsg*)codec->decodeMsg();

	//4判断状体
	bool ret = true;
	if (!resMsg->status()) {
		cout << "服务器处理请求失败。。。" << endl;
		ret = false;
	}
	else {
		//通过私钥将data中的数据解密出来
		string aesKey = crypto.rsaPriKeyDecrypt(resMsg->data());
		cout << "对称加密密钥： " << aesKey << endl;
		//6将aeskey 存储到共享内存中
	}
	//7释放内存
	delete factory;
	delete codec;
	//8关闭套接字
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




