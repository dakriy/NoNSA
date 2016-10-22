#pragma once
#include <queue>

class QueueHandler
{
protected:
	std::queue<void *> queue;
	bool mutex;
	void lock_mutex();
	void unlock_mutex();
	bool mutex_locked();
	void check_mutex();
public:
	void push_to_queue(void * packet);
	void * pop_from_queue();
};
