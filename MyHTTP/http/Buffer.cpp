#include "Buffer.h"
#include <unistd.h>
#include <malloc.h>
#include <memory.h>

Buffer::Buffer(int fd) :m_fd(fd),m_buffPos(0), m_buffSize(2048),m_nextbuffPos(0){
	nextbuff_ = nullptr;
};

Buffer::~Buffer() {
	if (nextbuff_ != nullptr) delete[]nextbuff_;
};

int Buffer::read2buff() {
	int ntoread=0;
	if ((ntoread = read(m_fd, &buff_[m_buffPos], m_buffSize-m_buffPos)) < 0) {
		//error
		if (ntoread < 0);
	}
	m_buffPos += ntoread;
	return ntoread;//true
};

int Buffer::write2buff(void* writebuff,int writeSize) {
	if (writeSize > (m_buffSize+m_buffPos)) {	//写入数据大小超过默认buff剩余大小，申请弹性buff"nextbuff_"
		nextbuff_ = new char[writeSize];
		memcpy(nextbuff_, writebuff, writeSize);
		return 0;
	}
	//写入数据大小小于默认buff剩余大小
	memcpy(buff_+m_buffPos, writebuff, writeSize);
	return 0;
};
