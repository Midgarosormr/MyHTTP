#include "HttpConn.h"
#include <iostream>

HttpConn::HttpConn():m_fd(-1),responseData(&this->requestData){
	//debug context begin
	//std::cout << "addr[" << this << "]" << "HttpConn::HttpConn() Ctor" << std::endl;
	//debug context end
};

//1
HttpConn::HttpConn(int& fd,const sockaddr_in& clientaddr):m_fd(fd),addr(clientaddr),isClose(false), responseData(&requestData),readBuff_(fd),writeBuff_(fd){
	//debug context begin
	//std::cout <<"addr["<<this<<"]"<< "HttpConn::HttpConn(int fd,sockaddr_in clientaddr) Ctor" << std::endl;
	//debug context end
};

HttpConn::HttpConn(const HttpConn& hc):m_fd(hc.m_fd),addr(hc.addr),isClose(hc.isClose),requestData(hc.requestData),responseData(&requestData),readBuff_(hc.readBuff_),writeBuff_(hc.writeBuff_){
	//debug context begin
	//std::cout << "addr[" << this << "]" << "HttpConn::HttpConn(const HttpConn& hc) CP_Ctor" << std::endl;
	//debug context end

}

HttpConn & HttpConn::operator=(const HttpConn & hcCOPY){
	//debug context begin
	//std::cout << "addr[" << this << "]" << "\tHttpConn::operator=(HttpConn & hcCOPY)" << std::endl;
	//debug context end
	m_fd = hcCOPY.m_fd;
	addr = hcCOPY.addr;
	isClose = hcCOPY.isClose;
	requestData = hcCOPY.requestData;
	readBuff_ = hcCOPY.readBuff_;
	writeBuff_ = hcCOPY.writeBuff_;
	return *this;
}

HttpConn::~HttpConn(){}

//1
int HttpConn::readBuff(){
	return readBuff_.read2fd();
};

//0
Buffer* HttpConn::getwriteBuff() {
	return &writeBuff_;	
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
		//std::cout <<"HttpConn::onWork_request()->readBuff() < 0"<< std::endl;
		//debug context end
		return false; 
	}

	if (!requestData.parse(readBuff_)) { 
		//debug context begin
		//std::cout << "HttpConn::onWork_request()-> ( !requestData.parse(readBuff_) )" << std::endl;
		//debug context end
		
		return false; 
	}
	//debug context begin
	//std::cout << std::endl << requestData.method + requestData.URI + requestData.version<< "\n";
	for (auto x : requestData.header)//std::cout << x.first<<":"<<x.second<<std::endl;
	//debug context end


	return true;
};

//0
bool HttpConn::onWork_response() {
	if (!responseData.onWork(&writeBuff_)) {
		//debug context begin
		//std::cout << "HttpConn::onWork_request()->readBuff() < 0" << std::endl;
		//debug context end	
		return false;
	}
	return true;
};

