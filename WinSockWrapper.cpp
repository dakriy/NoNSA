#include "WinSockWrapper.h"

WinSockWrapper::WinSockWrapper()
{
	this->initializeServer();
	this->initizlizeClient();
}

int WinSockWrapper::waitForConnection()
{
	// Create a SOCKET for connecting to server
	ListenSocket = socket(sresult->ai_family, sresult->ai_socktype, sresult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(sresult);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	siResult = bind(ListenSocket, sresult->ai_addr, (int)sresult->ai_addrlen);
	if (siResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(sresult);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(sresult);

	siResult = listen(ListenSocket, SOMAXCONN);
	if (siResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	
	return 0;
}

int WinSockWrapper::connectToHost(char* remoteHost)
{
	if(master)
	{

		return 1;
	}
	// Resolve the server address and port
	ciResult = getaddrinfo(remoteHost, DEFAULT_PORT, &chints, &cresult);
	if (ciResult != 0) {
		printf("getaddrinfo failed with error: %d\n", ciResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (cptr = cresult; cptr != NULL; cptr = cptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(cptr->ai_family, cptr->ai_socktype,
			cptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		ciResult = connect(ConnectSocket, cptr->ai_addr, (int)cptr->ai_addrlen);
		if (ciResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(cresult);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	return 0;
}

int WinSockWrapper::sendData(char * sendbuf)
{
	if (!master)
	{
		ciResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (ciResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}
	else
	{
		int iSendResult;
		iSendResult = send(ClientSocket, sendbuf, siResult, 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}
	
}

int WinSockWrapper::closeConnection()
{
	char close_connection_command = 0;
	this->sendData(&close_connection_command);
	this->cleanup();
	return 0;
}


int WinSockWrapper::cleanup()
{
	// shutdown the client socket
	ciResult = shutdown(ConnectSocket, SD_SEND);
	if (ciResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();


	// shutdown the server connection socket
	siResult = shutdown(ClientSocket, SD_SEND);
	if (siResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}

int WinSockWrapper::recieveData()
{
	char recvbuf[DEFAULT_BUFLEN];

	int recvbuflen = DEFAULT_BUFLEN;

	if (!master)
	{
		do {
			ciResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (ciResult > 0)
				printf("Bytes received: %d\n", ciResult);
			else if (ciResult == 0)
				this->cleanup();
			else
				printf("recv failed with error: %d\n", WSAGetLastError());

		} while (ciResult > 0);
	} else
	{
		// Receive until the peer shuts down the connection
		do {

			siResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
			if (siResult > 0) {
				printf("Bytes received: %d\n", siResult);
			}
			else if (siResult == 0)
				this->cleanup();
			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

		} while (siResult > 0);
	}
	return 0;
}

std::string WinSockWrapper::get_error()
{
	return this->error;
}

int WinSockWrapper::initializeServer()
{
	WSADATA wsaData;
	
	struct addrinfo hints;


	// Initialize Winsock
	siResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (siResult != 0) {
		printf("WSAStartup failed with error: %d\n", siResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	siResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &sresult);
	if (siResult != 0) {
		printf("getaddrinfo failed with error: %d\n", siResult);
		WSACleanup();
		return 1;
	}
	
	return 0;
}

int WinSockWrapper::initizlizeClient()
{
	WSADATA wsaData;

	// Initialize Winsock
	ciResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ciResult != 0) {
		printf("WSAStartup failed with error: %d\n", ciResult);
		return 1;
	}

	ZeroMemory(&chints, sizeof(chints));
	chints.ai_family = AF_UNSPEC;
	chints.ai_socktype = SOCK_STREAM;
	chints.ai_protocol = IPPROTO_TCP;

	return 0;
}
