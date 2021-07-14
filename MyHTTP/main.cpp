#include <iostream>
#include "webserver/WebServer.h"

int main()
{
	WebServer* server=new WebServer();
	server->initWebServer();

    return 0;
}
