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
	WebServer(int port,		//监听端口:80/8080
		uint TPnums,		//线程池线程数
		string user_db, string passwd_db, string name_db, uint sqlpool_nums, //SQL 用户名/密码/使用的数据库名称
		bool islog)		//是否开启LOG系统 
		:m_Listedport(port),TP_MAX_NUM(TPnums),sqlName(user_db),sqlpasswd(passwd_db),sqldabase(name_db),SQLPOOL_MAX_NUM(sqlpool_nums),m_islog(islog){};
	
	bool WebServerinit();

	~WebServer();

private:
	//Server's own attribute.
	in_port_t m_Listedport; //监听端口
	char* resouceDir;   //资源目录路径
	
	bool m_islog;	//是否启动LOG系统
	char* logDir;	//LOG系统记录路径

public:
	int m_epollfd;
	//resource
	ThreadPool* TPptr;	//线程池指针
	uint TP_MAX_NUM;	//线程池中线程数量

	SQLPool* SQLptr;	 //SQL池指针
	uint SQLPOOL_MAX_NUM;//SQL池中的预连接数
	string sqlName;		//SQL连接用户名
	string sqlpasswd;	//SQL连接用户密码
	string sqldabase;	//SQL连接使用的数据库名称

	HttpConn* connfd;  //一个请求连接的指针

};
