#include <iostream>
#include "ClientOP.h"
#include <string>
using namespace std;

int usage() {
	int nSel = -1;
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     1.��ԿЭ��                                            */");
	printf("\n  /*     2.��ԿУ��                                            */");
	printf("\n  /*     3.��Կע��                                            */");
	printf("\n  /*     4.��Կ�鿴                                            */");
	printf("\n  /*     0.�˳�ϵͳ                                            */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  ѡ��:");

	scanf("%d", &nSel);
	while (getchar() != '\n');

	return nSel;
	return 0;
}

int clientFunc() {
	ClientOP op("client.json");
	// ѭ��, �����û�������
	while (1)
	{
		int sel = usage();
		switch (sel)
		{
		case 1:
			// ��ԿЭ��
			op.seckeyAgree();
			break;
		case 2:
			op.seckeyCheck();
			// ��ԿУ��
			break;
		case 3:
			op.seckeyLogout();
			// ��Կע��
			break;
		}
	}
	return 0;
}


int main(int argc, char *argv[]) {
	//�ͻ��˺���
	clientFunc();

	return 0;
}
