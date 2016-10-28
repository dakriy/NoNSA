#include "external/SimpleJSON/src/JSON.h"
#include "external/SimpleJSON/src/JSONValue.h"
#include <string>

// Ideally this would talk directly to the keybase daemon
// via unix domain sockets, but for now we'll just call the
// executable or talk to the keybase server via HTTPS.
// Also, I'm not worrying about Win32 support for the
// first pass. The libraries are cross-platform, but
// this is going to require some platform specific
// code, so for now it's nix only. --TS

class Keybase
{
	std::string binPath;
	bool debug_enabled;
	std::string callKeybase(std::string args);
public:
	Keybase();
	int getKeybaseStatus();
	void disableDebug();
	void enableDebug();
	std::string SignEncrypt(std::string message,std::string recipient);
	std::string DecryptVerify(std::string message, std::string sender);
};
