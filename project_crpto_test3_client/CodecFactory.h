#pragma once
#include "Codec.h"

//���빤��
class CodecFactory
{
public:
	CodecFactory();
	virtual Codec* createCodec();

	virtual ~CodecFactory();
};

