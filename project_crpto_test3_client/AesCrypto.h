#pragma once
#include <iostream>
#include <openssl/aes.h>
#include <string>
using namespace std;

//�ԳƼ���
class AesCrypto {
public:
	//��ʹ��16byte 24 32λ��Կ
	AesCrypto(string key);
	~AesCrypto();
	//���� text�ı�
	string aesCBCEncrypt(string text);
	//���� �����ܹ���text�ı��� encStr
	string aesCBCDecrypt(string encStr);
private:
	string aesCrypto(string data, int crypto);
	//��������
	void generateIvec(unsigned char* ivec);

private:
	AES_KEY m_encKey;
	AES_KEY m_decKey;
	string m_key;
};

