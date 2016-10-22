#include "WinSockWrapper.h"

WinSockWrapper::WinSockWrapper()
{
	this->initializeServer();
	this->initizlizeClient();
}

int WinSockWrapper::waitForConnection()
{
	this->closed = false;
	// Create a SOCKET for connecting to server
	ListenSocket = socket(sresult->ai_family, sresult->ai_socktype, sresult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		this->error = WSAGetLastError();
		freeaddrinfo(sresult);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	siResult = bind(ListenSocket, sresult->ai_addr, (int)sresult->ai_addrlen);
	if (siResult == SOCKET_ERROR) {
		this->error = WSAGetLastError();
		freeaddrinfo(sresult);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(sresult);

	siResult = listen(ListenSocket, 1);
	if (siResult == SOCKET_ERROR) {
		this->error = WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		this->error = WSAGetLastError();
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	this->master = true;
	return 0;
}

int WinSockWrapper::connectToHost(char* remoteHost)
{
	this->closed = false;
	if(master)
	{
		this->error = "Already have an active connection.";
		return 1;
	}
	// Resolve the server address and port
	ciResult = getaddrinfo(remoteHost, DEFAULT_PORT, &chints, &cresult);
	if (ciResult != 0) {
		this->error = WSAGetLastError();
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (cptr = cresult; cptr != NULL; cptr = cptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(cptr->ai_family, cptr->ai_socktype,
			cptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			this->error = WSAGetLastError();
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
		this->error = "Unable to connect to server!\n";
		WSACleanup();
		return 1;
	}
	return 0;
}

int WinSockWrapper::sendData(char * data, int size_of_data)
{
	if (this->sendall(data, &size_of_data) == -1)
		return -1;
	return 0;
}

int WinSockWrapper::closeConnection()
{
	if (!this->closed)
	{
		this->cleanup();
		this->closed = true;
	}
	return 0;
}


int WinSockWrapper::cleanup()
{
	if (closed)
		return 0;
	// shutdown the client socket
	ciResult = shutdown(ConnectSocket, SD_BOTH);
	if (ciResult == SOCKET_ERROR) {
		this->error = WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ConnectSocket);


	// shutdown the server connection socket
	siResult = shutdown(ClientSocket, SD_BOTH);
	if (siResult == SOCKET_ERROR) {
		this->error = WSAGetLastError();
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ClientSocket);

	return 0;
}

int WinSockWrapper::sendall(char* buf, int* len)
{
	int total = 0;
	int bytesleft = *len;
	int n;

	if (!master)
	{
		while (total < *len) {
			n = send(ConnectSocket, buf + total, bytesleft, 0);
			if (n == SOCKET_ERROR) {
				this->error = WSAGetLastError();
				closesocket(ConnectSocket);
				WSACleanup();
				return -1;
			}
			if (n == -1) { break; }
			total += n;
			bytesleft -= n;
		}

		*len = total; // return number actually sent here
		return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
	}
	else {
		while (total < *len) {
			n = send(ClientSocket, buf + total, bytesleft, 0);
			if (n == SOCKET_ERROR) {
				this->error = WSAGetLastError();
				closesocket(ConnectSocket);
				WSACleanup();
				return -1;
			}
			if (n == -1) { break; }
			total += n;
			bytesleft -= n;
		}

		*len = total; // return number actually sent here

		return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
	}

}

void WinSockWrapper::recieveData()
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
				this->closeConnection();
			else
				this->error = WSAGetLastError();

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
				this->error = WSAGetLastError();
				closesocket(ClientSocket);
				WSACleanup();
			}

		} while (siResult > 0);
	}
}

std::string WinSockWrapper::get_error()
{
	return this->error;
}

void WinSockWrapper::stop()
{
	WSACleanup();
}

int WinSockWrapper::initializeServer()
{
	WSADATA wsaData;
	
	struct addrinfo hints;


	// Initialize Winsock
	siResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (siResult != 0) {
		this->error = WSAGetLastError();
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
		this->error = WSAGetLastError();
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
		this->error = WSAGetLastError();
		return 1;
	}

	ZeroMemory(&chints, sizeof(chints));
	chints.ai_family = AF_UNSPEC;
	chints.ai_socktype = SOCK_STREAM;
	chints.ai_protocol = IPPROTO_TCP;

	return 0;
}

WinSockWrapper::~WinSockWrapper()
{
	this->closeConnection();
	this->stop();
}