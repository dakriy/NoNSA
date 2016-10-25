#include "QueueHandler.h"
#include <boost/thread.hpp>

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
int QueueHandler<T>::check_mutex()
{
	int i = 0;
	while (this->mutex_locked())
	{
		boost::this_thread::sleep(boost::chrono::microseconds(1));
		i++;
		if (i > 30)
		{
			return 1;
		}
	}
	return 0;
}

template <class T>
int QueueHandler<T>::push_to_queue(T * data)
{
	if (this->check_mutex() == 1)
		return 1;
	this->lock_mutex();
	this->queue.push(data);
	this->unlock_mutex();
	return 0;

}

template <class T>
T * QueueHandler<T>::pop_from_queue()
{
	void * temp = nullptr;
	if (this->check_mutex() == 1)
		return nullptr;
	this->lock_mutex();
	if (!queue.empty())
	{
		temp = queue.front();
		queue.pop();
	}
	this->unlock_mutex();
	return temp;
}
