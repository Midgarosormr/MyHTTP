#pragma once
#include <string>
#include <sys/stat.h>
#include "HttpRequest.h"
#include "HttpHelper.h"
#include <sys/uio.h>

using std::string;

class HttpResponse
{
public:
	HttpResponse(const HttpRequest* request);
//	HttpResponse(const HttpResponse& cp);
	~HttpResponse();

	bool onWork(Buffer* writeBuff);
	bool fileAuthority();	//检测请求的文件权限问题
	bool onSendBuff(Buffer* writeBuff);
	int onSendfd(Buffer* writeBuff,int* err);

	
	int code_;

private:
	const HttpRequest* requestPTR_;
	struct stat fileStat;


};

