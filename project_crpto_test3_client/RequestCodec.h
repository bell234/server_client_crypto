#pragma once
#include "Codec.h"
#include "msg.pb.h"
#include <iostream>
#include <string>
using namespace std;

struct RequestInfo
{
	int cmd;//对应的处理序号
	string clientID;
	string serverID;
	string sign;//签名
	string data;//请求信息
};

class RequestCodec : public Codec
{
public:
	RequestCodec();
	RequestCodec(string encstr);
	RequestCodec(RequestInfo* info);
	void initMessage(string encstr);
	void initMessage(RequestInfo* info);
	string encodeMsg();
	void* decodeMsg();
	~RequestCodec();

private:
	string m_encStr;
	RequestMsg m_msg;
};

