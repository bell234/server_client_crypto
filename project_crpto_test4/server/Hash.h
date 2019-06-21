#pragma once
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <iostream>
#include <string.h>
#include <string>

using namespace std;
//ʹ��ö�ٹ��ܹ�ϣ�㷨
enum HashType {
	T_MD5,
	T_SHA1,
	T_SHA224,
	T_SHA256,
	T_SHA384,
	T_SHA512
};
//��ϣ��
class Hash
{
public:
	//���ݴ�����ϣ������ϣ���
	Hash(HashType type);
	~Hash();

	void addData(string data);
	string result();
private:
	//md5
	//��ʼ������ ��������
	inline void md5Init() {
		MD5_Init(&m_md5);
	}
	//�������-->����md5����
	inline void md5AddData(const char* data) {
		//��ʼ�����  Ҫ����md5��������� �ַ�������
		MD5_Update(&m_md5, data, strlen(data));
	}
	//����Md5_Final(unsigned char* md, MD5_CTX *c)
	//����MD5��ɢ��ֵ
	string md5Result();

	//sha1
	inline void sha1Init() {
		SHA1_Init(&m_sha1);
	}
	inline void sha1AddData(const char* data) {
		SHA1_Update(&m_sha1, data, strlen(data));
	}
	string SHA1Result();
	//sha224
	inline void sha224Init() {
		SHA224_Init(&m_sha224);
	}
	inline void sha224AddData(const char* data) {
		SHA224_Update(&m_sha224, data, strlen(data));
	}
	string SHA224Result();
	//sha256
	inline void sha256Init() {
		SHA256_Init(&m_sha256);
	}
	inline void sha256AddData(const char* data) {
		SHA256_Update(&m_sha256, data, strlen(data));
	}
	string SHA256Result();
	//sha384
	inline void sha384Init() {
		SHA384_Init(&m_sha384);
	}
	inline void sha384AddData(const char* data) {
		SHA384_Update(&m_sha384, data, strlen(data));
	}
	string SHA384Result();
	//sha512
	inline void sha512Init() {
		SHA512_Init(&m_sha512);
	}
	inline void sha512AddData(const char* data) {
		SHA512_Update(&m_sha512, data, strlen(data));
	}
	string SHA512Result();


private:
	HashType m_type;

	MD5_CTX m_md5;
	SHA_CTX m_sha1;
	SHA256_CTX m_sha224;
	SHA256_CTX m_sha256;
	SHA512_CTX m_sha384;
	SHA512_CTX m_sha512;

};