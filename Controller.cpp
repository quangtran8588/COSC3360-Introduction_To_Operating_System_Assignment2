#include "Controller.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}
// function to write data into socket and send to Server
int write_data (int sockConn, char *buff, int n)
{
	int bCount, bWrite;
	bCount = bWrite = 0;
	while(bCount < n)
	{
		if ((bWrite = send(sockConn,&buff[bWrite],n-bCount,0)) > 0) {
			bCount += bWrite;
		}
		else if (bWrite < 0)
			return -1;
		else
			return 0;
	}
	return bCount;
}
// function to read data which was sent from Server into TCP socket
int read_data (int sockConn, char *buff, int n)
{
	int bCount, bRead;
	bCount = bRead = 0;
	while (bCount < n)
	{
		if ((bRead = recv(sockConn, &buff[bRead],n-bCount,0)) > 0){
			bCount += bRead;
		}
		else if (bRead < 0)
			return -1;
		else 
			return 0;
	}
	return bCount;
}
// function to create TCP connection via socket to Server
void Controller::createConnection()
{
// Create and initialize variables
	int successful;
	int sock, connection;
	char hostname[1024];
	char hn[1024];
	char cont[100];
	int portnum;
	bool repeat = false;
	struct sockaddr_in address;
	struct hostent *host;
	char recvBuff[1024] = "";
	char sendBuff[1024] = "Hello! This is client connection.";
	successful = 0;
	portnum = sock = connection = 0;
	fd_set readfds;
	struct timeval tv;
	
	memset(&address, 0, sizeof(address));
// A loop is created to create Client TCP socket
// User will enter the hostname that needs to connect
// and its port number
// If port number or hostname is not correct
// Program will display warning message and prompt user to re-enter
	do
	{
		repeat = false;
		gethostname(hn,1024);
	// User enter hostname and port number to connect to Server
		do
		{
			cout << "Please enter hostname: ";
			cin >> hostname;
			if(strcmp(hostname,hn) != 0){
				cout << "Entering hostname is incorrect! Please enter the correct one." << endl;
				repeat = true;
			}
			else
				repeat = false;
		}while(repeat);
		cout << "Please enter port number: ";
		cin >> portnum;
		if ((host = gethostbyname(hostname)) == NULL)
		{
			cout << "Cannot get host name. Please check your hostname!" << endl;
			repeat = true;	
		}
	// Create TCP socket to connect to Server
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			cout << "Cannot create socket!" << endl;
			exit(1);
		}
		address.sin_addr = (*(struct in_addr*) host->h_addr);
		address.sin_family = AF_INET;
		address.sin_port = htons(portnum);
	// Connect to Server base on entering information of hostname and port number
	// If Server does not exist or wrong entering info, return back to prompt
	// Else, Client-Server connection will be created
	// and Client will send first hello message to Server
	// then receive wellcome message from Server
		if ((connection = connect(sock, (struct sockaddr*)&address, sizeof(address))) < 0)
		{
			cout << "Cannot create connection to Server. Please check your port number!" << endl;
			repeat = true;
		}
		else
		{
			if ((successful = write_data(sock, sendBuff, sizeof(sendBuff))) < 0){
				cout << "Fail to say Hello to server!" << endl;
				close(connection);
				close(sock);
				exit(1);
			}
			if ((successful = read_data(sock,recvBuff,sizeof(recvBuff))) < 0){
				cout << "Fail to receive message! Server might be down!" << endl;
				close(connection);
				close(sock);
				exit(1);
			}
			cout << recvBuff << endl;
	// After receiving wellcome message from Server
	// Client prompt user for userID in order to get corresponding public key
	// If userID = Terminate, then Client send to Server and close connection+socket
	// Otherwise, Client has to wait to receive response from Server
			do
			{
		//		repeat = true;
				cout << "Please enter user ID: ";
				cin >> sendBuff;
				if ((strcmp(sendBuff,"Terminate")) == 0)
				{	
					if ((successful = write_data(sock,sendBuff,sizeof(sendBuff))) < 0)
						cout << "Fail to send userID to Server" << endl;
					else if (successful == 0)
						cout << "Server is down. Cannot connect to Server" << endl;
					close(connection);
					close(sock);
					exit(1);
				}
				else
				{
					if ((successful = write_data(sock, sendBuff, sizeof(sendBuff))) < 0){
						cout << "Fail to send userID to Server" << endl;
						close(connection);
						close(sock);
						exit(1);
					}
					FD_ZERO(&readfds);
					FD_SET(sock, &readfds);
					tv.tv_sec = 20 ;
					int time_out = select(sock+1, &readfds, NULL, NULL, &tv);
					if (time_out == -1)
						perror("select");
					else if (time_out == 0)
					{
						cout << "Time Out! No response from Server after 20 seconds." << endl;
						close(connection);
						close(sock);
						exit(1);
					}
					else
					{
						if ((successful = read_data(sock, recvBuff, sizeof(recvBuff))) < 0){
							cout << "Fail to receive public key from Server" << endl;
							close(connection);
							close(sock);
							exit(1);
						}
						else if (successful == 0){
							cout << "Server is down! Cannot receive response from Server." << endl;
							close(connection);
							close(sock);
							exit(1);
						}
					}
					if ((strcmp(recvBuff, "")) == 0)
						cout << "There is no public key found" << endl;
					else
						cout << "The public key for user " << sendBuff << " is " << recvBuff << endl;
				}
				bool loop = false;
				do
				{
					loop = false;
					cout << "Do you want to continue?(Y/N): ";
					cin >> cont;
					if (strcmp(cont,"Y") == 0 || strcmp(cont,"y") == 0 )
						repeat = true;
					else if (strcmp(cont, "N") == 0 || strcmp(cont, "n") == 0){
						char close_conn[1024] = "Close Connection";
						if ((successful = write_data(sock, close_conn, sizeof(close_conn))) < 0)
							cout << "Fail to send message to close connection" << endl;
						close(connection);
						close(sock);
						exit(1);
					}
					else{
						loop = true;
						cout << "Invalid choice! Please enter (Y/N)." << endl;
					}
				}while(loop);		
			}while(repeat);
		}	
	}while(repeat);
}

