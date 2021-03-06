#include "HttpConn.h"

HttpConn::HttpConn(int fd,sockaddr_in clientaddr):m_fd(fd),addr(clientaddr),readBuff(fd),writeBuff(fd){
	
};
