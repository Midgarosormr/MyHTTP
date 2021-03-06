
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
	ssize_t Read();
	ssize_t Write();
	int WriteBytesCount();
	

public:
	HttpRequest requestData;	//解析client连接请求后，返回的数据格式(e.g：含有请求方法类型，url，请求头部等信息)
	HttpResponse responseData;	//返回请求的数据类

private:
	int m_fd;
	struct sockaddr_in addr;
	bool isClose;
	int iovCnt;
	struct  iovec	iov[2];
	Buffer readBuff;	//读缓冲区
	Buffer writeBuff;	//写缓冲区

};

