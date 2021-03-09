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

//1
Buffer::Buffer(int fd) :m_fd(fd), m_buffRWPos(0),m_buffPos(0), m_buffSize(2048),m_nextbuffPos(0),m_nextbuffRWPos(0){
	nextbuff_ = nullptr;
	isfull = false;
};

//1
Buffer::~Buffer() {
	if (nextbuff_ != nullptr) write2fd();	//�����������Զ���ᴥ��
};

//1
int Buffer::read2fd() {
	int nleft;
	int ntoread=0;
	nleft = m_buffSize;
	while (nleft > 0)
	{
		if ((ntoread = read(m_fd, buff_ + m_buffPos, nleft) < 0)) {
			if (nleft == m_buffSize)
				return -1;	//error
			else
				break;
		}
		else if (ntoread == 0) {	//EOF
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
	if (nextbuff_ != nullptr) {		//���ڱ���buff_,ʹ�ü���д
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
			if (ntoWrite != buffAbsWrite + nextbuffAbsWrite) {	//ûȫ��д��,����buffָ��
				if ((buffAbsWrite != 0)&&(ntoWrite >= buffAbsWrite)) {	//buff_������д�꣬nextbuff_û����ȫд��
					nleft -= ntoWrite;
					m_buffRWPos = m_buffPos = 0;
					isfull = false;
					ntoWrite -= buffAbsWrite;	//nextbuff_��д���ں˵����ݴ�С
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
		free(nextbuff_);	//���û�����������д�꣬�ͷ���Դ
		nextbuff_ = nullptr;	//���ñ��û�����Ϊ��,ע��������~Buffer()�����free
		return (n-nleft);
	}
	//ֻ����Ĭ��buff_
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
			//δ���꣬������Ϣ������һ��ѭ��
			nleft -= ntoWrite;
			m_buffRWPos += ntoWrite;
		}
		isfull = false;
		return (n - nleft);
	}
};

//1
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
int Buffer::writeBuff(void* writebuff,int writeSize) {	//����дĬ��buff
	if (!isfull) {	//Ĭ��buffδ��
		int writeableSize = m_buffSize - m_buffPos;
		if (writeSize > writeableSize) {	//д�����ݴ�С����Ĭ��buffʣ���С�����뵯��buff"nextbuff_"
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
