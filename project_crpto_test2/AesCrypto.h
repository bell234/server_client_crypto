#pragma once
#include <iostream>
#include <openssl/aes.h>
#include <string>
using namespace std;

//对称加密
class AesCrypto {
public:
	//可使用16byte 24 32位密钥
	AesCrypto(string key);
	~AesCrypto();
	//加密 text文本
	string aesCBCEncrypt(string text);
	//解密 被加密过的text文本， encStr
	string aesCBCDecrypt(string encStr);
private:
	string aesCrypto(string data, int crypto);
	//产生数组
	void generateIvec(unsigned char* ivec);

private:
	AES_KEY m_encKey;
	AES_KEY m_decKey;
	string m_key;
};

