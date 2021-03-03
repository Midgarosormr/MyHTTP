#pragma once
#include <pthread.h>
#include "../http/HttpConn.h"
#include <pthread.h>
#include <queue>
#include <functional>

class ThreadPool
{
public:
	ThreadPool(int threadNum = 8, int max_WaitRequests = 10000) :m_Threadnums(threadNum), m_MaxWaitRequests(max_WaitRequests) {};
	~ThreadPool();
	bool init();
public:
	bool addTask(std::function<void()>&& task);
	static void* onProcess(void*);
private:
	pthread_mutex_t mutex;
	int m_Threadnums; //�̳߳��е��߳���
	int m_MaxWaitRequests; //��������������
	
public:
	std::queue<std::function<void()>> workqueue; //�����߳̽�Ҫʹ�õ�������У����ݳ�ԱΪһ��"��/д��������"
	pthread_t* m_Threads; //�߳�����ָ��,���Ϊ m_Threads[m_Threadnums]

};

ThreadPool::~ThreadPool() {
	pthread_mutex_destroy(&mutex);
	free(m_Threads);
};

