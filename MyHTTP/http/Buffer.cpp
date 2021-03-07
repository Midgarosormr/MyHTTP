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
	if (nextbuff_ != nullptr) {		//���ڱ���buff_,ʹ�ü���д
		int nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
		struct iovec temp_iovec[2];
		temp_iovec[0].iov_base = &buff_[m_buffRWPos];
		temp_iovec[0].iov_len = buffAbsWrite;
		temp_iovec[1].iov_base = &nextbuff_[m_buffRWPos];
		temp_iovec[1].iov_len = nextbuffAbsWrite;
		if (ntoWrite=writev(m_fd, temp_iovec, 2) < 0) return -1;		//error
		if (ntoWrite!=buffAbsWrite+nextbuffAbsWrite) {	//ûȫ��д��,����buffָ��
			if (ntoWrite >= buffAbsWrite) {	//buff_������д�꣬nextbuff_û����ȫд��
				m_buffRWPos = m_buffPos = 0;
				ntoWrite -= buffAbsWrite;	//nextbuff_��д���ں˵����ݴ�С
				m_nextbuffRWPos += ntoWrite;
				write2fd();
				return 0;
			}
			else {
				m_buffRWPos += ntoWrite;
				write2fd();
				return 0;
			}
		}//ȫ��д��
		return 0;
	}
	//ֻ����Ĭ��buff_
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
	if (writeSize > (m_buffSize+m_buffPos)) {	//д�����ݴ�С����Ĭ��buffʣ���С�����뵯��buff"nextbuff_"
		nextbuff_ = new char[writeSize];
		memcpy(nextbuff_, writebuff, writeSize);
		return 0;
	}
	//д�����ݴ�СС��Ĭ��buffʣ���С
	memcpy(buff_+m_buffPos, writebuff, writeSize);
	return 0;
};
