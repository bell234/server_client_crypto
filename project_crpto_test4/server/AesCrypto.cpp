#include "AesCrypto.h"


//AES_KEY m_encKey;
//AES_KEY m_decKey;
//string m_key;


//对称加密
//==>   m_key 原始密钥字符串
//		m_encKey加密密钥
//		m_decKey解密密钥
AesCrypto::AesCrypto(string key)//密钥字符串
{
	if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
		const unsigned char* aesKey = (const unsigned char*)key.data();
		//加密/解密         密钥字符串   密钥长度    传出参数：保存设置密钥信息
		AES_set_encrypt_key(aesKey, key.size() * 8, &m_encKey);
		AES_set_decrypt_key(aesKey, key.size() * 8, &m_decKey);
		this->m_key = key;//原始密钥字符串
	}
	
}

AesCrypto::~AesCrypto()
{

}

string AesCrypto::aesCBCEncrypt(string text)
{
							//1
	return aesCrypto(text, AES_ENCRYPT);
}

string AesCrypto::aesCBCDecrypt(string encStr)
{
							//0
	return aesCrypto(encStr, AES_DECRYPT);
}
//根据传来的数据判定是加密还是解密
string AesCrypto::aesCrypto(string data, int crypto)
{	//判断是加密密钥还是解密密钥
	AES_KEY* key = crypto == AES_ENCRYPT ? &m_encKey : &m_decKey;
	unsigned char ivec[AES_BLOCK_SIZE] = { 0 };//16
	int length = data.size() + 1;
	if (length % 16) {
		length = (length / 16 + 1) * 16;
	}
	char* out = new char[length];
	generateIvec(ivec);//逆置原始数据==>密钥

	//CBC方式加密/解密
	//1要加密/解密的数据    2密文/铭文   3数据长度
	//4密钥
	//5ivec初始化字符串, 和第一个明文分组进行位运算, 	
	//	取16个字符加解密的时候保证该数组中的数据完全相同
	//6加/解密标志
	AES_cbc_encrypt((const unsigned char*)data.c_str(),
		(unsigned char*)out, length, key, ivec, crypto);
	string retStr = string(out);
	delete(out);

	return string(retStr);
}

void AesCrypto::generateIvec(unsigned char* ivec)
{
						//16
	for (int i = 0; i < AES_BLOCK_SIZE; ++i) {
				//原始密钥字符串
		ivec[i] = m_key.at(AES_BLOCK_SIZE - i - 1);
	}
}
