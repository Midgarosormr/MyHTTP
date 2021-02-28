/*
 * @Author       : SharkZ
 * @Date         : 2021-02-21
 */

#pragma once
#include "ThreadPool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "../log/LOG.h"
#include "../http/HttpConn.h"
#include "SQLPool.h"

using std::string;

const int MAX_FD = 65536;           //����ļ�������
const int MAX_EVENT_NUMBER = 10000; //����¼���

class WebServer	
{
public:
	WebServer(int port,		//�����˿�:80/8080
		uint TPnums,		//�̳߳��߳���
		string user_db, string passwd_db, string name_db, uint sqlpool_nums, //SQL �û���/����/ʹ�õ����ݿ�����
		bool islog)		//�Ƿ���LOGϵͳ 
		:m_ListenPort(port),TP_MAX_NUM(TPnums),sqlName(user_db),sqlpasswd(passwd_db),sqldabase(name_db),SQLPOOL_MAX_NUM(sqlpool_nums),m_islog(islog)
	{
		initWebServer();
	};
	~WebServer();
	bool initWebServer();
	bool initThreadPoll();
	bool initSQLPoll();
	bool initLog();
	void serverStart();


public:
	//Server's own attribute.
	int m_listenfd;	//�����ļ�������
	in_port_t m_ListenPort; //�����˿�
	
	int m_epollfd;
	epoll_event events[MAX_EVENT_NUMBER]; //
	
	bool m_islog;	//�Ƿ�����LOGϵͳ
	
	
	//RESOURCE PATH 
	char* root = "/var/MyHTTP";
	char* logDir="/var/log/MyHTTP";	//LOGϵͳ��¼·��

	//�̳߳����
	ThreadPool* TPptr;	//�̳߳�ָ��
	uint TP_MAX_NUM;	//�̳߳����߳�����

	//SQL�����
	SQLPool* SQLPptr;	 //SQL��ָ��
	uint SQLPOOL_MAX_NUM;//SQL���е�Ԥ������
	string sqlName;		//SQL�����û���
	string sqlpasswd;	//SQL�����û�����
	string sqldabase;	//SQL����ʹ�õ����ݿ�����
	
	//LOG���
	LOG* LOGptr;

private:
	void onRead(int clientfd);
	void onWrite(int clientfd);

};
