﻿#pragma once
#include <string>
#include "CodecFactory.h"
#include "Codec.h"
#include "RequestCodec.h"
#include <iostream>
using namespace std;

class RequestFactory :
	public CodecFactory
{
public:
	//RequestFactory();
	RequestFactory(string enc);
	RequestFactory(RequestInfo* info);
	Codec* createCodec();
	~RequestFactory();

private:
	bool m_flag;
	string m_encStr;
	RequestInfo* m_info;
};

