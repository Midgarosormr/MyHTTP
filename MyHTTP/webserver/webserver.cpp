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

	setFdNonblock(m_listenfd);	//���ü���FDΪ������
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
			if (clientfd == m_listenfd) { //�����˿ڳ���������
				if (clientConnCount > MAX_EVENT_NUMBER) //���������࣬�ȴ���һ�ν�������
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
			else if (events[i].events & EPOLLIN) {	//�����ݵ���
				//debug context begin
				//std::cout<< "WebServer::serverStart() -> for(;;) ->(events[i].events & EPOLLIN):\t&userlist[clientfd]:"<< &userlist[clientfd] <<std::endl;
				//debug context end

				dealRead(&userlist[clientfd]);
			}
			else if (events[i].events & EPOLLOUT){	//�����ݿ�д
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
	epoll_ctl(m_epollfd, EPOLL_CTL_ADD, clientfd, &clientOpt);  //�¿ͻ�����ע���epoll�¼���
	setFdNonblock(clientfd);	//����Ϊ������
	return true;
};

//1
bool WebServer::dealRead(HttpConn* hc) { //���߳̽���������ӵ��̳߳��������
	//debug context begin
	//std::cout << "WebServer::dealRead(HttpConn& hc)->hc.m_fd:" << hc->m_fd << std::endl;
	//debug context end
	TPptr->addTask(std::bind(&WebServer::onRead, this, hc));
	pthread_cond_signal(&TPptr->cond);
	return true;
};

//1
bool WebServer::dealWrite(HttpConn* hc) {	//���߳̽�д������ӵ��̳߳��������
	TPptr->addTask(std::bind(&WebServer::onWrite,this,hc));
	pthread_cond_signal(&TPptr->cond);
	return true;
};

//1
bool WebServer::onRead(HttpConn* hc) {	//�����̴߳��������
	//debug context begin
	//std::cout <<"\t|_:ONRead Begin" << std::endl;
	//debug context end

	if (!hc->onWork_request()) {	//ʧ��
		//1.��epoll��ɾ���Լ��ļ���
		//2.�ر��Լ���������
		
		//debug context begin
		//std::cout << pthread_self() << ":hc.onWork_request() is false" << std::endl;
		//debug context end

		epoll_ctl(m_epollfd,EPOLL_CTL_DEL,hc->m_fd,NULL);
		close(hc->m_fd);
		return false;
	}
	if (!hc->onWork_response()) {	//�ѻ�Ӧ���ݴ���BUFF
		//debug context begin
		//std::cout << "\t|_hc.onWork_request() is false" << std::endl;
		//debug context end
		epoll_ctl(m_epollfd, EPOLL_CTL_DEL, hc->m_fd, NULL);
		close(hc->m_fd);
		return false;
	}
	//��ʱ�ѽ��������û����󣬴����������Ѵ���Buff�У�����hc��epoll�����¼�Ϊд�¼�
	epoll_event ev;
	ev.data.fd = hc->m_fd;
	ev.events = connEvent_ | EPOLLOUT;
	epoll_ctl(m_epollfd,EPOLL_CTL_MOD,hc->m_fd,&ev);
	
	//debug context begin
	//std::cout << pthread_self() << ":ONRead End,ReadWork is True" << std::endl;
	//debug context end
	return true;

};

bool WebServer::onWrite(HttpConn* hc) {	//�����̴߳���д����
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
	//���ݴ������
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
