#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/buffer.h>
#include <string.h>
#include <iostream>

//	RSA* m_publicKey;//��Կ
//	RSA* m_privateKey;

//extern "C" {
//#include <openssl/applink.c>
//};


RSACrypto::RSACrypto() {
	//����һ���ڴ�
	this->m_privateKey = RSA_new();
	this->m_publicKey = RSA_new();
}
//���ݲ������ȷ����ʼ����Կ����˽Կ
RSACrypto::RSACrypto(string fileName, bool isPrivate) {
	this->m_privateKey = RSA_new();
	this->m_publicKey = RSA_new();
	if (isPrivate) {
		initPrivateKey(fileName);
	}
	else {
		initPublicKey(fileName);
	}
}
//�������� �ͷ��������Կ���ڴ� 
RSACrypto::~RSACrypto() {
	RSA_free(this->m_privateKey);
	RSA_free(this->m_publicKey);
}
//����Կ/˽Կ�ַ���������RSA������
void RSACrypto::parseKeyString(string keystr, bool pubkey)
{	//�ַ�������-->BIO������
	BIO* bio = BIO_new_mem_buf(keystr.data(), keystr.size());
	//��Կ�ַ���
	if (pubkey) {
		PEM_read_bio_RSAPublicKey(bio, &m_publicKey, NULL, NULL);
	}
	else {
		//˽Կ�ַ���
		PEM_read_bio_RSAPrivateKey(bio, &m_privateKey, NULL, NULL);
	}
	BIO_free(bio);
}

//����RSA��Կ��
void RSACrypto::generateKeyFile(int bits, string pub, string pri) {
	//1�����ڴ�
	RSA* rsa = RSA_new();

	BIGNUM* bignum = BN_new();
	//��ʼ��BIGNUM����
	BN_set_word(bignum, 4567887);

	//����RSA��Կ��
	RSA_generate_key_ex(rsa, bits, bignum, NULL);

	//����bio�ļ�����
	BIO* publicBIO = BIO_new_file(pub.data(), "w");
	//��Կ��pem��ʽд���ļ���
	PEM_write_bio_RSAPublicKey(publicBIO, rsa);
	//����������ˢ�µ�������
	BIO_flush(publicBIO);
	BIO_free(publicBIO);

	//����bio����
	BIO* privateBi0 = BIO_new_file(pri.data(), "w");
	//˽Կ��pem��ʽд���ļ���
	PEM_write_bio_RSAPrivateKey(privateBi0, rsa, NULL, NULL, 0, NULL, NULL);
	BIO_flush(privateBi0);
	BIO_free(privateBi0);

	//�õ���Կ��˽Կ
	//������rsa�еĹ�Կ/˽Կ��ȡ����
	m_privateKey = RSAPrivateKey_dup(rsa);
	m_publicKey = RSAPublicKey_dup(rsa);
	//�ͷ���Դ
	BN_free(bignum);
	RSA_free(rsa);
}
//��Կ����
string RSACrypto::rsaPubKeyEncrypt(string data) {
	cout << "�������ݳ��ȣ�" << data.size() << endl;
	//���㹫Կ����
	int keyLen = RSA_size(m_publicKey);
	cout << "pbulic key length is: ��Կ����Ϊ��" << keyLen << endl;
	//���ݹ�Կ���������ڴ�
	char* encode = new char[keyLen + 1];

	//ʹ�ù�Կ����
	int ret = RSA_public_decrypt(data.size(), (const unsigned char*)data.data(),
		(unsigned char*)encode, m_publicKey, RSA_PKCS1_PADDING);

	string retStr = string();
	if (ret >= 0) {
		//���ܳɹ�
		cout << "ret: " << ret << ", keyLen: " << keyLen << endl;
		retStr = toBase64(encode, ret);
	}
	else {
		ERR_print_errors_fp(stdout);
	}
	delete[] encode;
	return retStr;
}
//˽Կ����
string RSACrypto::rsaPriKeyDecrypt(string encData) {
	char* text = fromBase64(encData);
	//����˽Կ����
	//cout<<"�������ݳ��ȣ� "<<text.size()<<endl;
	int keyLen = RSA_size(m_privateKey);
	//���ݳ��������ڴ�
	char* decode = new char[keyLen + 1];

	//ʹ��˽Կ����
	int ret = RSA_private_decrypt(keyLen, (const unsigned char*)text,
		(unsigned char*)decode, m_privateKey, RSA_PKCS1_PADDING);

	string retStr = string();
	if (ret >= 0) {
		//���ܳɹ�
		retStr = string(decode, ret);
	}
	else {
		cout << "����ʧ��" << endl;
		ERR_print_errors_fp(stdout);
	}
	delete[] decode;
	delete[] text;
	return retStr;
}
//ʹ��RSAǩ��
string RSACrypto::rsaSign(string data, SignLevel level) {

	unsigned int len = 0;
	char* signBuf = new char[RSA_size(m_privateKey) + 1];
	memset(signBuf, 0, RSA_size(m_privateKey));
	//ǩ��									//data.length();?
	int ret = RSA_sign(level, (const unsigned char*)data.data(), data.size(),
		(unsigned char*)signBuf, &len, m_privateKey);
	cout << "sign length is ǩ���ĳ��ȣ�" << len << endl;
	if (ret == -1) {
		ERR_print_errors_fp(stdout);
	}
	cout << "sign len: " << len << ", ret " << ret << endl;
	string retStr = toBase64(signBuf, len);
	delete[] signBuf;
	return retStr;
}

//ʹ��RSA��֤ǩ��
bool RSACrypto::rsaVertifySign(string data, string signData, SignLevel level ) {
	//��֤ǩ��
	int keyLen = RSA_size(m_publicKey);
	char* sign = fromBase64(signData);
	int ret = RSA_verify(level, (unsigned char*)data.data(), data.size(),
		(const unsigned char*)signData.data(), signData.size(), m_publicKey);
	delete[] sign;
	if (ret == -1) {
		ERR_print_errors_fp(stdout);
	}
	if (ret != 1) {
		return false;
	}
	return true;
}
string RSACrypto::toBase64(const char* str, int len)
{
	BIO* mem = BIO_new(BIO_s_mem());
	BIO* base64 = BIO_new(BIO_f_base64());
	//mem��ӵ�base64��
	base64 = BIO_push(base64, mem);
	//д����
	BIO_write(base64, str, len);
	BIO_flush(base64);
	//�õ��ڴ����ָ��
	BUF_MEM* memPtr;
	BIO_get_mem_ptr(base64, &memPtr);
	string retStr = string(memPtr->data, memPtr->length - 1);
	BIO_free_all(base64);
	return retStr;
}

char* RSACrypto::fromBase64(string str)
{
	int length = str.size();
	BIO* base64 = BIO_new(BIO_f_base64());
	BIO* mem = BIO_new_mem_buf(str.data(), length);
	BIO_push(base64, mem);
	char* buffer = new char[length];
	memset(buffer, 0, length);
	BIO_read(base64, buffer, length);
	BIO_free_all(base64);
	return buffer;
}
//private:
	//��ȡ��Կ�Ƿ�ɹ�
bool RSACrypto::initPublicKey(string pubfile) {
	//����bio����			//�����Ĵ����ļ�  �����ļ��ķ�ʽ
	BIO* pubBio = BIO_new_file(pubfile.data(), "r");
	if (!PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL)) {
		ERR_print_errors_fp(stdout);
		return false;
	}
	BIO_free(pubBio);
	return true;
	/*FILE* fp = fopen(pubfile.data(), "r");
	if (!PEM_read_RSAPublicKey(fp, &m_publicKey, NULL, NULL)) {
		ERR_print_errors_fp(fp);
		return false;
	}
	fclose(fp);
	return true;*/
}

	//��ȡ˽Կ�Ƿ�ɹ�
bool RSACrypto::initPrivateKey(string prifile) {
	BIO* priBio = BIO_new_file(prifile.data(), "r");
	//��bio��pem���ݶ���
	if (!PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL)) {
		ERR_print_errors_fp(stdout);
		return false;
	}
	BIO_free(priBio);
	return true;
	/*FILE* fp = fopen(prifile.data(), "r");
	if (!PEM_read_RSAPrivateKey(fp, &m_privateKey, NULL, NULL)) {
		ERR_print_errors_fp(fp);
		return false;
	}
	fclose(fp);
	return true;*/
}


