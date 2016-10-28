#include "Message.h"

void Message::set_header()
{
	using namespace std; // For sprintf and memcpy.
	char header[header_length + 1] = "";
	sprintf_s(header, "%4d", body_length);
	memcpy(data, header, header_length);
	return;
}

void Message::set_body(std::string data)
{
	size_t len = data.length();
	if (len > max_body_length)
		len = max_body_length;
	memcpy(this->data + header_length, data.c_str(), len);
	this->body_length = len;
	this->set_header();
}

void Message::set_body(char* data, size_t len)
{
	if (len > max_body_length)
		len = max_body_length;
	std::memcpy(this->data + header_length, data, len);
	this->body_length = len;
	this->set_header();
}

void Message::set_body_length(size_t length)
{
	body_length = length;
	if (body_length > max_body_length)
		body_length = max_body_length;
}

size_t Message::get_body_length()
{
	return body_length;
}

char* Message::body()
{
	return this->data + header_length;
}

char* Message::get_data()
{
	return this->data;
}

size_t Message::length() const
{
	return header_length + body_length;
}
