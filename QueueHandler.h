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
	int check_mutex();
public:
	int push_to_queue(T * packet);
	T * pop_from_queue();
};
