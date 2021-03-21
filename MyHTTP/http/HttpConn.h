
#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include "Buffer.h"
#include "HttpRequest.h"	//����������
#include "HttpResponse.h"	//��������������

class HttpConn
{
public:
	HttpConn();
	HttpConn(int& fd,const sockaddr_in& clientaddr);
	HttpConn(const HttpConn& hc);
//	HttpConn& operator=(HttpConn& hcCOPY) {};
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
	Buffer readBuff_;	//��������
	Buffer writeBuff_;	//д������

};

