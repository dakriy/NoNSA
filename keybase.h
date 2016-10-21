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
public:
	Keybase();
	int getKeybaseStatus();
	std::string W2String(std::wstring ws);
	JSONValue *callKeybase(std::string args);
};