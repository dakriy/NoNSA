#include "Thread.h"

template <class T>
Thread<T>::Thread()
{
	
}

template <class T>
int Thread<T>::start_thread(std::function<void> func)
{
	thread{func, args};
	return 0;
}

template <class T>
Thread<T>::~Thread()
{
	thread.interrupt();
	thread.join();
}
