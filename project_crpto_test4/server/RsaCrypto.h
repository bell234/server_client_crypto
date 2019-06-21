#pragma once
#include <iostream>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

enum SignLevel
{
	Level1 = NID_md5,
	Level2 = NID_sha1,
	Level3 = NID_sha224,
	Level4 = NID_sha256,
	Level5 = NID_sha384,
	Level6 = NID_sha512
};
//Cryptographic
class RSACrypto
{
public:
	RSACrypto();
	RSACrypto(string fileName, bool isPrivate = true);
	~RSACrypto();

	//ͨ�������ַ����õ���Կ
	void parseKeyString(string keystr, bool pubkey = true);
	//����RSA��Կ��
	void generateKeyFile(int bits, string pub = "public.pem", string pri = "private.pem");
	//��Կ����
	string rsaPubKeyEncrypt(string data);
	//˽Կ����
	string rsaPriKeyDecrypt(string encData);
	//ʹ��RSAǩ��
	string rsaSign(string data, SignLevel level = Level3);
	//ʹ��RSA��֤ǩ��
	bool rsaVertifySign(string data, string signData, SignLevel level = Level3);
private:
	string toBase64(const char* str, int len);
	//base64����
	char* fromBase64(string str);
	//��ȡ��Կ
	bool initPublicKey(string pubfile);
	//��ȡ˽Կ
	bool initPrivateKey(string prifile);

private:
	RSA* m_publicKey;//��Կ
	RSA* m_privateKey;//˽Կ
};
