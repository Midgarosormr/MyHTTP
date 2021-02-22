/*
 * @Author       : SharkZ
 * @Date         : 2021-02-21
 */

#pragma once
#include "ThreadPool.h"

#define CRLF "\r\n"


class WebServer	
{
public:
	WebServer();
	bool WebServerinit();
	~WebServer();

private:
	//Server's own attribute.
	
	//resource
	ThreadPool * TPptr;


};
