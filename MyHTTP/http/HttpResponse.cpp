#include "HttpResponse.h"
#include <sys/sendfile.h>
#include <fcntl.h>
#include <iostream>

HttpResponse::HttpResponse(const HttpRequest* hr):requestPTR_(hr){

}


HttpResponse::~HttpResponse() {};

bool HttpResponse::onWork(int cfd) {
	if (requestPTR_->URI.empty()) { 
		//debug context begin
		std::cout << "HttpResponse::onWork(int cfd) -> (requestPTR_->URI.empty()) is false" << std::endl;
		//debug context end
		return false;
	}
	if (!fileAuthority(cfd)) {
		//debug context begin
		std::cout << "HttpResponse::onWork(int cfd) -> (!fileAuthority(cfd)) is false" << std::endl;
		//debug context end
		return false;
	}
	return true;
};

//1-0    POST方法未定义
bool HttpResponse::fileAuthority(int clientFd) {
	if (requestPTR_->method == "GET") {

		//debug context begin
		std::cout << "HttpResponse::fileAuthority(int clientFd) -> (requestPTR_->method == GET) begin " << std::endl;
		//debug context end

		if(stat(requestPTR_->URI.c_str(), &fileStat)<0) return false;
		if ((!(fileStat.st_mode & S_IRUSR)) && (!(fileStat.st_mode & S_IRGRP))) return false;
		std::cout << "file pass authority" << std::endl;

		if (sendfile(clientFd, open(requestPTR_->URI.c_str(), O_RDONLY), 0, fileStat.st_size) < 0) return false;
		return true;
	}
	if (requestPTR_->method == "POST") {
		if ((!(fileStat.st_mode & S_IRUSR)) && (!(fileStat.st_mode & S_IRGRP))	//若前面的判断为1，则代表不具有读权限
			|| (!(fileStat.st_mode & S_IWUSR)) && (!(fileStat.st_mode & S_IWGRP))) return false;

		return true;
	}

	return true;
};

void HttpResponse::onError(int cfd) {
	
};