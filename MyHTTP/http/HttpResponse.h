#pragma once
#include <string>
#include <sys/stat.h>
#include "HttpRequest.h"


using std::string;

class HttpResponse
{
public:
	HttpResponse(const HttpRequest* request);
//	HttpResponse(const HttpResponse& cp);
	~HttpResponse();

	bool onWork(int cfd);
	bool fileAuthority(int clientFd);	//检测请求的文件权限问题
	void onError(int cfd);

private:
	const HttpRequest* requestPTR_;
	struct stat fileStat;
};

