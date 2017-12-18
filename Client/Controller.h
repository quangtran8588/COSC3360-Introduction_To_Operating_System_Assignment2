#pragma once
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
using namespace std;

class Controller
{
public:
	Controller();
	~Controller();
	void createConnection();
};

