
#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include "../webserver/WebServer.h"
#include "Buffer.h"
#include "HttpRequest.h"	//请求数据类
#include "HttpResponse.h"	//返回请求数据类

class HttpConn
{
public:
	explicit HttpConn(int fd,sockaddr_in clientaddr);
	~HttpConn();
	int readBuff();
	int writeBuff();
	int WriteBytesCount();
	bool onWork_request();
	bool onWork_response();

public:
	int m_fd;
	struct sockaddr_in addr;
	bool isClose;
	HttpRequest requestData;	//解析client连接请求后，返回的数据格式(e.g：含有请求方法类型，url，请求头部等信息)
	HttpResponse responseData;	//返回请求的数据类

private:
	struct  iovec	iov[2];
	int iovCnt;
	Buffer readBuff_;	//读缓冲区
	Buffer writeBuff_;	//写缓冲区

};

