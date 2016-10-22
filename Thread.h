#pragma once

#include <functional>

class Thread
{
public:
	void CreateThread(std::function<void()> func, void * args);
};
