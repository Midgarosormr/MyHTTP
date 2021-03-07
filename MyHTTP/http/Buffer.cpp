#include "Buffer.h"
#include <unistd.h>
#include <malloc.h>
#include <memory.h>
#include <sys/uio.h>

/*
		//n
		//returnType function(...){};

	n=0		->		funtion was uncomplete;
	n=1		->		funtion was complete;
	n=1-0	->		function was complete,but isn't prefect;
*/

//0
Buffer::Buffer(int fd) :m_fd(fd), m_buffRWPos(0),m_buffPos(0), m_buffSize(2048),m_nextbuffPos(0){
	nextbuff_ = nullptr;
};

//0
Buffer::~Buffer() {
	if (nextbuff_ != nullptr) delete[]nextbuff_;
};

//0
int Buffer::read2fd() {
	int ntoread=0;
	if ((ntoread = read(m_fd, &buff_[m_buffPos], m_buffSize-m_buffPos)) < 0) {
		//error
		if (ntoread < 0);
	}
	m_buffPos += ntoread;
	return ntoread;//true
};

//10
int Buffer::write2fd() {
	int ntoWrite=0;
	int buffAbsWrite = m_buffPos - m_buffRWPos;
	if (nextbuff_ != nullptr) {		//存在备用buff_,使用集中写
		int nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
		struct iovec temp_iovec[2];
		temp_iovec[0].iov_base = &buff_[m_buffRWPos];
		temp_iovec[0].iov_len = buffAbsWrite;
		temp_iovec[1].iov_base = &nextbuff_[m_buffRWPos];
		temp_iovec[1].iov_len = nextbuffAbsWrite;
		if (ntoWrite=writev(m_fd, temp_iovec, 2) < 0) return -1;		//error
		if (ntoWrite!=buffAbsWrite+nextbuffAbsWrite) {	//没全部写完,更新buff指针
			if (ntoWrite >= buffAbsWrite) {	//buff_缓冲区写完，nextbuff_没有完全写完
				m_buffRWPos = m_buffPos = 0;
				ntoWrite -= buffAbsWrite;	//nextbuff_已写入内核的数据大小
				m_nextbuffRWPos += ntoWrite;
				write2fd();
				return 0;
			}
			else {
				m_buffRWPos += ntoWrite;
				write2fd();
				return 0;
			}
		}//全部写完
		return 0;
	}
	//只存在默认buff_
	else if (ntoWrite = write(m_fd, &buff_[m_buffRWPos], buffAbsWrite) < 0) {	
		return -1;		//write error
	}
	else if (ntoWrite != buffAbsWrite) {
		m_buffRWPos += ntoWrite;
		write2fd();
		return 0;
	}
	return 0;
};

//0
int Buffer::readBuff(void* readbuff, int readSize) {

};

//0
int Buffer::writeBuff(void* writebuff,int writeSize) {
	if (writeSize > (m_buffSize+m_buffPos)) {	//写入数据大小超过默认buff剩余大小，申请弹性buff"nextbuff_"
		nextbuff_ = new char[writeSize];
		memcpy(nextbuff_, writebuff, writeSize);
		return 0;
	}
	//写入数据大小小于默认buff剩余大小
	memcpy(buff_+m_buffPos, writebuff, writeSize);
	return 0;
};
