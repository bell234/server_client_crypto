#include "ServerOP.h"
#include "RequestCodec.h"
#include "RequestFactory.h"
#include <pthread.h>
#include <json/json.h>//json c++�ļ�
#include <fstream>
#include <unistd.h>
#include <string.h>
#include "RsaCrypto.h"
#include "Hash.h"


ServerOP::ServerOP()
{
	
}


ServerOP::ServerOP(string jsonfile)
{
	//���������ļ�
	//1�Ӵ����ж��ļ�
	ifstream ifs(jsonfile);
	Json::Reader jsonReader;
	Json::Value jsonValue;
	jsonReader.parse(ifs, jsonValue);
	//�����ݴ�Value��ȡ��
	m_info.dbPassWd = jsonValue["dbPasswd"].asString();
	m_info.dbSid = jsonValue["dbSID"].asString();
}

ServerOP::~ServerOP()
{
}

void ServerOP::startServer()
{
}

string ServerOP::seckeyArgee(RequestMsg* msg)
{
	return string();
}

string ServerOP::getRandStr(KeyLen num)
{
	return string();
}

void* working(void* arg)
{
	return nullptr;
}
