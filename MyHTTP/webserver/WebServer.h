/*
 * @Author       : SharkZ
 * @Date         : 2021-02-21
 */

#pragma once
#include "ThreadPool.h"

class WebServer	
{
public:
	WebServer();
	bool WebServerinit();
	~WebServer();

private:
	ThreadPool * TPptr;
	uint m_count_;
	int port_;
	bool openLinger_;
	int timeoutMS_;  /* MS */
	bool isClose_;   //status
	int listenFd_;
	char* srcDir_;

};
