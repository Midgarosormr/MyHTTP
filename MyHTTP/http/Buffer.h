/*
 * @Author       : SharkZ
 * @Date         : 2021-03-05
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
	int read2buff();
	int write2buff(void* writebuff,int writeSize);

public:
	int m_fd;
	int m_buffPos;			//已存入buff_中的数据大小
	int m_nextbuffPos;
	int m_buffSize;			//缓冲区大小


private:
	char buff_[2048];
	char* nextbuff_;	//备用buff，提供弹性空间
	
};

