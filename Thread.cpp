#include "Thread.h"
#include <thread>

void Thread::CreateThread(std::function<void()> func, void * args)
{
	std::thread exec(func, args);
}