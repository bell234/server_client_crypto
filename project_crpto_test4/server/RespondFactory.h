#pragma once
#include "CodecFactory.h"
#include "RespondCodec.h"
#include <iostream>
#include <string>

using namespace std;

class RespondFactory :
	public CodecFactory
{
public:
	RespondFactory();
	RespondFactory(string enc);
	RespondFactory(RespondInfo* info);
	Codec* create();

	~RespondFactory();
private:
	bool m_flag;
	string m_encStr;
	RespondInfo* m_info;
};

