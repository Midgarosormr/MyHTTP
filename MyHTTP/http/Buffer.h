/*
 * @Author       : SharkZ
 * @CreateDate   : 2021-03-05
 * @Update		 : 2021-03-09
 */
/*
	clientfdΪ������,epoll�����¼�����ΪET
*/

#pragma once
#include <sys/uio.h>

class Buffer
{
public:
	Buffer();
	Buffer(int fd);
	Buffer(const Buffer& cpBuffer);
	~Buffer();
	int getFd();
	int setFd(int fd);
	//operate
	int read2fd();							//���׽���fd�������ݵ�Buffer::buff_
	int write2fd(int* err);							//��Buffer::buff_д�����ݵ��׽���fd

	int readBuff(void* dest, int readSize);
	int writeBuff(void* writebuff,int writeSize); //д�����ݵ�Buffer::buff_

	char* getBeginPos();	//����ָ���һ���ɶ���ָ��
	char* getEndPOS();	//����ָ���β��ָ��
	bool adjustReadByte(int readsize);	//��ʾ˵���Ѷ��������ֽ����������Ѷ����ָ��m_buffRWPos
	int readable();

public:
	iovec vec[2];
	int m_fd;
	volatile int m_buffRWPos;		//�û���Ҫ��/дbuff_����ʱ��ָ��
	volatile int m_buffPos;			//���ں˻�����д���ݵ�buff_ʱ��ָ��
	
	volatile int m_nextbuffRWPos;
	volatile int m_nextbuffPos;
	bool isfull;			//�������޿��ÿռ�(��Buffer�൱��д��������ʹ��),�˱��������ж��Ƿ���Ҫʹ�ñ���buff_
	int m_buffSize;			//��������С

/*
buff_: 
					   ��buffPos		  ��buffSize
		[ ][ ][*][*][*][ ][ ][ ][ ][ ][  ][  ]
		 0  1  2  3  4  5  6  7  8  9  10  11
			  ��buffRWPos
*/

	char buff_[2048];
	char* nextbuff_;	//����buff���ṩ���Կռ�
	

};

