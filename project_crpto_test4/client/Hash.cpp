#include "Hash.h"

Hash::Hash(HashType type)
{
	this->m_type = type;
	switch(type) {
		case HashType::T_MD5:
			md5Init();
			break;
		case HashType::T_SHA1:
			sha1Init();
			break;
		case HashType::T_SHA224:
			sha224Init();
			break;
		case HashType::T_SHA256:
			sha256Init();
			break;
		case HashType::T_SHA384:
			sha384Init();
			break;
		case HashType::T_SHA512:
			sha512Init();
			break;
		default:
			md5Init();
			break;
	}
}

Hash::~Hash()
{
}

void Hash::addData(string data)
{
	switch (m_type)
	{
	case T_MD5:
		md5AddData(data.data());
		break;
	case T_SHA1:
		sha1AddData(data.data());
		break;
	case T_SHA224:
		sha224AddData(data.data());
		break;
	case T_SHA256:
		sha256AddData(data.data());
		break;
	case T_SHA384:
		sha384AddData(data.data());
		break;
	case T_SHA512:
		sha512AddData(data.data());
		break;
	default:
		md5AddData(data.data());
		break;
	}
}

string Hash::result()
{
	string result = string();
	switch (m_type)
	{
	case T_MD5:
		result = md5Result();
		break;
	case T_SHA1:
		result = SHA1Result();
		break;
	case T_SHA224:
		result = SHA224Result();
		break;
	case T_SHA256:
		result = SHA256Result();
		break;
	case T_SHA384:
		result = SHA384Result();
		break;
	case T_SHA512:
		result = SHA512Result();
		break;
	default:
		result = md5Result();
		break;
	}
	return result;
}

string Hash::md5Result()
{					//16
	unsigned char md[MD5_DIGEST_LENGTH];
	char res[MD5_DIGEST_LENGTH * 2 + 1];
	MD5_Final(md, &m_md5);
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		sprintf(&res[i * 2], "%02x", md[i]);
	}
	return string(res, MD5_DIGEST_LENGTH * 2 + 1);
}

string Hash::SHA1Result()
{
	unsigned char md[SHA_DIGEST_LENGTH];
	char res[SHA_DIGEST_LENGTH * 2 + 1];
	SHA1_Final(md, &m_sha1);
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
		sprintf(&res[i], "%02x", md[i]);
	}
	return string(res, SHA_DIGEST_LENGTH * 2 + 1);
}

string Hash::SHA224Result()
{
	unsigned char md[SHA224_DIGEST_LENGTH];
	char res[SHA224_DIGEST_LENGTH * 2 + 1];
	SHA224_Final(md, &m_sha224);
	for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i) {
		sprintf(&res[i], "%02x", md[i]);
	}
	return string(res, SHA224_DIGEST_LENGTH * 2 + 1);
}

string Hash::SHA256Result()
{
	unsigned char md[SHA256_DIGEST_LENGTH];
	char res[SHA256_DIGEST_LENGTH * 2 + 1];
	SHA256_Final(md, &m_sha256);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
		sprintf(&res[i], "%02x", md[i]);
	}
	return string(res, SHA256_DIGEST_LENGTH * 2 + 1);
}

string Hash::SHA384Result()
{
	unsigned char md[SHA384_DIGEST_LENGTH];
	char res[SHA384_DIGEST_LENGTH * 2 + 1];
	SHA384_Final(md, &m_sha384);
	for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i){
		sprintf(&res[i], "%02x", md[i]);
	}
	return string(res, SHA384_DIGEST_LENGTH * 2 + 1);
}

string Hash::SHA512Result()
{
	unsigned char md[SHA512_DIGEST_LENGTH];
	char res[SHA512_DIGEST_LENGTH * 2 + 1];
	SHA512_Final(md, &m_sha512);
	for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
		sprintf(&res[i], "%02x", md[i]);
	}
	return string(res, SHA512_DIGEST_LENGTH * 2 + 1);
}
