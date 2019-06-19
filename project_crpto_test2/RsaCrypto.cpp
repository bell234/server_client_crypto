#include "RsaCrypto.h"
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/buffer.h>
#include <string.h>
#include <iostream>

//	RSA* m_publicKey;//公钥
//	RSA* m_privateKey;

//extern "C" {
//#include <openssl/applink.c>
//};


RSACrypto::RSACrypto() {
	//申请一块内存
	this->m_privateKey = RSA_new();
	this->m_publicKey = RSA_new();
}
//根据拆入参数确定初始化公钥还是私钥
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
//析构函数 释放申请的密钥的内存 
RSACrypto::~RSACrypto() {
	RSA_free(this->m_privateKey);
	RSA_free(this->m_publicKey);
}
//将公钥/私钥字符串解析到RSA对象中
void RSACrypto::parseKeyString(string keystr, bool pubkey)
{	//字符串数据-->BIO对象中
	BIO* bio = BIO_new_mem_buf(keystr.data(), keystr.size());
	//公钥字符串
	if (pubkey) {
		PEM_read_bio_RSAPublicKey(bio, &m_publicKey, NULL, NULL);
	}
	else {
		//私钥字符串
		PEM_read_bio_RSAPrivateKey(bio, &m_privateKey, NULL, NULL);
	}
	BIO_free(bio);
}

//生成RSA密钥对
void RSACrypto::generateKeyFile(int bits, string pub, string pri) {
	//1申请内存
	RSA* rsa = RSA_new();

	BIGNUM* bignum = BN_new();
	//初始化BIGNUM对象
	BN_set_word(bignum, 4567887);

	//生成RSA密钥对
	RSA_generate_key_ex(rsa, bits, bignum, NULL);

	//创建bio文件对象
	BIO* publicBIO = BIO_new_file(pub.data(), "w");
	//公钥以pem格式写入文件中
	PEM_write_bio_RSAPublicKey(publicBIO, rsa);
	//缓存中数据刷新到缓冲区
	BIO_flush(publicBIO);
	BIO_free(publicBIO);

	//创建bio对象
	BIO* privateBi0 = BIO_new_file(pri.data(), "w");
	//私钥以pem格式写入文件中
	PEM_write_bio_RSAPrivateKey(privateBi0, rsa, NULL, NULL, 0, NULL, NULL);
	BIO_flush(privateBi0);
	BIO_free(privateBi0);

	//得到公钥和私钥
	//将参数rsa中的公钥/私钥提取出来
	m_privateKey = RSAPrivateKey_dup(rsa);
	m_publicKey = RSAPublicKey_dup(rsa);
	//释放资源
	BN_free(bignum);
	RSA_free(rsa);
}
//公钥加密
string RSACrypto::rsaPubKeyEncrypt(string data) {
	cout << "加密数据长度：" << data.size() << endl;
	//计算公钥长度
	int keyLen = RSA_size(m_publicKey);
	cout << "pbulic key length is: 公钥长度为：" << keyLen << endl;
	//根据公钥长度申请内存
	char* encode = new char[keyLen + 1];

	//使用公钥加密
	int ret = RSA_public_decrypt(data.size(), (const unsigned char*)data.data(),
		(unsigned char*)encode, m_publicKey, RSA_PKCS1_PADDING);

	string retStr = string();
	if (ret >= 0) {
		//加密成功
		cout << "ret: " << ret << ", keyLen: " << keyLen << endl;
		retStr = toBase64(encode, ret);
	}
	else {
		ERR_print_errors_fp(stdout);
	}
	delete[] encode;
	return retStr;
}
//私钥解密
string RSACrypto::rsaPriKeyDecrypt(string encData) {
	char* text = fromBase64(encData);
	//计算私钥长度
	//cout<<"解密数据长度： "<<text.size()<<endl;
	int keyLen = RSA_size(m_privateKey);
	//根据长度申请内存
	char* decode = new char[keyLen + 1];

	//使用私钥解密
	int ret = RSA_private_decrypt(keyLen, (const unsigned char*)text,
		(unsigned char*)decode, m_privateKey, RSA_PKCS1_PADDING);

	string retStr = string();
	if (ret >= 0) {
		//解密成功
		retStr = string(decode, ret);
	}
	else {
		cout << "解密失败" << endl;
		ERR_print_errors_fp(stdout);
	}
	delete[] decode;
	delete[] text;
	return retStr;
}
//使用RSA签名
string RSACrypto::rsaSign(string data, SignLevel level) {

	unsigned int len = 0;
	char* signBuf = new char[RSA_size(m_privateKey) + 1];
	memset(signBuf, 0, RSA_size(m_privateKey));
	//签名									//data.length();?
	int ret = RSA_sign(level, (const unsigned char*)data.data(), data.size(),
		(unsigned char*)signBuf, &len, m_privateKey);
	cout << "sign length is 签名的长度：" << len << endl;
	if (ret == -1) {
		ERR_print_errors_fp(stdout);
	}
	cout << "sign len: " << len << ", ret " << ret << endl;
	string retStr = toBase64(signBuf, len);
	delete[] signBuf;
	return retStr;
}

//使用RSA验证签名
bool RSACrypto::rsaVertifySign(string data, string signData, SignLevel level ) {
	//验证签名
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
	//mem添加到base64中
	base64 = BIO_push(base64, mem);
	//写数据
	BIO_write(base64, str, len);
	BIO_flush(base64);
	//得到内存对象指针
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
	//获取公钥是否成功
bool RSACrypto::initPublicKey(string pubfile) {
	//创建bio对象			//操作的磁盘文件  操作文件的方式
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

	//获取私钥是否成功
bool RSACrypto::initPrivateKey(string prifile) {
	BIO* priBio = BIO_new_file(prifile.data(), "r");
	//将bio中pem数据读出
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


