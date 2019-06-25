﻿#include "RespondCodec.h"
#include <iostream>

using namespace std;



RespondCodec::RespondCodec()
{
}

RespondCodec::RespondCodec(std::string enc)
{
	initMessage(enc);
}

RespondCodec::RespondCodec(RespondInfo* info)
{
	initMessage(info);
}

void RespondCodec::initMessage(std::string enc)
{
	m_encStr = enc;
}

void RespondCodec::initMessage(RespondInfo* info)
{
	m_msg.set_status(info->status);
	m_msg.set_seckeyid(info->seckeyID);
	m_msg.set_clientid(info->clientID);
	m_msg.set_serverid(info->serverID);
	m_msg.set_data(info->data);
}

string RespondCodec::encodeMsg()
{
	string output;
	m_msg.SerializeToString(&output);
	return output;
}

void* RespondCodec::decodeMsg()
{
	cout << "1111111111111111111111111" << endl;

	m_msg.ParseFromString(m_encStr);
	return &m_msg;
}


RespondCodec::~RespondCodec()
{

}
