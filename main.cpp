#include <stdio.h>
#include "logo.h"
#include <iostream>
//#include "keybase.h"
#include "SocketWrapper.h"

int main()
{
	Logo logo;
	printf("Welcome to the NoNSA peer-to-peer chat client!\n");
	logo.printLogo();
	SocketWrapper sock;
	std::string what_to_do;
	std::string input;
	std::string enc_input;
	std::string recipient;
	//Keybase keybase;

	while (what_to_do.empty())
	{
		std::cout << "Connect or Recieve? C/R" << std::endl;
		std::cin >> what_to_do;
	}
	if (what_to_do == "C")
	{
		std::string port;
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
		int port;
		std::cout << "Please enter port to listen on:" << std::endl;
		std::cin >> port;
		std::cout << "Please enter username of recipient:" << std::endl;
		std::cin >> recipient;
		sock.setPartnerName(recipient);
		sock.wait_for_connection(port);
	}

	for (;;)
	{
		std::cout << sock.getError() << std::endl;
		std::getline(std::cin, input);

		//enc_input = keybase.SignEncrypt(input,recipient);
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
