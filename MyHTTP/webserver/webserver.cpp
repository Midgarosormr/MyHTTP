#pragma once
#include "WebServer.h"
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <fcntl.h>
#include <pthread.h>

int WebServer::clientConnCount = 0;

WebServer::~WebServer(){}

bool WebServer::initWebServer() {
	//change the current working directory
	if (chdir(root.c_str())<0) {
		//std::cout << "can't change directory" << std::endl;
	};
	if (m_ListenPort > 65535 || m_ListenPort < 1024) {
		return false;
	}
	connEvent_= EPOLLONESHOT | EPOLLRDHUP;

	serverStart();
}


//1
void WebServer::serverStart() {
	//std::cout << "m_listenfd beging" << std::endl;	//

	m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;
	socklen_t addresslen = sizeof(address);
	int ret = 0;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(m_ListenPort);
	ret = bind(m_listenfd, (sockaddr*)&address, addresslen);
	ret = listen(m_listenfd, 128);

	//std::cout << "m_listenfd listed" << std::endl;	//

	setFdNonblock(m_listenfd);	//设置监听FD为非阻塞
	m_epollfd = epoll_create(MAX_EVENT_NUMBER);
	setFdNonblock(m_epollfd);
	epoll_event EPevents;
	EPevents.events = EPOLLIN|EPOLLRDHUP;
	EPevents.data.fd = m_listenfd;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &EPevents);
	for (;;) {
		int epnum = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
		for (int i = 0;i < epnum;i++) {
			int clientfd = events[i].data.fd;
			if (clientfd == m_listenfd) { //监听端口出现新连接
				if (clientConnCount > MAX_EVENT_NUMBER) //连接数过多，等待下一次接受连接
					continue;
				sockaddr_in clientsockaddr;
				socklen_t clientsockaddrlen = sizeof(clientsockaddr);
				int newclientfd = accept(clientfd, (sockaddr*)&clientsockaddr,&clientsockaddrlen);
				if (newclientfd < 0) { return; }
				dealNewConn(newclientfd, clientsockaddr);
			}
			else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) { //error
				close(clientfd);
			}
			else if (events[i].events & EPOLLIN) {	//有数据到来
				//debug context begin
				//std::cout<< "WebServer::serverStart() -> for(;;) ->(events[i].events & EPOLLIN):\t&userlist[clientfd]:"<< &userlist[clientfd] <<std::endl;
				//debug context end

				dealRead(&userlist[clientfd]);
			}
			else if (events[i].events & EPOLLOUT){	//有数据可写
				//debug context begin
				//std::cout << "WebServer::serverStart() -> for(;;) ->(events[i].events & EPOLLOUT):\t&userlist[clientfd]:" << &userlist[clientfd]<< std::endl;
				//debug context end
				dealWrite(&userlist[clientfd]);
			}
		}
	};
	
};

//1
bool WebServer::dealNewConn(int clientfd, sockaddr_in clientaddres) {
	userlist[clientfd] = HttpConn(clientfd, clientaddres);
	clientConnCount++;
	epoll_event clientOpt;
	clientOpt.data.fd = clientfd;
	clientOpt.events = connEvent_|EPOLLIN|EPOLLONESHOT|EPOLLET;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, clientfd, &clientOpt);  //新客户连接注册进epoll事件表
	setFdNonblock(clientfd);	//设置为非阻塞
	return true;
};

//1
bool WebServer::dealRead(HttpConn* hc) { //主线程将读任务添加到线程池任务队列
	//debug context begin
	//std::cout << "WebServer::dealRead(HttpConn& hc)->hc.m_fd:" << hc->m_fd << std::endl;
	//debug context end
	TPptr->addTask(std::bind(&WebServer::onRead, this, hc));
	pthread_cond_signal(&TPptr->cond);
	return true;
};

//1
bool WebServer::dealWrite(HttpConn* hc) {	//主线程将写任务添加到线程池任务队列
	TPptr->addTask(std::bind(&WebServer::onWrite,this,hc));
	pthread_cond_signal(&TPptr->cond);
	return true;
};

//1
bool WebServer::onRead(HttpConn* hc) {	//工作线程处理读任务
	//debug context begin
	//std::cout <<"\t|_:ONRead Begin" << std::endl;
	//debug context end

	if (!hc->onWork_request()) {	//失败
		//1.在epoll里删除自己的监听
		//2.关闭自己的描述符
		
		//debug context begin
		//std::cout << pthread_self() << ":hc.onWork_request() is false" << std::endl;
		//debug context end

		epoll_ctl(m_epollfd,EPOLL_CTL_DEL,hc->m_fd,NULL);
		close(hc->m_fd);
		return false;
	}
	if (!hc->onWork_response()) {	//把回应数据存入BUFF
		//debug context begin
		//std::cout << "\t|_hc.onWork_request() is false" << std::endl;
		//debug context end
		epoll_ctl(m_epollfd, EPOLL_CTL_DEL, hc->m_fd, NULL);
		close(hc->m_fd);
		return false;
	}
	//此时已解析完了用户请求，待发送数据已存在Buff中，更改hc的epoll监听事件为写事件
	epoll_event ev;
	ev.data.fd = hc->m_fd;
	ev.events = connEvent_ | EPOLLOUT;
	epoll_ctl(m_epollfd,EPOLL_CTL_MOD,hc->m_fd,&ev);
	
	//debug context begin
	//std::cout << pthread_self() << ":ONRead End,ReadWork is True" << std::endl;
	//debug context end
	return true;

};

bool WebServer::onWrite(HttpConn* hc) {	//工作线程处理写任务
	//debug context begin
	//std::cout <<"\t|_:ONWrite" << std::endl;
	//debug context end
	int ret = 0;
	int err = 0;
	if ((ret=hc->responseData.onSendfd(hc->getwriteBuff(), &err)) < 0) {
		if (err == EAGAIN) {
			epoll_event ev;
			ev.data.fd = hc->m_fd;
			ev.events = connEvent_ | EPOLLOUT;
			epoll_ctl(m_epollfd, EPOLL_CTL_MOD, hc->m_fd, &ev);
			return true;
		}
	}
	//数据传输完成
	epoll_ctl(m_epollfd, EPOLL_CTL_DEL, hc->m_fd, NULL);
	close(hc->m_fd);
	clientConnCount--;	
	//debug context begin
	//std::cout << pthread_self() << ":ONWrite End" << std::endl;
	//debug context end
	return true;
};

int WebServer::setFdNonblock(int fd) {
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
};
