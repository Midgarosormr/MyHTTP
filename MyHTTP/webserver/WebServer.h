#pragma once
#
#include "ThreadPool.h"

class WebServer	
{
public:
	WebServer();
	int WebServerinit();
	~WebServer();

private:
	ThreadPool * TPptr;
	uint m_count;

};
