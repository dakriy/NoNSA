#include "QueueHandler.h"
#include <thread>

template <class T>
void QueueHandler<T>::lock_mutex()
{
	this->mutex = true;
}

template <class T>
void QueueHandler<T>::unlock_mutex()
{
	this->mutex = false;
}

template <class T>
bool QueueHandler<T>::mutex_locked()
{
	return this->mutex;
}

template <class T>
void QueueHandler<T>::check_mutex()
{
	while (this->mutex_locked())
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

template <class T>
void QueueHandler<T>::push_to_queue(T * data)
{
	this->check_mutex();
	this->lock_mutex();
	this->queue.push(data);
	this->unlock_mutex();

}

template <class T>
T * QueueHandler<T>::pop_from_queue()
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
