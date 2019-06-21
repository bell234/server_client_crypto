#include <iostream>
#include "ClientOP.h"
#include <string>

using namespace std;
extern int usage();
extern int clientFunc();

int main(int argc, char* argv[]) {
	//客户端函数
	clientFunc();

	return 0;
}

int usage() {
	int nSel = -1;
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     1.密钥协商                                            */");
	printf("\n  /*     2.密钥校验                                            */");
	printf("\n  /*     3.密钥注销                                            */");
	printf("\n  /*     4.密钥查看                                            */");
	printf("\n  /*     0.退出系统                                            */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  选择:");

	scanf("%d", &nSel);
	while (getchar() != '\n');

	return nSel;
	return 0;
}

int clientFunc() {
	ClientOP op("client.json");
	// 循环, 接收用户的输入
	while (1)
	{
		int sel = usage();
		switch (sel)
		{
		case 1:
			// 密钥协商
			op.seckeyAgree();
			break;
		case 2:
			op.seckeyCheck();
			// 密钥校验
			break;
		case 3:
			op.seckeyLogout();
			// 密钥注销
			break;
		default:
			printf("请输入正确的序号!\n");
			break;
		}
	}
	return 0;
}



