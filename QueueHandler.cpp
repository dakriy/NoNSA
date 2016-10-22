#include "QueueHandler.h"
#include <thread>

void QueueHandler::lock_mutex()
{
	this->mutex = true;
}

void QueueHandler::unlock_mutex()
{
	this->mutex = false;
}

bool QueueHandler::mutex_locked()
{
	return this->mutex;
}

void QueueHandler::check_mutex()
{
	while (this->mutex_locked())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

void QueueHandler::push_to_queue(void * data)
{
	this->check_mutex();
	this->lock_mutex();
	this->queue.push(data);
	this->unlock_mutex();

}

void * QueueHandler::pop_from_queue()
{
	void * temp = nullptr;
	this->check_mutex();
	this->lock_mutex();
	if (!queue.empty())
	{
		temp = queue.front();
		queue.pop();
	}
	this->unlock_mutex();
	return temp;
}
