
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
	//创建tcpserver对象
	m_server = new TcpServer();
	m_server->setListen(m_info.port, NULL);
	//接收连接请求
	while (true) {
		TcpSocket* pSocket = m_server->acceptConnectSelect();
		if (!pSocket) {
			cout << "accept 错误：继续监听。。。" << endl;
			continue;
		}
		cout << "accept sucess!" << endl;
		//通信
		pthread_t tid;
		pthread_create(&tid, NULL, working, this);
		pthread_detach(tid);
		//将通信套接字放在容器中
		m_socket.insert(make_pair(tid, pSocket));
	}

}

void* working(void* arg)
{
	usleep(100);
	//得到通信套接字
	ServerOP* op = (ServerOP*)arg;
	//得到通信的套接字对象
	TcpSocket* pSocket = op->m_socket[pthread_self()];
	//接收数据-->编码之后
	string recvMsg = pSocket->recvMsg();

	//对接收的数据进行解码
	CodecFactory* factory = new RequestFactory(recvMsg);
	Codec* codec = factory->createCodec();
	RequestMsg* reqMsg = (RequestMsg*)codec->decodeMsg();

	//根据cmd判断客户端请求
	string str;
	switch (reqMsg->cmdtype())
	{
	case 1:
		//密钥协商
		str = op->seckeyArgee(reqMsg);
		break;
	case 2:
		//密钥校验
		break;
	case 3:
		//密钥注销
		break;
	default:
		break;
	}
	pSocket->sendMsg(str);
	cout << "数据已经发送给了客户端。。。" << endl;

	pSocket->disConnect();
	//将psoket对象从map中删除
	op->m_socket.erase(pthread_self());
	//资源释放
	delete factory;
	delete codec;
	delete pSocket;

	return nullptr;
}
//密钥协商
string ServerOP::seckeyArgee(RequestMsg* msg)
{
	//1将得到的公钥写入服务器磁盘
	//写文件，文件名：客户端ID
	cout << "接收的公钥文件内容为：" <<  msg->data() << endl;
	//根据客户端id组合pem成 想要生成的文件
	string fileName = msg->clientid() + ".pem";
	//将文件流刷入磁盘中
	ofstream ofs(fileName);
	ofs << msg->data();
	//数据刷到磁盘flush()
	ofs.close();

	cout << "磁盘上生成了公钥文件。。。" << endl;
	RSACrypto rsa(fileName, false);
	//2校验签名
	//保证rsa对象中公钥是可用的
	RespondInfo info;
	Hash sha1(T_SHA1);
	sha1.addData(msg->data());
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
//参数： num == 16 24 32
//改成枚举
string ServerOP::getRandStr(KeyLen num)
{	//以当前时间为种子
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
