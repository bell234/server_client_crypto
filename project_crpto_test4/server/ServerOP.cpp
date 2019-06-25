#include "ServerOP.h"
#include "RespondFactory.h"
#include "RequestFactory.h"
#include "RsaCrypto.h"
#include "Hash.h"
#include <pthread.h>
#include <json/json.h>//json c++文件
#include <fstream>
#include <string.h>
#include <string>
#include <ctime>
#include <unistd.h>


ServerOP::ServerOP(string jsonfile)
{
	//加载配置文件
	//1从磁盘中读文件
	ifstream ifs(jsonfile);
	Json::Reader jsonReader;
	Json::Value jsonValue;
	jsonReader.parse(ifs, jsonValue);
	//将数据从Value中取出
	cout << "服务器读取json文件初始化" << endl;
	m_info.dbPasswd = jsonValue["dbPasswd"].asString();
	m_info.dbSid = jsonValue["dbSID"].asString();
	m_info.dbUser = jsonValue["dbUser"].asString();
	m_info.maxNode = jsonValue["maxnode"].asInt();
	m_info.port = jsonValue["port"].asInt();
	m_info.serverID = jsonValue["serverID"].asString();
	m_info.shmKey = jsonValue["shmkey"].asString();
	cout << "1服务器读取内容为 " << m_info.port << endl << m_info.serverID << endl
		<< m_info.shmKey << endl << m_info.maxNode << endl << m_info.dbUser << endl
		<< m_info.dbSid << endl << m_info.dbPasswd << endl;
}

ServerOP::~ServerOP()
{
	delete m_server;
}

void ServerOP::startServer()
{
	//创建tcpserver对象
	cout << "2开始监听操作" << endl;
	m_server = new TcpServer();
	m_server->setListen(m_info.port, NULL);
	//接收连接请求
	cout << "接受连接请求" << endl;
	while (true) {
		TcpSocket* pSocket = m_server->acceptConnectSelect();
		if (pSocket == NULL) {
			cout << "accept 错误：继续监听。。。" << endl;
			continue;
		}
		cout << "一次连接请求成功accept sucess!" << endl;
		//通信
		cout << "3准备通信" << endl;
		pthread_t tid;
		cout << "4创建线程处理通信动作" << endl;
		pthread_create(&tid, NULL, working, this);
		cout << "设置线程分离" << endl;
		pthread_detach(tid);
		//将通信套接字放在容器中
		cout << "5将套接字插入容器中" << endl;
		m_socket.insert(make_pair(tid, pSocket));
	}

}

void* working(void* arg)
{
	cout << "先睡100ms" << endl;
	usleep(100);//此处目的是为了map方便插入键值对
	//得到通信套接字
	ServerOP* op = (ServerOP*)arg;
	//得到通信的套接字对象
	
	TcpSocket* pSocket = op->m_socket[pthread_self()];
	cout << "6得到socket对象" << op->m_socket[pthread_self()] << endl;
	//接收数据-->编码之后
	string recvMsg = pSocket->recvMsg();
	cout << "7接收的数据内容为" << endl <<  recvMsg << endl;

	//对接收的数据进行解码
	cout << "8接受数据解码" << endl;
	CodecFactory* factory = new RequestFactory(recvMsg);
	Codec* codec = factory->createCodec();
	RequestMsg* reqMsg = (RequestMsg*)codec->decodeMsg();//通过网络接口传来结构
	//根据cmd判断客户端请求
	string str;
	cout << "9客户请求命令为 " << reqMsg->cmdtype() << endl;
	cout << "客户端的reqMsg 信息为： " << endl <<  reqMsg->data() << endl;
	switch (reqMsg->cmdtype())
	{
	case 1:
		//密钥协商
		cout << "10密钥协商功能实现" << endl;
		str = op->seckeyArgee(reqMsg);
		break;
	case 2:
		//密钥校验
	
		break;
	case 3:
		//密钥注销
		break;
	default:
		//str = op->seckeyArgee(reqMsg);
		break;
	}
	cout << "发送数据给客户端内容为" << endl << str <<  endl;
	pSocket->sendMsg(str);
	cout << "数据已经发送给了客户端。。。" << endl;

	pSocket->disConnect();
	//将psoket对象从map中删除
	op->m_socket.erase(pthread_self());
	//资源释放
	delete factory;
	delete codec;
	delete pSocket;

	return NULL;
}
//密钥协商
string ServerOP::seckeyArgee(RequestMsg* msg)
{
	//1将得到的公钥写入服务器磁盘
	//写文件，文件名：客户端ID
	cout << "接收的公钥文件内容为：" <<  msg->data() << endl;//base64
	//根据客户端id组合pem成 想要生成的文件
	string fileName = msg->clientid() + ".pem";
	//将文件流刷入磁盘中
	ofstream ofs(fileName);
	ofs << msg->data();
	//数据刷到磁盘flush()
	ofs.close();

	cout << "磁盘上生成了公钥文件。。。" << endl;
	cout << "fileName" << endl <<  fileName << endl;
	RSACrypto rsa(fileName, false);//false为公钥
	//2校验签名
	//保证rsa对象中公钥是可用的
	RespondInfo info;
	Hash sha1(T_SHA1);
	sha1.addData(msg->data());
	sha1.result();

#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/buffer.h>
#include <string.h>
#include <iostream>
	int keyLen = RSA_size(m_publicKey);
	char* sign = fromBase64(signData);
	int ret = RSA_verify(level, (const unsigned char*)data.data(), data.size(),
		(const unsigned char*)sign, keyLen, m_publicKey);
	cout << "验证签名返回值ret = " << ret << endl;//0
	//ret = 1;
	delete[] sign;
	
	


	bool b1 = rsa.rsaVertifySign(sha1.result(), msg->sign());
	if (b1 == false) {
		cout << "签名校验失败。。。" << endl;
		info.status = false;
	}
	else {
		info.status = true;
		cout << "签名校验成功。。。" << endl;
		//3生成一个随机字符串 16/24/32位 对称加密的密钥
		string seckey = getRandStr(LenNormal);
		//4回复数据—》结构体
		//5数据初始化--》密钥使用公钥加密
		info.clientID = msg->clientid();
		info.serverID = m_info.serverID;
		//需要数据操作，目前直接赋值即可
		info.seckeyID = 123;//对称加密的密钥ID
		//对称加密的密钥，使用公钥加密
		cout << "对称加密密钥：" << seckey << endl;
		info.data = rsa.rsaPubKeyEncrypt(seckey);
	}
	//生成序列化对象
	CodecFactory* factory = new RespondFactory(&info);
	Codec* codec = factory->createCodec();
	//6通过工厂函数创建编码对象-->编码
	string sendMsg = codec->encodeMsg();
	//7释放资源
	delete factory;
	delete codec;

	return sendMsg;
}
//获取随机字符串
string ServerOP::getRandStr(KeyLen num)
{	//以当前时间为种子
	srand(time(NULL));//unsigned int
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
	return retStr;
}

//密钥验证
string ServerOP::seckeyVertify(RequestMsg* msg)
{
	return string();
}
//密钥注销
string ServerOP::seckeyLogout(RequestMsg* msg)
{
	return string();
}
//参数： num == 16 24 32
//改成枚举

