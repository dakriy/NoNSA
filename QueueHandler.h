#pragma once
#include <queue>

template <class T>
class QueueHandler
{
protected:
	std::queue<T> queue;
	bool mutex;
	void lock_mutex();
	void unlock_mutex();
	bool mutex_locked();
	void check_mutex();
public:
	void push_to_queue(T * packet);
	T * pop_from_queue();
};
