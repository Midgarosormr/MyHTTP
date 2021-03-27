#pragma once
#include <pthread.h>
#include "../http/HttpConn.h"
#include <pthread.h>
#include <queue>
#include <functional>

class ThreadPool
{
public:
	ThreadPool(int threadNum = 8, int max_WaitRequests = 10000) :m_Threadnums(threadNum), m_MaxWaitRequests(max_WaitRequests) { init(); };
	~ThreadPool();
	bool init();
public:
	bool addTask(std::function<bool()>&& task);
	static void* onProcess(void*);
private:

	int m_Threadnums; //�̳߳��е��߳���
	int m_MaxWaitRequests; //��������������
	
public:
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	std::queue<std::function<bool()>> workqueue; //�����߳̽�Ҫʹ�õ�������У����ݳ�ԱΪһ��"��/д��������"
	pthread_t* m_Threads; //�߳�����ָ��,���Ϊ m_Threads[m_Threadnums]

};



