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
	//std::cout << std::endl <<"Buffer::Buffer() Ctor" << std::endl;
	//debug context end
	
	nextbuff_ = nullptr;
	isfull = false;
};

//1
Buffer::Buffer(int fd) :m_fd(fd), m_buffRWPos(0),m_buffPos(0), m_buffSize(2048),m_nextbuffPos(0),m_nextbuffRWPos(0){
	//debug context begin
	//std::cout << std::endl << "Buffer::Buffer(int fd) Ctor" << std::endl;
	//debug context end
	
	nextbuff_ = nullptr;
	isfull = false;
};

Buffer::Buffer(const Buffer& cpBuff) {
	//debug context begin
	//std::cout << std::endl << "Buffer::Buffer(const Buffer& cpBuff) CP_Ctor" << std::endl;
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
	if (nextbuff_ != nullptr) free(nextbuff_);	//正常情况下永远不会触发
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
		if ( (ntoread = read(m_fd, buff_+m_buffPos, nleft) ) <0) {
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
int Buffer::write2fd(int* err) {
	int ntoWrite=0;
	int buffAbsWrite = m_buffPos - m_buffRWPos;
	if (nextbuff_ != nullptr) {		//存在备用buff_,使用集中写
		//debug context begin
		//std::cout << std::endl <<"["<<m_fd<<"]"<<"Buffer::write2fd() -> (nextbuff_ != nullptr) " << std::endl;
		//debug context end
		int nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
		vec[0].iov_base = &buff_[m_buffRWPos];
		vec[0].iov_len = buffAbsWrite;
		vec[1].iov_base = &nextbuff_[m_nextbuffRWPos];
		vec[1].iov_len = nextbuffAbsWrite;
		int nleft = buffAbsWrite + nextbuffAbsWrite;
		int n = nleft;
		while (nleft>0)
		{
			//debug context begin	
			//std::cout << "\n\t(ntoWrite = writev(m_fd, vec, 2) )\n\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
			//debug context end	
			if ((ntoWrite = writev(m_fd, vec, 2) )< 0) {
				if (errno == EAGAIN) {
					//debug context begin
					//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> (nextbuff_ != nullptr)-> EAGAIN " << std::endl;
					//std::cout << "\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
					//debug context end	
					*err = errno;
					return ntoWrite;
				}
				if (nleft == n)
					return -1;	//error
				else
					break;
			}
			else if(ntoWrite==0){	//EOF
				//debug context begin
				//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> (nextbuff_ != nullptr)-> EOF " << std::endl;
				//debug context end	
				break;
			}
			else if (ntoWrite != (buffAbsWrite + nextbuffAbsWrite) ) {	//没全部写完,更新buff指针
				//debug context begin
				//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> (nextbuff_ != nullptr)-> line 125 " << std::endl;
				//debug context end					
				if ((buffAbsWrite != 0)&&(ntoWrite >= buffAbsWrite)) {	//buff_缓冲区写完，nextbuff_没有完全写完
					//debug context begin
					//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> (nextbuff_ != nullptr)-> line 129 " << std::endl;
					//std::cout << "\tupdate before\n\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
					//debug context end		
					nleft -= ntoWrite;
					m_buffRWPos = m_buffPos = 0;
					isfull = false;
					ntoWrite -= buffAbsWrite;	//nextbuff_已写入内核的数据大小
					vec[0].iov_len = buffAbsWrite = 0;
					m_nextbuffRWPos += ntoWrite;
					vec[1].iov_base = &nextbuff_[m_nextbuffRWPos];
					vec[1].iov_len = nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
					//debug context begin	
					//std::cout << "\n\tupdate after\n\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
					//debug context end		
					continue;
				}
				else if (buffAbsWrite == 0) {	
					//debug context begin
					//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> (nextbuff_ != nullptr)-> line 142 " << std::endl;
					//debug context begin	
					//std::cout << "\tupdate before\n\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
					//debug context end		
					//debug context end	
					nleft -= ntoWrite;
					m_nextbuffRWPos += ntoWrite;
					nextbuffAbsWrite = m_nextbuffPos - m_nextbuffRWPos;
					//debug context begin
					//std::cout<<"\n\tupdate after\n\t|_nextbuffRWPos:"<<m_nextbuffRWPos<< "\n\t|_nextbuffPos:"<<m_nextbuffPos <<"\n\t|_nextbuffAbsWrite:"<<nextbuffAbsWrite<< std::endl;
					//debug context end	
					if (nextbuffAbsWrite == 0) break;
					vec[0].iov_len = buffAbsWrite = 0;
					vec[1].iov_base = &nextbuff_[m_nextbuffRWPos];
					vec[1].iov_len = nextbuffAbsWrite;
					continue;
				}
				else { //写入小于 默认Buff_可写空间，notowrite< m_buffABSWrite
					//debug context begin
					//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> (nextbuff_ != nullptr)-> line 153 " << std::endl;
					//debug context begin	
					//std::cout << "\tupdate before\n\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
					//debug context end		
					nleft -= ntoWrite;
					m_buffRWPos += ntoWrite;
					vec[0].iov_base= &buff_[m_buffRWPos];
					vec[0].iov_len = buffAbsWrite = m_buffPos - m_buffRWPos;
					//debug context begin	
					//std::cout << "\n\tupdate after\n\t|_nextbuffRWPos:" << m_nextbuffRWPos << "\n\t|_nextbuffPos:" << m_nextbuffPos << "\n\t|_nextbuffAbsWrite:" << nextbuffAbsWrite << std::endl;
					//debug context end		
					continue;
				}			
			}
			nleft -= ntoWrite;
		}

		isfull = false;
		free(nextbuff_);	//备用缓冲区内容已写完，释放资源
		nextbuff_ = nullptr;	//重置备用缓冲区为空,注意永不在~Buffer()里调用free

		return nleft;	//数据全部写完应当返回0
		
	}
	//只存在默认buff_
	else {
		//debug context begin
		//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() ->只存在默认buff_ -> line 169 " << std::endl;
		//debug context end	
		int nleft=m_buffPos-m_buffRWPos;
		int n = nleft;
		while (nleft>0)
		{
			if ((ntoWrite = write(m_fd, buff_ + m_buffRWPos, nleft)) < 0) {
				if (errno == EAGAIN) {
					//debug context begin
					//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> nextbuff_ == nullptr -> EAGAIN " << std::endl;
					//debug context end	
					*err = errno;
					return ntoWrite;
				}
				if (nleft == n )
					return -1;
				else
					break;
			}
			else if (ntoWrite == 0) {	//EOF
				//debug context begin
				//std::cout << std::endl << "[" << m_fd << "]" << "|_Buffer::write2fd() -> nextbuff_ == nullptr -> EOF " << std::endl;
				//debug context end	
				break;
			}
			//未读完，更新信息继续下一次循环
			nleft -= ntoWrite;
			m_buffRWPos += ntoWrite;
		}
		isfull = false;
		return nleft;
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
			m_buffPos += writeableSize;
			isfull = true;
			nextbuff_ = new char[writeSize-writeableSize];
			memcpy(nextbuff_, writebuff+writeableSize, writeSize - writeableSize);
			m_nextbuffPos = writeSize - writeableSize;
			return 0;	//true
		}
		else {
			memcpy(buff_ + m_buffPos, writebuff, writeSize);
			m_buffPos += writeSize;
			if (m_buffPos == m_buffSize) isfull = true;
			return 0;
		}
	}
	else {
		nextbuff_ = new char[writeSize];
		memcpy(nextbuff_, writebuff, writeSize);
		m_nextbuffPos += writeSize;
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