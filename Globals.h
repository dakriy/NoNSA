#pragma once

#include <string>

enum Relationship
{
	master, slave, disconnected
};

struct data_packet
{
	data_packet()
	{
		
	};

	data_packet(std::string data)
	{
		this->data = new char[data.length() + 1];
		this->data = _strdup(data.data());
		this->len = data.length() + 1;
		this->data[len - 1] = '\0';
	};
	
	data_packet(char * data, size_t len)
	{
		this->data = data;
		this->len = len;
	};

	std::string get_string_val()
	{
		std::string retval(data);
		return retval;
	};

	char * data;
	size_t len;

	~data_packet()
	{
		delete[] data;
	};
};
