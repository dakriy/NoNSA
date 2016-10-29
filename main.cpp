#include <stdio.h>
#include "logo.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "SocketWrapper.h"

int main()
{
	Logo logo;
	printf("Welcome to the NoNSA peer-to-peer chat client!\n");
	logo.printLogo();
	printf("You're here because you're on a list.\n");
	SocketWrapper sock;
	std::string what_to_do;
	std::string input;
	std::string recipient;
	std::string port;
	while (what_to_do.empty())
	{
		std::cout << "Connect or Recieve? C/R" << std::endl;
		std::cin >> what_to_do;
	}
	if (what_to_do == "C")
	{
		std::string host;
		std::cout << "Please enter host to connect to:" << std::endl;
		std::cin >> host;
		std::cout << "Please enter port number to connect to:" << std::endl;
		std::cin >> port;
		std::cout << "Please enter username of recipient:" << std::endl;
		std::cin >> recipient;
		sock.setPartnerName(recipient);
		sock.connect(host, port);
	} else if (what_to_do == "R")
	{
		std::cout << "Please enter port to listen on:" << std::endl;
		std::cin >> port;
		std::cout << "Please enter username of recipient:" << std::endl;
		std::cin >> recipient;
		sock.setPartnerName(recipient);
		sock.wait_for_connection(atoi(port.c_str()));
	}

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
				sock.wait_for_connection(atoi(port.c_str()));
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

	return 0;
}
