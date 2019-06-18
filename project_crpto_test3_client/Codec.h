#pragma once
#include <iostream>
#include <string>

using namespace std;

class Codec
{
public:
	Codec();
	//º”√‹
	virtual string encodeMsg();
	//Ω‚√‹
	virtual void* decodeMsg();
	virtual ~Codec();
};

