/***********************************************
- Quang Tran - PSID: 1290921
- COSC 3360 - Spring 2017
- Assignment 2: Client-Server TCP Socket
***********************************************/
#include "Controller.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
	Controller controller = Controller();
	cout << "Server start running......." << endl << endl;
	controller.initiate();
	controller.createSocket();
//	system("pause");
	return 0;
}
