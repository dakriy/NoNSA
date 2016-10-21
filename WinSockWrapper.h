#pragma once

#ifndef WIN_WRAPPER_SOCKET_HEADER
#define WIN_WRAPPER_SOCKET_HEADER

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "6666"
#endif

class WinSockWrapper
{
public:
	WinSockWrapper();
	int waitForConnection();
	int connectToHost(char * remoteHost);
	int sendData(char * sendbuf);
	int closeConnection();
	int recieveData();
	std::string get_error();
protected:
	int initializeServer();
	int initizlizeClient();
	int cleanup();
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
};