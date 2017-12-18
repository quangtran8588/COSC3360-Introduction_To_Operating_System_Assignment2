#include "Controller.h"
const int MAXHOSTNAME = 1024;
InputList* inputList;

Controller::Controller()
{
}

Controller::~Controller()
{
}
// function to push data into socket and send to Client
int write_data(int sockConn,char *buff, int n)
{
	int bCount, bWrite;
	bCount = bWrite = 0;
	while (bCount < n)
	{
		if ((bWrite = send(sockConn,&buff[bWrite],n-bCount,0)) > 0){
			bCount += bWrite;
		}
		else if (bWrite < 0)
			return -1;
		else
			return 0;			
	}
	return bCount;
} 
// function to get data from socket
int read_data (int sockConn, char *buff, int n)
{
	int bCount, bRead;
	bCount = bRead = 0;
	while (bCount < n)
	{
		if ((bRead = recv(sockConn,&buff[bRead],n-bCount,0)) > 0){
			bCount += bRead;
		}
		else if (bRead < 0)
			return -1;
		else
			return 0;
	}
	return bCount;
}
// function to create Server to communicate between Client-Server socket TCP
int establish()
{
// Create and Initialize variables to be used
	long successful;
	char myname[MAXHOSTNAME+1];
	int connection, sock, c;
	int portnum;
	int time_out = 0;
	bool connection_lost = false;
	bool repeat = false;
	successful = 0;
	c = portnum = connection = sock = 0;
	struct sockaddr_in address;
	struct hostent *hp;
 	char sendBuff[1024] = "";
	char recvBuff[1024] = "";
	fd_set readfds;
	struct timeval tv;
// Get hostname and Port number of Socket Server	
	memset(&address, 0, sizeof(address));
	gethostname(myname, MAXHOSTNAME);
	cout << "Please enter port number: ";
	cin >> portnum;
	c = sizeof(struct sockaddr_in);
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = htons(portnum);
// Create Socket Server
	if ((sock = socket(AF_INET, SOCK_STREAM,0)) < 0){
		cout << "Fail to create a socket" << endl;
		return -1;
	}
// Bind socket and listen Client connection
// Maximum of 100 clients can connect at the same time
	if (bind(sock,(struct sockaddr*)&address, sizeof(address)) < 0){
		cout << "Fail to bind a socket" << endl;
		close(sock);
		return -1;
	}
	cout << "Waiting for client connection....." << endl;
	listen(sock, 100);
// Create a loop to perform communicate between Server-Client TCP connection
// When Client first connect to server, Client will send hello message to Server
// then, Server will reply by wellcome message to Client
// After that, Client will send userID to request public key from Server
// If userID is in the text file, Server will respond the public key
// Otherwise, Server respond an empty string to Client
// If server is received the userID = Terminate which is dedicated for super user to shutdown server
// then, Server will close the socket and all connections.
	for (;;)
	{
		connection_lost = false;
		if ((connection = accept(sock,(struct sockaddr*)&address,(socklen_t*)&c)) < 0){
			cout << "Error to connect" << endl;
			return -1;
		}
		else
		{
		// Receive hello message from Client when it connects to Server
			cout << "\nConnection was found!" << endl;
			if ((successful = read_data(connection, recvBuff, sizeof(recvBuff))) < 0){
				cout << "Fail to receive response from Client" << endl;
				close(connection);
				close(sock);
				exit(1);
			}
			cout << "Message from Client: " << recvBuff << endl;
		// Reply wellcome message to Client
			char wellcome[1024] = "Wellcome. You have been connected to the Server";
			if ((successful = write_data(connection, wellcome, sizeof(wellcome))) < 0){
				cout << "Fail to Wellcome message" << endl;
				close(connection);
				close(sock);
				exit(1);
			}
		// Set time_out for receiving userID from Client
		// If Server receive userID within 60 seconds
		// it will check in its text file to find the public key which correspond to userID
		// and send back to Client the corresponding public key
		// If Client connection has been lost
		// then, Server will continue to listen incoming connections or accept the waiting Client
		// The Server will be terminated when it receive super userID which is "Terminate"
			do
			{
				repeat = true;
				FD_ZERO(&readfds);
				FD_SET(connection, &readfds);
				tv.tv_sec = 60;
				time_out = select(connection+1, &readfds, NULL, NULL, &tv);
				if (time_out == -1)
					perror("select");
				else if (time_out == 0){
					cout << "Time out! No data have been received after 60 seconds." << endl;
					close(connection);
					repeat = false;
				}
				else
				{	
					if ((successful = read_data(connection, recvBuff, sizeof(recvBuff))) < 0)
					{
						cout << "Fail to receive response from Client" << endl;		
						close(connection);
						close(sock);
						exit(1);
					}
					else if (successful == 0){
						cout << "Client connection has been lost!" << endl;
						close(connection);
						connection_lost = true;
						repeat = false;
					}
				}
				if (time_out != 0 && time_out != -1 && !connection_lost){
					data* checkUserID;
					if ((strcmp(recvBuff, "Terminate")) == 0)
					{
						cout << "\nServer has been terminated by super user!" << endl;
						close(connection);
						close(sock);
						return 0;
					}
					else if ((strcmp(recvBuff, "Close Connection")) == 0){
						cout << "Client has been closed the connection." << endl;
						close(connection);
						repeat = false;
					}
					else if ((checkUserID = inputList->exist(recvBuff)) != NULL)
					{
						string pubKey = inputList->getPublicKey(checkUserID);
						strcpy(sendBuff, pubKey.c_str());
						if ((successful = write_data(connection, sendBuff, sizeof(sendBuff))) < 0){
							cout << "Fail to send public key to Client" << endl;
							close(connection);
							close(sock);
							exit(1);
						}
						else if (successful == 0){
							cout << "Connection has been lost!" << endl;
							close(connection);
							repeat = false;
						}
					}
					else
					{
						char empty_string[1024] = "";
						if ((successful = write_data(connection, empty_string, sizeof(empty_string))) < 0)
						{
							cout << "Fail to send empty string to Client" << endl;
							close(connection);
							close(sock);	
							exit(1);
						} 
						else if (successful == 0){
							cout << "Connection has been lost!" << endl;
							close(connection);
							repeat = false;
						}
					}
				}
			}while(repeat);
		}
	}
	return sock;
}
// function to read the text file which contains userIDs and public keys
void Controller::initiate()
{
	string filename;
	ifstream infile;
	inputList = new InputList();
	bool repeat = false;
	do
	{
		cout << "Please enter the file name: ";
		cin >> filename;
		infile.open(filename);
		if (!infile){
			cout << "Unable to open the file. Please check it again." << endl;
			repeat = true;
		}
		else
		{
			while (!infile.eof())
			{	
				string data, publicKey;
				stringstream ss;
				getline(infile, data);
				if (data  == "")
					continue;
				else{
					ss << data;
					ss >> data >> publicKey;
					inputList = inputList->setData(data, publicKey);
				}
			}
			repeat = false;
		}
	}while(repeat);
//	cout << inputList->toString(inputList);
}

int Controller::createSocket()
{
	int s;
	s = 0;
	if ((s = establish() < 0))
		return -1;
	return 0;
}
