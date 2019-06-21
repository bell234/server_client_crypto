#include <iostream>
#include "ServerOP.h"
using namespace std;


int serverFunc() {

	ServerOP op("server.json");
	op.startServer();
	return 0;
}


int main(int argc, char *argv[]) {
	//客户端函数
	serverFunc();

	return 0;
}
