#include "HttpConn.h"
#include <iostream>

HttpConn::HttpConn():m_fd(-1),responseData(&this->requestData){
	//debug context begin
	std::cout << "HttpConn::HttpConn() Ctor" << std::endl;
	//debug context end
};

//1
HttpConn::HttpConn(int& fd,const sockaddr_in& clientaddr):m_fd(fd),addr(clientaddr),isClose(false), responseData(&requestData),readBuff_(fd),writeBuff_(fd){
	//debug context begin
	std::cout << "HttpConn::HttpConn(int fd,sockaddr_in clientaddr) Ctor" << std::endl;
	
	std::cout << "HttpConn::HttpConn() is start" << std::endl;
	std::cout << "fd:" << m_fd 
			  << "\treadBuffFD:"<<readBuff_.getFd()
			  <<"\treadBuff_.m_buffRWPos:"<<readBuff_.m_buffRWPos
			  << "\treadBuff.m_buffPos:" << readBuff_.m_buffPos
			  << "\twriteBuff" << std::endl;
	//debug context end
};

HttpConn::HttpConn(const HttpConn& hc):m_fd(hc.m_fd),addr(hc.addr),isClose(hc.isClose),requestData(hc.requestData),responseData(hc.responseData),readBuff_(hc.readBuff_),writeBuff_(hc.writeBuff_){
	//debug context begin
	std::cout << "HttpConn::HttpConn(const HttpConn& hc) CP_Ctor" << std::endl;
	//debug context end

}

HttpConn::~HttpConn(){}

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
	if (readBuff() < 0) { 
		//debug context begin
		std::cout <<"HttpConn::onWork_request()->readBuff() < 0"<< std::endl;
		//debug context end
		return false; 
	}

	if (!requestData.parse(readBuff_)) { 
		//debug context begin
		std::cout << "HttpConn::onWork_request()-> ( !requestData.parse(readBuff_) )" << std::endl;
		//debug context end
		
		return false; 
	}
	return true;
};

//0
bool HttpConn::onWork_response() {
	if (!responseData.onWork(m_fd)) {
		//debug context begin
		std::cout << "HttpConn::onWork_request()->readBuff() < 0" << std::endl;
		//debug context end
		
		return false;
	}
	return true;
};

