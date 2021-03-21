#include "Buffer.h"
#include <unistd.h>
#include <sys/socket.h>
#include <malloc.h>
#include <memory.h>
#include <sys/uio.h>
#include <iostream>
#include <error.h>

/*
		//n
		//returnType function(...){};

	n=0		->		funtion was uncomplete;
	n=1		->		funtion was complete;
	n=1-0	->		function was complete,but isn't prefect;
*/

Buffer::Buffer() :m_fd(-1), m_buffRWPos(0), m_buffPos(0), m_buffSize(2048), m_nextbuffPos(0), m_nextbuffRWPos(0) {
	//debug context begin
	std::cout << std::endl <<"Buffer::Buffer() Ctor" << std::endl;
	//debug context end
	
	nextbuff_ = nullptr;
	isfull = false;
};

//1
Buffer::Buffer(int fd) :m_fd(fd), m_buffRWPos(0),m_buffPos(0), m_buffSize(2048),m_nextbuffPos(0),m_nextbuffRWPos(0){
	//debug context begin
	std::cout << std::endl << "Buffer::Buffer(int fd) Ctor" << std::endl;
	//debug context end
	
	nextbuff_ = nullptr;
	isfull = false;
};

Buffer::Buffer(const Buffer& cpBuff) {
	//debug context begin
	std::cout << std::endl << "Buffer::Buffer(const Buffer& cpBuff) CP_Ctor" << std::endl;
	//debug context end
	m_fd = cpBuff.m_fd;
	m_buffRWPos=cpBuff.m_buffRWPos;
	m_buffPos = cpBuff.m_buffPos;
	m_nextbuffRWPos = cpBuff.m_nextbuffRWPos;
	m_nextbuffPos = cpBuff.m_nextbuffPos;
	m_buffSize = cpBuff.m_buffSize;
	strncpy(buff_, cpBuff.buff_,m_buffSize);
	if (cpBuff.nextbuff_ != nullptr) {
		nextbuff_ = new char[m_buffSize];
		strncpy(nextbuff_, cpBuff.nextbuff_, m_buffSize);
	}
	else
	{
		nextbuff_ = nullptr;
	}
	isfull = cpBuff.isfull;
}

//1
Buffer::~Buffer() {
	if (nextbuff_ != nullptr) write2fd();	//正常情况下永远不会触发
}
int Buffer::getFd()
{
	return m_fd;
}
int Buffer::setFd(int fd)
{
	return m_fd=fd;
}
;

//1
int Buffer::read2fd() {
	int nleft;
	ssize_t ntoread=0;
	nleft = m_buffSize-m_buffPos;
	while (nleft > 0)
	{
		if ( (ntoread = read(m_fd, buff_, nleft) ) <0) {
			if (errno == EAGAIN) break;
			if (nleft == (m_buffSize - m_buffPos)) {
				return -1;	//error
			}
			else
				break;
		}
		if (ntoread == 0) {
			break;
		}
		nleft -= ntoread;
		m_buffPos += ntoread;
	}

	return m_buffPos;
};

//1
int Buffer::write2fd() {
	int ntoWrite=0;
	int buffAbsWrite = m_buffPos - m_buffRWPos;
	if (nextbuff_ != nullptr) {		//存在备用buff_,使用集中写
		int nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
		struct iovec temp_iovec[2];
		temp_iovec[0].iov_base = &buff_[m_buffRWPos];
		temp_iovec[0].iov_len = buffAbsWrite;
		temp_iovec[1].iov_base = &nextbuff_[m_nextbuffRWPos];
		temp_iovec[1].iov_len = nextbuffAbsWrite;
		int nleft = buffAbsWrite + nextbuffAbsWrite;
		int n = nleft;
		while (nleft>0)
		{
			if (ntoWrite = writev(m_fd, temp_iovec, 2) < 0) {
				if (ntoWrite == n)
					return -1;	//error
				else
					break;
			}else if(ntoWrite==0){	//EOF
				break;
			}
			if (ntoWrite != buffAbsWrite + nextbuffAbsWrite) {	//没全部写完,更新buff指针
				if ((buffAbsWrite != 0)&&(ntoWrite >= buffAbsWrite)) {	//buff_缓冲区写完，nextbuff_没有完全写完
					nleft -= ntoWrite;
					m_buffRWPos = m_buffPos = 0;
					isfull = false;
					ntoWrite -= buffAbsWrite;	//nextbuff_已写入内核的数据大小
					temp_iovec[0].iov_len = buffAbsWrite = 0;
					m_nextbuffRWPos += ntoWrite;
					temp_iovec[1].iov_base = &nextbuff_[m_nextbuffRWPos];
					temp_iovec[1].iov_len = nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
				}
				else if ((buffAbsWrite == 0)&& (ntoWrite >= buffAbsWrite)) {
					nleft -= ntoWrite;
					m_nextbuffRWPos += ntoWrite;
					temp_iovec[1].iov_base = &nextbuff_[m_nextbuffRWPos];
					temp_iovec[1].iov_len = nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
				}
				else {
					nleft -= ntoWrite;
					m_buffRWPos += ntoWrite;
					temp_iovec[0].iov_base= &buff_[m_buffRWPos];
					buffAbsWrite = m_buffPos - m_buffRWPos;
					temp_iovec[0].iov_len = buffAbsWrite;
				}			
			}
		}
		isfull = false;
		free(nextbuff_);	//备用缓冲区内容已写完，释放资源
		nextbuff_ = nullptr;	//重置备用缓冲区为空,注意永不在~Buffer()里调用free
		return (n-nleft);
	}
	//只存在默认buff_
	else {
		int nleft=m_buffPos-m_buffRWPos;
		int n = nleft;
		while (nleft>0)
		{
			if ((ntoWrite = write(m_fd, buff_ + m_buffRWPos, nleft)) < 0) {
				if (nleft == n)
					return -1;
				else
					break;
			}
			else if (ntoWrite == 0)	//EOF
					break;
			//未读完，更新信息继续下一次循环
			nleft -= ntoWrite;
			m_buffRWPos += ntoWrite;
		}
		isfull = false;
		return (n - nleft);
	}
};

//1
//updata:2021-3-13   此函数未使用
int Buffer::readBuff(void* readbuff,int readSize) {
	int readableSize = m_buffPos - m_buffRWPos;
	if (readSize >= readableSize) {
		readSize = readableSize;
		memcpy(readbuff, buff_ + m_buffRWPos, readSize);
		m_buffRWPos = m_buffPos = 0;
	}
	else {
		memcpy(readbuff, buff_ + m_buffRWPos, readSize);
		m_buffRWPos += readSize;
	}
	return readSize;
};

//1
int Buffer::writeBuff(void* writebuff,int writeSize) {	//优先写默认buff
	if (!isfull) {	//默认buff未满
		int writeableSize = m_buffSize - m_buffPos;
		if (writeSize > writeableSize) {	//写入数据大小超过默认buff剩余大小，申请弹性buff"nextbuff_"
			memcpy(buff_ + m_buffPos, writebuff, writeableSize);
			isfull = true;
			nextbuff_ = new char[writeSize-writeableSize];
			memcpy(nextbuff_, writebuff+writeableSize, writeSize - writeableSize);
			return 0;	//true
		}
	}
	else {
		nextbuff_ = new char[writeSize];
		memcpy(nextbuff_, writebuff, writeSize);
		return 0;	//true
	}
};

char* Buffer::getBeginPos() {
	return buff_+m_buffRWPos;
};

char* Buffer::getEndPOS() {
	return buff_ + m_buffPos;
};

bool Buffer::adjustReadByte(int n) {
	if ((m_buffRWPos + n) <=m_buffPos) {
		m_buffRWPos+= n;
		if (m_buffRWPos == m_buffPos) m_buffRWPos = m_buffPos = 0;
		return true;
	}else
	return false;
};

int Buffer::readable() {
	return (m_buffPos-m_buffRWPos);
	// >0,可读
	//=0,无数据
};