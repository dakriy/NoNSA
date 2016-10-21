#include <stdio.h>
#include "logo.h"
#include "keybase.h"

int main()
{
	Logo logo;
	printf("Welcome to the NoNSA peer-to-peer chat client!\n");
	logo.printLogo();
	printf("Currently there is zero functionality. Sorry about that...\n");

	Keybase keybase;
	printf("Keybase Status: %d\n",keybase.getKeybaseStatus());
	return 0;
}
