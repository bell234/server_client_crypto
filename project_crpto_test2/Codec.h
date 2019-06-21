#pragma once
#include <iostream>
#include <string>

using namespace std;

class Codec
{
public:
	Codec();
	//加密
	virtual string encodeMsg();
	//解密
	virtual void* decodeMsg();
	virtual ~Codec();
};

