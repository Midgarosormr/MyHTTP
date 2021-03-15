/*
 * @Author       : SharkZ
 * @Date         : 2021-02-21
 */

#pragma once
#include "ThreadPool.h"
#include <unordered_map>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string>
#include "../log/LOG.h"
#include "../http/HttpConn.h"
#include "SQLPool.h"

using std::string;

int MAX_FD = 65536;           //����ļ�������
const int MAX_EVENT_NUMBER = 10000; //����¼���

class WebServer	
{
public:
	WebServer(int port,		//�����˿�:80/8080
		int TPnums,		//�̳߳��߳���
		string user_db, string passwd_db, string name_db, int sqlpool_nums, //SQL �û���/����/ʹ�õ����ݿ�����
		bool islog)		//�Ƿ���LOGϵͳ 
		:m_ListenPort(port),TP_MAX_NUM(TPnums),sqlName(user_db),sqlpasswd(passwd_db),sqldabase(name_db),SQLPOOL_MAX_NUM(sqlpool_nums),m_islog(islog)
		, TPptr(new ThreadPool(TP_MAX_NUM,MAX_EVENT_NUMBER)),SQLPptr(new SQLPool()),LOGptr(new LOG())
	{
		initWebServer();
	};
	~WebServer();
	bool initWebServer();
	bool initThreadPoll();
	bool initSQLPoll();
	bool initLog();
	void serverStart();
	int setFdNonblock(int fd);

public:
	//Server's own attribute.
	int m_listenfd;	//�����ļ�������
	in_port_t m_ListenPort; //�����˿�
	
	int m_epollfd;
	epoll_event events[MAX_EVENT_NUMBER]; //
	uint32_t connEvent_;//Ĭ�����ӵ��¼���������
	
	bool m_islog;	//�Ƿ�����LOGϵͳ
	
	std::unordered_map<int, HttpConn> userlist;
	static int clientConnCount; //�����������ӵ��û�����
	
	//RESOURCE PATH 
	char* root = "/var/MyHTTP";
	char* logDir="/var/log/MyHTTP";	//LOGϵͳ��¼·��

	//�̳߳����
	std::unique_ptr<ThreadPool> TPptr;	//�̳߳�ָ��
	int TP_MAX_NUM;	//�̳߳����߳�����

	//SQL�����
	std::unique_ptr<SQLPool> SQLPptr;	 //SQL��ָ��
	int SQLPOOL_MAX_NUM;//SQL���е�Ԥ������
	string sqlName;		//SQL�����û���
	string sqlpasswd;	//SQL�����û�����
	string sqldabase;	//SQL����ʹ�õ����ݿ�����
	
	//LOG���
	std::unique_ptr<LOG> LOGptr;

private:
	bool dealNewConn(int clientfd, sockaddr_in clientaddres);
	bool dealRead(HttpConn& hc);
	bool dealWrite(HttpConn& hc);
	bool onRead(HttpConn& hc);
	bool onWrite(HttpConn& hc);

};

int WebServer::clientConnCount = 0;