#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

class Message
{
public:
	enum { header_length = 4 };
	enum { max_body_length = 4096 };
	Message() : body_length(0)
	{
		
	};
	void set_header();
	void set_body(std::string data);
	void set_body(char * data, size_t len);
	void set_body_length(size_t length);
	size_t get_body_length();
	char* body();
	char* get_data();
	size_t length() const;
private:
	char data[header_length + max_body_length];
	size_t body_length;
};
