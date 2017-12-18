#pragma once
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include "InputList.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
using namespace std;

class Controller
{
public:
	Controller();
	~Controller();
	void initiate();
	int createSocket();
};

