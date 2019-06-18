#include "RequestFactory.h"



RequestFactory::RequestFactory()
{
}


RequestFactory::RequestFactory(string enc)
{
	m_flag = false;
	m_encStr = enc;
}
RequestFactory::RequestFactory(RequestInfo *info)
{
	m_flag = true;
	m_info = info;
}

Codec* RequestFactory::createCodec()
{
	Codec* codec = NULL;
	if (m_flag) {
		codec = new RequestCodec(m_info);
	}
	else {
		codec = new RequestCodec(m_encStr);
	}

	return codec;
}

RequestFactory::~RequestFactory()
{
}
