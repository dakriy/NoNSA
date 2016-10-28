#include "SocketWrapper.h"

void SocketWrapper::setError(std::string e)
{
	this->_error = e;
}

void SocketWrapper::listen_thread_func()
{
	for (;;)
	{
		try
		{
			if (this->_status != disconnected)
			{
				char buf[CHAR_MAX];
				boost::system::error_code error;
				size_t len = 0;
				read(*sock, boost::asio::buffer(buf, Message::header_length), error);
				if (error == boost::asio::error::eof)
				{
					this->disconnect();
					return; // Connection closed cleanly by peer
				}
				else if (error)
					throw boost::system::system_error(error);

				char c_len[5] = { '\0' };
#ifdef __linux__
				strncpy(c_len, buf, Message::header_length);
#else
				strncpy_s(c_len, buf, Message::header_length);
#endif
				size_t body_len = atoi(c_len);

				//char data[1024];
				char data[4096];

				read(*sock, boost::asio::buffer(data, body_len), error);
				if (error == boost::asio::error::eof)
				{
					this->disconnect();
					return; // Connection closed cleanly by peer
				}
				else if (error)
					throw boost::system::system_error(error);

				if(body_len == 6)
				{
					char buff[7];
					char compare[7] = { 'e','x','i','t','(',')','\0' };
#ifdef __linux__
					strncpy(buff, data, 6);
#else
					strncpy_s(buff, data, 6);
#endif
					if (strcmp(buff, compare) == 0)
					{
						return;
					}
				}
				std::string data_str = std::string(data, body_len);
				std::string decrypted = keybase->DecryptVerify(data_str,partner);
				//printf("Remote Host: %.*s\n", body_len, data);
			}
			else
				return;
		} catch (std::exception& e)
		{
			printf("%s",e.what());
			printf("\n");
			return;
		}

	}
}

void SocketWrapper::send_thread_func()
{
	for (;;)
	{
		try
		{
			if (this->_status != disconnected)
			{
				if (!this->send_queue.empty())
				{
					Message data_to_send;
					this->send_queue.pop(data_to_send);
					boost::system::error_code ignored_error;
					//char temp[1028];
					char temp[4096];
					memcpy(temp, data_to_send.get_data(), data_to_send.length());
					//printf("Data Length: %d\n", data_to_send.length());
					boost::asio::write(*sock, boost::asio::buffer(temp, data_to_send.length()), boost::asio::transfer_all(), ignored_error);
					this->io_service->run();
				}
			}
			else
				return;
		} catch (std::exception& e)
		{
			printf("%s",e.what());
			printf("\n");
			return;
		}
	}
}

SocketWrapper::SocketWrapper()
{
	this->_status = disconnected;
	keybase = new Keybase();
}

Relationship SocketWrapper::get_status()
{
	return this->_status;
}

int SocketWrapper::connect(std::string server_addr, std::string port)
{
	if (this->get_status() == disconnected)
	{
		try
		{
			this->io_service = new boost::asio::io_service;

			tcp::resolver resolver(*io_service);
			tcp::resolver::query query(server_addr, port);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			sock = new tcp::socket(*io_service);
			boost::asio::connect(*sock, endpoint_iterator);
			this->_status = slave;
			this->start_threads();
			printf("Successfully connected to ");
			printf("%s",sock->remote_endpoint().address().to_string().c_str());
			printf("\n");
		} catch (std::exception& e)
		{
			printf("Could not connect to the remote host");
			printf("\n");
			this->setError(e.what());
			return 1;
		}
		
		return 0;
	}
	this->setError("Already have an active connection");
	return 1;
}

int SocketWrapper::wait_for_connection(int port = 6666)
{
	if (this->_status == disconnected)
	{
		try
		{
			printf("Listenting for connections...\n");
			this->io_service = new boost::asio::io_service;
			tcp::acceptor acceptor(*io_service, tcp::endpoint(tcp::v4(), port));

			this->sock = new tcp::socket(*io_service);
			acceptor.accept(*sock);
			printf("Recieved connection from ");
			printf("%s",sock->remote_endpoint().address().to_string().c_str());
			printf("\n");
			this->_status = master;
			this->start_threads();
			return 0;
		}
		catch (std::exception& e)
		{
			this->setError(e.what());
			return 1;
		}
	} else
	{
		this->setError("Already have active connection.");
		return 1;
	}
}

void SocketWrapper::send(std::string data)
{
	if (data.empty())
		return;
	Message tmp;
	std::string encrypted = keybase->SignEncrypt(data,partner);
	// tmp.set_body(data);
	tmp.set_body(encrypted);
	this->send_queue.push(tmp);
}

void SocketWrapper::send(Message data)
{
	this->send_queue.push(data);
}

void SocketWrapper::send(char* data, size_t len)
{
	Message temp;
	temp.set_body(data, len);
	this->send_queue.push(temp);
}

std::string SocketWrapper::getError()
{
	return this->_error;
}

void SocketWrapper::start_threads()
{
	listen_thread = new boost::thread(boost::bind(&SocketWrapper::listen_thread_func, this));
	send_thread = new boost::thread(boost::bind(&SocketWrapper::send_thread_func, this));
}

void SocketWrapper::stop_threads()
{
	this->_status = disconnected;
	this->listen_thread->join();
	this->send_thread->join();
	delete this->listen_thread;
	delete this->send_thread;
}

int SocketWrapper::disconnect()
{
	if (this->_status != disconnected)
	{
		this->_status = disconnected;
		this->send("exit()");
		this->stop_threads();
		delete io_service;
		delete sock;
		return 0;
	}
	this->setError("Already disconnected");
	return 1;
}

SocketWrapper::~SocketWrapper()
{
	if (this->_status != disconnected)
	{
		this->disconnect();
	}
}

void SocketWrapper::setPartnerName(std::string p_name)
{
	partner = p_name;
}
