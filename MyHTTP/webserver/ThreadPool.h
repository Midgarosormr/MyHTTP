#pragma once
#include <pthread.h>

class ThreadPool
{
public:
	ThreadPool(int threadNum=8,int max_WaitRequests=10000);
	~ThreadPool();

private:
	int m_Threadnums; //�̳߳��е��߳���
	int m_MaxWaitRequests; //��������������
	pthread_t* m_Threads; //�߳�����ָ��,���Ϊ m_Threads[m_Threadnums]


};

