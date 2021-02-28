#pragma once
#include "WebServer.h"
#include <sys/socket.h>
#include <sys/epoll.h>
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
	initThreadPoll();
	initSQLPoll();
	initLog();
	serverStart();
};

bool WebServer::initThreadPoll() {
	TPptr = new ThreadPool();
	if (!TPptr) return false;
	return true;
};

bool WebServer::initSQLPoll() {
	SQLPptr = new SQLPool();
	if (!SQLPptr)return false;
	return true;
};

bool WebServer::initLog() {
	LOGptr = new LOG();
	if (!LOGptr)return false;
	return true;
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
	ret = listen(m_listenfd, 1000);

	m_epollfd = epoll_create(MAX_EVENT_NUMBER+1);
	epoll_event EPevents;
	EPevents.events = EPOLLIN;
	EPevents.data.fd = m_listenfd;
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &EPevents);
	for (;;) {
		int epnum = epoll_wait(m_epollfd, events, MAX_EVENT_NUMBER, -1);
		for (int i = 0;i < epnum;i++) {
			int clientfd = events[i].data.fd;
			if (clientfd == m_listenfd) { //监听端口出现新连接
				sockaddr_in clientsockaddr;
				socklen_t clientsockaddrlen = sizeof(clientsockaddr);
				int newclientfd = accept(clientfd, (sockaddr*)&clientsockaddr,&clientsockaddrlen);
				epoll_event clientOpt;
				clientOpt.data.fd=clientfd;
				clientOpt.events = EPOLLIN|EPOLLONESHOT;
				epoll_ctl(m_epollfd, EPOLL_CTL_ADD, clientfd, &clientOpt);  //新客户连接注册进epoll事件表
				int old_option = fcntl(clientfd, F_GETFD);
				fcntl(clientfd, F_SETFD, old_option | O_NONBLOCK);	//设置为非阻塞
			}
			else if (events[i].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) { //error
				close(clientfd);
			}
			else if (events[i].events & EPOLLIN) {	//有数据到来
				onRead(clientfd);
			}
			else if (events[i].events & EPOLLOUT){	//有数据可写
				onWrite(clientfd);
			}
		}
	};
};