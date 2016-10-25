#pragma once


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <queue>
#include "QueueHandler.h"
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6666"

class WinSockWrapper
{
public:
	WinSockWrapper();
	int waitForConnection();
	int connectToHost(char * remoteHost);
	int sendData(char * data, int size_of_data);
	void recieveData();
	std::string get_error();
	// Returns true if master or false if slave or if there is no connection.
	bool get_connection_relationship();
	// Returns false if there is no connection
	bool get_connection_state();
	int disconnect();
	void stop();
	~WinSockWrapper();
protected:
	int initializeServer();
	int initizlizeClient();
	int closeConnection();
	int sendall(char *buf, int *len);
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ConnectSocket = INVALID_SOCKET;
	int siResult;
	bool master = false;
	struct addrinfo *sresult = NULL,
		*cresult = NULL,
		*cptr = NULL,
		chints;
	int ciResult;
	std::string error;
	bool closed = true;
};

