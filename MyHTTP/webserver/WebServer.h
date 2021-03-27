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
#include <iostream>

using std::string;

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 10000; //最大事件数



class WebServer	
{
public:
	WebServer(int port=8000,		//监听端口:80/8080
		int TPnums=4,		//线程池线程数
		string user_db="", string passwd_db = "", string name_db="", int sqlpool_nums=4, //SQL 用户名/密码/使用的数据库名称
		bool islog=0)		//是否开启LOG系统 
		:m_ListenPort(port),TP_MAX_NUM(TPnums),sqlName(user_db),sqlpasswd(passwd_db),sqldabase(name_db),SQLPOOL_MAX_NUM(sqlpool_nums),m_islog(islog)
		, TPptr(new ThreadPool(TP_MAX_NUM,MAX_EVENT_NUMBER)),SQLPptr(new SQLPool()),LOGptr(new LOG()),userlist(MAX_EVENT_NUMBER)
	{

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
	int m_listenfd;	//监听文件描述符
	in_port_t m_ListenPort; //监听端口
	
	int m_epollfd;
	epoll_event events[MAX_EVENT_NUMBER]; //
	uint32_t connEvent_;//默认连接的事件监听类型
	
	bool m_islog;	//是否启动LOG系统
	
	std::unordered_map<int, HttpConn> userlist;
	static int clientConnCount; //服务器已连接的用户数量
	
	//RESOURCE PATH 
	string root = "/var/MyHTTP/html";
	string logDir="/var/log/MyHTTP";	//LOG系统记录路径

	//线程池相关
	std::unique_ptr<ThreadPool> TPptr;	//线程池指针
	int TP_MAX_NUM;	//线程池中线程数量

	//SQL池相关
	std::unique_ptr<SQLPool> SQLPptr;	 //SQL池指针
	int SQLPOOL_MAX_NUM;//SQL池中的预连接数
	string sqlName;		//SQL连接用户名
	string sqlpasswd;	//SQL连接用户密码
	string sqldabase;	//SQL连接使用的数据库名称
	
	//LOG相关
	std::unique_ptr<LOG> LOGptr;

private:
	bool dealNewConn(int clientfd, sockaddr_in clientaddres);
	bool dealRead(HttpConn* hc);
	bool dealWrite(HttpConn* hc);
	bool onRead(HttpConn* hc);
	bool onWrite(HttpConn* hc);

};

