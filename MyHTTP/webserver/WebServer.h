/*
 * @Author       : SharkZ
 * @Date         : 2021-02-21
 */

#pragma once
#include "ThreadPool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

#include "../http/HttpConn.h"
#include "SQLPool.h"

using std::string;

class WebServer	
{
public:
	WebServer(int port,		//�����˿�:80/8080
		uint TPnums,		//�̳߳��߳���
		string user_db, string passwd_db, string name_db, uint sqlpool_nums, //SQL �û���/����/ʹ�õ����ݿ�����
		bool islog)		//�Ƿ���LOGϵͳ 
		:m_Listedport(port),TP_MAX_NUM(TPnums),sqlName(user_db),sqlpasswd(passwd_db),sqldabase(name_db),SQLPOOL_MAX_NUM(sqlpool_nums),m_islog(islog){};
	
	bool WebServerinit();

	~WebServer();

private:
	//Server's own attribute.
	in_port_t m_Listedport; //�����˿�
	char* resouceDir;   //��ԴĿ¼·��
	
	bool m_islog;	//�Ƿ�����LOGϵͳ
	char* logDir;	//LOGϵͳ��¼·��

public:
	int m_epollfd;
	//resource
	ThreadPool* TPptr;	//�̳߳�ָ��
	uint TP_MAX_NUM;	//�̳߳����߳�����

	SQLPool* SQLptr;	 //SQL��ָ��
	uint SQLPOOL_MAX_NUM;//SQL���е�Ԥ������
	string sqlName;		//SQL�����û���
	string sqlpasswd;	//SQL�����û�����
	string sqldabase;	//SQL����ʹ�õ����ݿ�����

	HttpConn* connfd;  //һ���������ӵ�ָ��

};
