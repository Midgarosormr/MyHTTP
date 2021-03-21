/*
 * @Author       : SharkZ
 * @CreateDate   : 2021-03-05
 * @Update		 : 2021-03-09
 */
/*
	clientfd为非阻塞,epoll监听事件类型为LT
*/

#pragma once

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
	int read2fd();							//从套接字fd读入数据到Buffer::buff_
	int write2fd();							//从Buffer::buff_写入数据到套接字fd

	int readBuff(void* dest, int readSize);
	int writeBuff(void* writebuff,int writeSize); //写入数据到Buffer::buff_

	char* getBeginPos();	//返回指向第一个可读处指针
	char* getEndPOS();	//返回指向结尾处指针
	bool adjustReadByte(int readsize);	//显示说明已读缓冲区字节数，调整已读后的指针m_buffRWPos
	int readable();

public:
	int m_fd;
	int m_buffRWPos;		//用户需要读/写buff_数据时的指针
	int m_buffPos;			//从内核缓冲区写数据到buff_时的指针
	
	int m_nextbuffRWPos;
	int m_nextbuffPos;

	int m_buffSize;			//缓冲区大小

/*
buff_: 
					   ↓buffPos		  ↓buffSize
		[ ][ ][*][*][*][ ][ ][ ][ ][ ][  ][  ]
		 0  1  2  3  4  5  6  7  8  9  10  11
			  ↑buffRWPos
*/

private:
	char buff_[2048];
	char* nextbuff_;	//备用buff，提供弹性空间
	bool isfull;			//缓冲区无可用空间(仅Buffer类当做写缓冲区才使用),此变量用来判断是否需要使用备用buff_

};

