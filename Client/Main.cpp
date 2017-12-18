/********************************************
- Quang Tran - PSID: 1290921
- COSC 3360 - Spring 2017
- Assignment 2 - Client-Server TCP Socket
********************************************/
#include <string>
#include "Controller.h"
using namespace std;

int main()
{
	Controller controller = Controller();
	controller.createConnection();
	return 0;
}
