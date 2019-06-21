#pragma once
#include "Codec.h"
#include <iostream>
#include "msg.pb.h"
using namespace std;

struct RespondInfo
{
	int status;
	int seckeyID;
	string clientID;
	string serverID;
	string data;
};


class RespondCodec :
	public Codec
{
public:
	RespondCodec();
	RespondCodec(std::string enc);
	RespondCodec(RespondInfo* info);
	void initMessage(std::string enc);
	void initMessage(RespondInfo* info);
	string encodeMsg();
	void* decodeMsg();
	~RespondCodec();

private:
	string m_encStr;
	RespondMsg m_msg;
};

