#include "ThreadPool.h"

bool ThreadPool::init() {
	pthread_mutex_init(&mutex, NULL);
	m_Threads = new pthread_t[m_Threadnums];
	for (int i = 0;i < m_Threadnums;i++) {
		int ret=pthread_create(&m_Threads[i], nullptr, onProcess,(void*)this);
		if (ret)return false;	//ret==0 is true,!0 is error
		pthread_detach(m_Threads[i]);
	}
	return true;
};
void* ThreadPool::onProcess(void* argv) {
	ThreadPool* self = (ThreadPool*)argv;
	for (;;) {
		pthread_mutex_lock(&self->mutex);
		if (self->workqueue.empty()) {	//任务队列为空，解锁
			pthread_mutex_unlock(&self->mutex);
			continue;
		}
		//任务队列非空，处理任务
		auto task=std::move(self->workqueue.front());
		self->workqueue.pop();
		pthread_mutex_unlock(&self->mutex);
		task(); //执行具体的"读/写任务"
	}
};
bool ThreadPool::addTask(std::function<void()>&& task) {
	pthread_mutex_lock(&mutex);
	workqueue.emplace(std::forward<std::function<void()>>(task));	
	pthread_mutex_unlock(&mutex);
	return true;
};