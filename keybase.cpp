#include "keybase.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

Keybase::Keybase()
{
	binPath="/usr/bin/keybase";
	return;
}

int Keybase::getKeybaseStatus()
{

	std::string args = " status -j";
	JSONValue *status = callKeybase(args);
	JSONObject root;
	std::wstring username,loggedin,session_valid;
	if(status->IsObject())
	{
		root=status->AsObject();
		if (root.find(L"Username") != root.end() && root[L"Username"]->IsString() && root[L"Username"]->AsString().size()>0)
		{
			username=root[L"Username"]->AsString();
			printf("Username: %ls\n", username.c_str());

			if (root.find(L"LoggedIn") != root.end() && root[L"LoggedIn"]->IsBool())
			{
				loggedin = root[L"LoggedIn"]->AsBool();
				bool loggedin_bool = loggedin.c_str()[0];
				printf("Logged In: %s\n", (loggedin_bool ? "True":"False"));
				return 0;

			}
			else
			{
				printf("%ls is not currenty loggedinto keybase. Please login and try again.\n",username.c_str());
				return -1;
			}
		}

		printf("Keybase didn't respond with a valid user field. Please setup keybase and try again.");
	return -1;
	}
	
	printf("Unable to parse response from keybase!\n");
	return -1;
}

JSONValue *Keybase::callKeybase(std::string args)
{
	char buffer[128];
	std::string result = "";
	std::string full_cmd;
	full_cmd = binPath+args;
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
	//std::cout << "Results: " << result << std::endl;
	//printf("Results?:%s\n", result.c_str();

	ret = JSON::Parse(result.c_str());
	if (ret->IsObject())
		return ret;
	else
		return NULL;
}