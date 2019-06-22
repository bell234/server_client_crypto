﻿#pragma once
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
	RespondCodec(string enc);
	RespondCodec(RespondInfo* info);
	void initMessage(string enc);
	void initMessage(RespondInfo* info);
	string encodeMsg();
	void* decodeMsg();
	~RespondCodec();

private:
	string m_encStr;
	RespondMsg m_msg;
};

