#pragma once

#include "QueueHandler.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

template <class T>
class Thread
{
protected:
	boost::thread thread;
public:
	Thread();
	int start_thread(std::function<void> func);
	QueueHandler<T> communicationQueue;
	~Thread();
};


