#pragma once
#include "WebServer.h"
#include <sys/socket.h>

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <fcntl.h>
bool WebServer::initWebServer() {
	//change the current working directory
	if (chdir(root)<0) {
		std::cout << "can't change directory" << std::endl;
	};
	if (m_listenfd > 65535 ||m_listenfd < 1024) {
		return false;
	}
	serverStart();
};

void WebServer::serverStart() {
	m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;
	socklen_t addresslen = sizeof(address);
	int ret = 0;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(m_ListenPort);
	ret = bind(m_listenfd, (sockaddr*)&address, addresslen);
	ret = listen(m_listenfd, 128);
	setFdNonblock(m_listenfd);	//设置监听FD为非阻塞
	m_epollfd = epoll_create(MAX_EVENT_NUMBER);
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
				dealRead(&userlist[i]);
			}
			else if (events[i].events & EPOLLOUT){	//有数据可写
				dealWrite(&userlist[i]);
				clientConnCount--;	//任务完成，释放连接
				close(clientfd);
			}
		}
	};
};

void WebServer::dealNewConn(int clientfd, sockaddr_in clientaddres) {
	userlist[clientfd] =  HttpConn(clientfd, clientaddres);
	clientConnCount++;
	epoll_event clientOpt;
	clientOpt.data.fd = clientfd;
	clientOpt.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, clientfd, &clientOpt);  //新客户连接注册进epoll事件表
	setFdNonblock(clientfd);	//设置为非阻塞
};

void WebServer::dealRead(HttpConn* hc) { //主线程将读任务添加到任务队列
	TPptr->addTask(std::bind(&WebServer::onRead,this,hc));
	auto f = std::bind(&WebServer::onRead, this, hc);
};

void WebServer::dealWrite(HttpConn* hc) {	//主线程将写任务添加到任务队列
	TPptr->addTask(std::bind(&WebServer::onWrite,this,hc));
};

void WebServer::onRead(HttpConn* hc) {	//工作线程处理读任务

};
void WebServer::onWrite(HttpConn* hc) {	//工作线程处理写任务



};
int WebServer::setFdNonblock(int fd) {
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
};
