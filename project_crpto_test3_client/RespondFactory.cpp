#include "RespondFactory.h"



RespondFactory::RespondFactory()
{
}

RespondFactory::RespondFactory(string enc)
{
	m_flag = false;
	m_encStr = enc;
}

RespondFactory::RespondFactory(RespondInfo* info)
{
	m_flag = true;
	m_info = info;
}

Codec* RespondFactory::create()
{
	Codec* codec = NULL;
	if (m_flag) {
		codec = new RespondCodec(m_info);
	}
	else {
		codec = new RespondCodec(m_encStr);
	}
	return codec;
}


RespondFactory::~RespondFactory()
{
}
