#include "HttpResponse.h"
#include <sys/sendfile.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <error.h>

HttpResponse::HttpResponse(const HttpRequest* hr):requestPTR_(hr){

}


HttpResponse::~HttpResponse() {};

bool HttpResponse::onWork(Buffer* writeBuffPtr_) {
	//std::cout << "HttpResponse addr:[" << this << "]=HttpRequest\t" <<"requestPTR_"<<requestPTR_ <<std::endl;

	
	if (requestPTR_->URI.empty()) { 
		//debug context begin
		//std::cout << "HttpResponse::onWork(int cfd) -> (requestPTR_->URI.empty()) is false" << std::endl;
		//debug context end
		return false;
	}
	if (!fileAuthority()) {
		//debug context begin
		//std::cout << "HttpResponse::onWork(int cfd) -> (!fileAuthority(cfd)) is false" << std::endl;
		//debug context end
		code_ = 404;
	}
	else {
		code_ = 200;
	}
	if (!onSendBuff(writeBuffPtr_)) {
		//debug context begin
		//std::cout << "HttpResponse::onWork(int cfd) -> (!onSend()) is false" << std::endl;
		//debug context end
		return false;
	}
	return true;
};

//1-0    POST方法未定义
bool HttpResponse::fileAuthority() {
	if (requestPTR_->method == "GET") {

		//debug context begin
		//std::cout << "HttpResponse::fileAuthority(int clientFd) -> (requestPTR_->method == GET) begin " << std::endl;
		//debug context end

		if(stat(requestPTR_->URI.c_str(), &fileStat)<0) return false;
		if ((!(fileStat.st_mode & S_IRUSR)) && (!(fileStat.st_mode & S_IRGRP))) return false;
		//std::cout << "file pass authority" << std::endl;
		return true;
	}
	if (requestPTR_->method == "POST") {
		if ((!(fileStat.st_mode & S_IRUSR)) && (!(fileStat.st_mode & S_IRGRP))	//若前面的判断为1，则代表不具有读权限
			|| (!(fileStat.st_mode & S_IWUSR)) && (!(fileStat.st_mode & S_IWGRP))) return false;

		return true;
	}

	return true;
};

bool HttpResponse::onSendBuff(Buffer* writeBuff){
	string CRLF("\r\n");
	string statline = string("HTTP/1.1 ") + std::to_string(code_) + string(" ") + CODE_RESPONSE_STATUS[code_]+CRLF;
	//write stat line
	if (writeBuff->writeBuff(&*statline.begin(), statline.size())) { // 0 is true 
		//debug context begin
		//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) -> line 71 is false" << std::endl;
		//debug context end
		return false;
	}
	//write head line

	//write SP line
	if ( writeBuff->writeBuff(&CRLF.front(), CRLF.size()) ) {
		//debug context begin
		//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) -> line 80 is false" << std::endl;
		//debug context end
		return false;
	}
	//write body
	if (code_ == 200) {
		int filefd = open(requestPTR_->URI.c_str(), O_RDONLY);
		char filebuff[fileStat.st_size];
		int ret = read(filefd, filebuff, fileStat.st_size);
		if (ret < 0) {
			//debug context begin
			//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) -> code:[200] line 91 is false" << std::endl;
			//debug context end
			return false;
		}
		if (writeBuff->writeBuff(filebuff, fileStat.st_size)) {
			//debug context begin
			//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) ->code:[200]  line 97 is false" << std::endl;
			//debug context end
			return false;
		}
		close(filefd);
	}
	else if (code_ == 404) {
		int filefd = open("./404.html", O_RDONLY);
		if (stat("./404.html", &fileStat) < 0) {
			//debug context begin
			//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) -> code:[404]line 106 is false" << std::endl;
			//debug context end
			return false;
		}
		char filebuff[fileStat.st_size];
		int ret = read(filefd, filebuff, fileStat.st_size);
		if (ret < 0) {
			//debug context begin
			//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) -> code:[404]line 114 is false" << std::endl;
			//debug context end
			return false;
		}
		if (writeBuff->writeBuff(filebuff, fileStat.st_size)) {
			//debug context begin
			//std::cout << "HttpResponse::onSend(iovec* iov,Buffer* writeBuff) ->code:[404] line 120 is false" << std::endl;
			//debug context end
			return false;
		}
		close(filefd);
	}

	return true;
}

int HttpResponse::onSendfd(Buffer* writeBuff,int* err) {
	
	return writeBuff->write2fd(err); 

}