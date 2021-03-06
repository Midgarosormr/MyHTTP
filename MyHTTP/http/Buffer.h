/*
 * @Author       : SharkZ
 * @Date         : 2021-03-05
 */
/*
	clientfdΪ������,epoll�����¼�����ΪLT
*/

#pragma once

class Buffer
{
public:
	Buffer();
	explicit Buffer(int fd);
	~Buffer();

	//operate
	int read2buff();
	int write2buff(void* writebuff,int writeSize);

public:
	int m_fd;
	int m_buffPos;			//�Ѵ���buff_�е����ݴ�С
	int m_nextbuffPos;
	int m_buffSize;			//��������С


private:
	char buff_[2048];
	char* nextbuff_;	//����buff���ṩ���Կռ�
	
};

