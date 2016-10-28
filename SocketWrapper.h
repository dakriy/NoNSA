#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/atomic.hpp>
#include "Globals.h"

using boost::asio::ip::tcp;

class SocketWrapper
{
protected:
	void setError(std::string e);
	boost::asio::io_service * io_service;
	std::string _error;
	boost::atomic<Relationship> _status;
	tcp::socket * sock;
	void listen_thread_func();
	void send_thread_func();
	boost::thread * listen_thread;
	boost::thread * send_thread;
	void start_threads();
	void stop_threads();
	boost::lockfree::queue<data_packet *, boost::lockfree::capacity<100>> listen_queue;
	boost::lockfree::queue<data_packet *, boost::lockfree::capacity<100>> send_queue;
public:
	SocketWrapper();
	Relationship get_status();
	int connect(std::string server_addr, std::string port);
	int wait_for_connection(int port);
	void send(std::string data);
	void send(data_packet * data);
	void send(char * data, size_t len);
	// Delete any packets coming out of here
	data_packet * recieve();
	std::string recieve_str();
	std::string getError();
	int disconnect();
	~SocketWrapper();
};