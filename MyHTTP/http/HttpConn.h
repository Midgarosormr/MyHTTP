
#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include "../webserver/WebServer.h"
#include "Buffer.h"
#include "HttpRequest.h"	//����������
#include "HttpResponse.h"	//��������������

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
	HttpRequest requestData;	//����client��������󣬷��ص����ݸ�ʽ(e.g���������󷽷����ͣ�url������ͷ������Ϣ)
	HttpResponse responseData;	//���������������

private:
	struct  iovec	iov[2];
	int iovCnt;
	Buffer readBuff_;	//��������
	Buffer writeBuff_;	//д������

};

