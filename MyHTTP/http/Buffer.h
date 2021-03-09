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
	explicit Buffer(int fd);
	~Buffer();

	//operate
	int read2fd();							//从套接字fd读入数据到Buffer::buff_
	int write2fd();							//从Buffer::buff_写入数据到套接字fd

	int readBuff(void* dest, int readSize);
	int writeBuff(void* writebuff,int writeSize); //写入数据到Buffer::buff_

public:
	int m_fd;
	int m_buffRWPos;		//用户需要读/写buff_数据时的指针
	int m_buffPos;			//从内核缓冲区写数据到buff_时的指针
	
	int m_nextbuffRWPos;
	int m_nextbuffPos;

	int m_buffSize;			//缓冲区大小
	bool isfull;			//缓冲区无可用空间(仅Buffer类当做写缓冲区才使用),此变量用来判断是否需要使用备用buff_

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
	
};

