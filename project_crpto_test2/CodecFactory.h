#pragma once
#include "Codec.h"

//´úÂë¹¤³§
class CodecFactory
{
public:
	CodecFactory();
	virtual Codec* createCodec();

	virtual ~CodecFactory();
};

