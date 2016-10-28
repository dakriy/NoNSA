#include <stdio.h>
#include "logo.h"
#include <iostream>
#include "keybase.h"
#include <boost/algorithm/string.hpp>
#include "SocketWrapper.h"

int main()
{
	Logo logo;
	printf("Welcome to the NoNSA peer-to-peer chat client!\n");
	logo.printLogo();
	printf("You're on a list.\n");
	SocketWrapper sock;
	std::string what_to_do;
	std::string port;
	while (what_to_do.empty())
	{
		std::cout << "Connect or Recieve? C/R" << std::endl;
		std::cin >> what_to_do;
	}
	if (what_to_do == "C")
	{
		std::string host;
		std::cout << "host" << std::endl;
		std::cin >> host;
		std::cout << "port" << std::endl;
		std::cin >> port;
		sock.connect(host, port);
	} else if (what_to_do == "R")
	{
		std::cout << "port" << std::endl;
		std::cin >> port;
		sock.wait_for_connection(atoi(port.c_str()));
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
			std::cout << "Client disconnected. Do you want to rehost? Y/N" << std::endl;
			std::getline(std::cin, input);
			boost::algorithm::to_lower(input);
			if (input == "y" || input =="yes")
			{
				sock.wait_for_connection(port);
			} else
			{
				input = "exit()";
			}
			
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
