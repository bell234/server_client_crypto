#include "AesCrypto.h"


//AES_KEY m_encKey;
//AES_KEY m_decKey;
//string m_key;


//�ԳƼ���
//==>   m_key ԭʼ��Կ�ַ���
//		m_encKey������Կ
//		m_decKey������Կ
AesCrypto::AesCrypto(string key)//��Կ�ַ���
{
	if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
		const unsigned char* aesKey = (const unsigned char*)key.data();
		//����/����         ��Կ�ַ���   ��Կ����    ��������������������Կ��Ϣ
		AES_set_encrypt_key(aesKey, key.size() * 8, &m_encKey);
		AES_set_decrypt_key(aesKey, key.size() * 8, &m_decKey);
		this->m_key = key;//ԭʼ��Կ�ַ���
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
//���ݴ����������ж��Ǽ��ܻ��ǽ���
string AesCrypto::aesCrypto(string data, int crypto)
{	//�ж��Ǽ�����Կ���ǽ�����Կ
	AES_KEY* key = crypto == AES_ENCRYPT ? &m_encKey : &m_decKey;
	unsigned char ivec[AES_BLOCK_SIZE] = { 0 };//16
	int length = data.size() + 1;
	if (length % 16) {
		length = (length / 16 + 1) * 16;
	}
	char* out = new char[length];
	generateIvec(ivec);//����ԭʼ����==>��Կ

	//CBC��ʽ����/����
	//1Ҫ����/���ܵ�����    2����/����   3���ݳ���
	//4��Կ
	//5ivec��ʼ���ַ���, �͵�һ�����ķ������λ����, 	
	//	ȡ16���ַ��ӽ��ܵ�ʱ��֤�������е�������ȫ��ͬ
	//6��/���ܱ�־
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
				//ԭʼ��Կ�ַ���
		ivec[i] = m_key.at(AES_BLOCK_SIZE - i - 1);
	}
}
