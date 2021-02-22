#pragma once
#include <pthread.h>

class ThreadPool
{
public:
	ThreadPool(int threadNum=8,int max_WaitRequests=10000);
	~ThreadPool();

private:
	int m_Threadnums; //线程池中的线程数
	int m_MaxWaitRequests; //允许的最大请求数
	pthread_t* m_Threads; //线程数组指针,最大为 m_Threads[m_Threadnums]


};

