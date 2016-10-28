#include "keybase.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

Keybase::Keybase()
{
	binPath="/usr/bin/keybase";
	debug_enabled=0;
	return;
}

int Keybase::getKeybaseStatus()
{
	// Returns -1 for any error states
	std::string args = "status -j";
	std::string stat = callKeybase(args);
	JSONValue *status = JSON::Parse(stat.c_str());
	JSONObject root;
	std::wstring username,loggedin,session_valid;
	if(status->IsObject())
	{
		root=status->AsObject();
		if (root.find(L"Username") != root.end() && root[L"Username"]->IsString() && root[L"Username"]->AsString().size()>0)
		{
			username=root[L"Username"]->AsString();
			if(debug_enabled) printf("Username: %ls\n", username.c_str());

			if (root.find(L"LoggedIn") != root.end() && root[L"LoggedIn"]->IsBool())
			{
				loggedin = root[L"LoggedIn"]->AsBool();
				bool loggedin_bool = loggedin.c_str()[0];
				if(debug_enabled) printf("Logged In: %s\n", (loggedin_bool ? "True":"False"));
				return 0;

			}
			else
			{
				if(debug_enabled) printf("%ls is not currenty loggedinto keybase. Please login and try again.\n",username.c_str());
				return -1;
			}
		}

		if(debug_enabled) printf("Keybase didn't respond with a valid user field. Please setup keybase and try again.");
	return -1;
	}
	if(debug_enabled) printf("Unable to parse response from keybase!\n");
	return -1;
}

std::string Keybase::callKeybase(std::string args)
{
	char buffer[128];
	std::string result = "";
	std::string full_cmd;
	full_cmd = binPath+" "+args;
	const char *cmd = full_cmd.c_str();
	JSONValue *ret;

	FILE* pipe = popen(cmd, "r");
	if(!pipe) throw std::runtime_error("popen() failed!");
	try{
		while(fgets(buffer, sizeof(buffer), pipe) != NULL)
		{
			result += buffer;
		}
	} catch (...) {
		pclose(pipe);
		throw;
	}
	pclose(pipe);

	return result;
}
void Keybase::disableDebug()
{
	debug_enabled=0;
	return;
}
void Keybase::enableDebug()
{
	debug_enabled=1;
	return;
}

std::string Keybase::SignEncrypt(std::string message,std::string recipient)
{
	int fd;
	char tempfilename[]="/tmp/fileXXXXXX";
	fd = mkstemp(tempfilename);
	write(fd,message.c_str(),strlen(message.c_str()));
	std::string args = std::string("sign -i ")+tempfilename;
	std::string signed_message = callKeybase(args);
	lseek(fd,0,SEEK_SET);
	write(fd,signed_message.c_str(),strlen(signed_message.c_str()));
	args = std::string("encrypt -i ")+tempfilename+" "+recipient;
	std::string enc_message = callKeybase(args);
	std::string zeros = std::string(signed_message.length(),'0');
	write(fd,zeros.c_str(),strlen(zeros.c_str()));
	close(fd);
	unlink(tempfilename);
	return enc_message;
}
std::string Keybase::DecryptVerify(std::string enc_message, std::string sender)
{
	int fd;
	char tempfilename[]="/tmp/fileXXXXXX";
	fd = mkstemp(tempfilename);
	std::string args = std::string("decrypt -o ")+tempfilename+" -m \""+enc_message+"\"";
	callKeybase(args);
	args = std::string("verify -i ")+tempfilename;
	std::string message = callKeybase(args);
	printf("%s\n",message.c_str());
	lseek(fd,0,SEEK_SET);
	std::string zeros = std::string(enc_message.length(),'0');
	close(fd);
	unlink(tempfilename);
	return message;
}