#include "HttpConn.h"

//1
HttpConn::HttpConn(int fd,sockaddr_in clientaddr):m_fd(fd),addr(clientaddr),readBuff_(fd),writeBuff_(fd){
	
};

//1
int HttpConn::readBuff(){
	return readBuff_.read2fd();
};

//0
int HttpConn::writeBuff() {
	
};

//0
int HttpConn::WriteBytesCount()
{
	return 0;
}
;

//1
bool HttpConn::onWork_request() {
	if(readBuff()<0) return false;
	if (!requestData.parse(readBuff_))	return false;
	return true;
};

//0
bool HttpConn::onWork_response() {

};