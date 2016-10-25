#pragma once

#include "QueueHandler.h"
#include <boost/thread.hpp>

template <class T>
class Thread
{
public:
	Thread();
	QueueHandler<T> communicationQueue;
	~Thread();
};


