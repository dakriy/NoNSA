#include <stdio.h>
#include "logo.h"
#include <iostream>
#include "keybase.h"
#include "SocketWrapper.h"

int main()
{
	Logo logo;
	printf("Welcome to the NoNSA peer-to-peer chat client!\n");
	logo.printLogo();
	SocketWrapper sock;
	std::string what_to_do;

	while (what_to_do.empty())
	{
		std::cout << "Connect or Recieve? C/R" << std::endl;
		std::cin >> what_to_do;
	}
	if (what_to_do == "C")
	{
		std::string port = "6666";
		std::string host = "127.0.0.1";
		std::cout << "host" << std::endl;
		//std::cin >> host;
		std::cout << "port" << std::endl;
		//std::cin >> port;
		sock.connect(host, port);
	} else if (what_to_do == "R")
	{
		int port = 6666;
		std::cout << "port" << std::endl;
		//std::cin >> port;
		sock.wait_for_connection(port);
	}

	std::string input;

	while (input != "exit()")
	{
		std::cout << sock.getError() << std::endl;
		std::getline(std::cin, input);
		sock.send(input);
		std::cout << sock.recieve_str() << std::endl;
		if (sock.get_status() == disconnected)
		{
			std::cout << "Disconnected" << std::endl;
			input = "exit()";
		}
	}
	if(sock.get_status() != disconnected)
	{
		sock.disconnect();
	}
	//printf("Currently there is zero functionality. Sorry about that...\n");

	//Keybase keybase;
	//printf("Keybase Status: %d\n",keybase.getKeybaseStatus());
	return 0;
}
