
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
	ssize_t Read();
	ssize_t Write();
	int WriteBytesCount();
	

public:
	HttpRequest requestData;	//����client��������󣬷��ص����ݸ�ʽ(e.g���������󷽷����ͣ�url������ͷ������Ϣ)
	HttpResponse responseData;	//���������������

private:
	int m_fd;
	struct sockaddr_in addr;
	bool isClose;
	int iovCnt;
	struct  iovec	iov[2];
	Buffer readBuff;	//��������
	Buffer writeBuff;	//д������

};

