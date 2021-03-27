#include "ThreadPool.h"
#include <iostream>

	
ThreadPool::~ThreadPool() {
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	free(m_Threads);
};

bool ThreadPool::init() {
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	m_Threads = new pthread_t[m_Threadnums];
	for (int i = 0;i < m_Threadnums;i++) {
		int ret=pthread_create(&m_Threads[i], nullptr, onProcess,(void*)this);
		if (ret)return false;	//ret==0 is true,!0 is error
		if (pthread_detach(m_Threads[i])) return false;
	}
	return true;
};

void* ThreadPool::onProcess(void* argv) {
	ThreadPool* self = (ThreadPool*)argv;
	for (;;) {
		pthread_mutex_lock(&self->mutex);
		while (self->workqueue.empty()) {	//任务队列为空
			pthread_cond_wait(&self->cond, &self->mutex);
		}
		auto task = std::move(self->workqueue.front());
		self->workqueue.pop();
		pthread_mutex_unlock(&self->mutex);
		//debug context begin
		//std::cout << pthread_self() << std::endl;
		//debug context end
		task(); //执行具体的"读/写任务"

	}
};

bool ThreadPool::addTask(std::function<bool()>&& task) {
	pthread_mutex_lock(&mutex);
	workqueue.emplace(std::forward<std::function<bool()>>(task));	
	pthread_mutex_unlock(&mutex);
	return true;
};