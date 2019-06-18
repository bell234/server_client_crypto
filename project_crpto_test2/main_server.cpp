#include <iostream>
#include "ServerOP.h"
using namespace std;


int serverFunc() {

	ServerOP op("server.json");
	op.startServer();
	return 0;
}


int main(int argc, char *argv[]) {
	//¿Í»§¶Ëº¯Êý
	serverFunc();

	return 0;
}
