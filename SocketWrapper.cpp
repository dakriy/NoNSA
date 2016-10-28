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
				char * buf = new char[CHAR_MAX];
				boost::system::error_code error;
				size_t len = (*sock).read_some(boost::asio::mutable_buffers_1(buf, CHAR_MAX), error);
				printf("Remote Host: %.*s\n", len, buf);
				if (error == boost::asio::error::eof)
				{
					this->disconnect();
					return; // Connection closed cleanly by peer
				}
				else if (error)
					throw boost::system::system_error(error);
				
				this->io_service->run();
			}
			else
				return;
		} catch (std::exception& e)
		{
			printf(e.what());
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
					data_packet * data_to_send;
					this->send_queue.pop(data_to_send);
					boost::system::error_code ignored_error;
					boost::asio::write(*sock, boost::asio::buffer((*data_to_send).get_string_val()), boost::asio::transfer_all(), ignored_error);
					delete data_to_send;
					this->io_service->run();
				}
			}
			else
				return;
		} catch (std::exception& e)
		{
			printf(e.what());
			printf("\n");
			return;
		}
	}
}

SocketWrapper::SocketWrapper()
{
	this->_status = disconnected;
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
			printf(sock->remote_endpoint().address().to_string().c_str());
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
			printf(sock->remote_endpoint().address().to_string().c_str());
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
	data_packet *temp = new data_packet(data);
	this->send_queue.push(temp);
}

void SocketWrapper::send(data_packet * data)
{
	this->send_queue.push(data);
}

void SocketWrapper::send(char* data, size_t len)
{
	data_packet *temp = new data_packet(data, len);
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
