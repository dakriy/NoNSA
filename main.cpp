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
	printf("You're on a list.\n");
	SocketWrapper sock;
	std::string what_to_do;

	while (what_to_do.empty())
	{
		std::cout << "Connect or Recieve? C/R" << std::endl;
		std::cin >> what_to_do;
	}
	if (what_to_do == "C")
	{
		std::string port;
		std::string host;
		std::cout << "host" << std::endl;
		std::cin >> host;
		std::cout << "port" << std::endl;
		std::cin >> port;
		sock.connect(host, port);
	} else if (what_to_do == "R")
	{
		int port;
		std::cout << "port" << std::endl;
		std::cin >> port;
		sock.wait_for_connection(port);
	}

	std::string input;

	for (;;)
	{
		if (sock.getError().empty())
		{
			std::cout << sock.getError() << std::endl;
		}
		std::getline(std::cin, input);
		
		sock.send(input);
		if (sock.get_status() == disconnected)
		{
			std::cout << "Disconnected" << std::endl;
			input = "exit()";
		}
		if (input == "exit()")
		{
			sock.disconnect();
			break;
		}
	}

	if(sock.get_status() != disconnected)
	{
		sock.disconnect();
	}
	std::cout << "Press any key to exit" << std::endl;
	std::getline(std::cin, input);

	//printf("Currently there is zero functionality. Sorry about that...\n");

	//Keybase keybase;
	//printf("Keybase Status: %d\n",keybase.getKeybaseStatus());
	return 0;
}
