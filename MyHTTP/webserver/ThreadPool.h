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

	int m_Threadnums; //线程池中的线程数
	int m_MaxWaitRequests; //允许的最大请求数
	
public:
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	std::queue<std::function<bool()>> workqueue; //工作线程将要使用的任务队列，数据成员为一个"读/写操作函数"
	pthread_t* m_Threads; //线程数组指针,最大为 m_Threads[m_Threadnums]

};



