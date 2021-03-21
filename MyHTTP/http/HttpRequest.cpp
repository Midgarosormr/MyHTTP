#include "HttpRequest.h"
#include <algorithm>
#include <iostream>

//RFC 2616中定义的头部(Request Header Fields & Entity Header Fields)有42个
HttpRequest::HttpRequest():header(42){
	state_main = REQUEST_LINE;
}
HttpRequest::HttpRequest(const HttpRequest & cp):method(cp.method)
{

}
;



HttpRequest::~HttpRequest() {};

/*

Request = Request-Line CRLF
（（ general-header | request-header | entity-header ） CRLF）* ; 
CRLF
[ message-body ] ;

*/

//1
bool HttpRequest::parse(Buffer& buff) {
	const char CRLF[] = "\r\n";
	while ( (buff.readable()>0) && (state_main!=FINISH)) {
		//debug context begin
		std::cout << "HttpRequest::parse(Buffer& buff) -> ( while ( (buff.readable()>0) && (state_main!=FINISH)) )   begin" << std::endl;
		//debug context end
		char* lineEndPos = std::search(buff.getBeginPos(), buff.getEndPOS(), CRLF, CRLF + 2);
		string line(buff.getBeginPos(), lineEndPos);	//
		switch (state_main)
		{
		case HttpRequest::REQUEST_LINE:
			if (!parseRequestLine(line)) {
				//debug context begin
				std::cout << std::endl<<"HttpRequest::parse(Buffer& buff) -> ( (!parseRequestLine(line)) )   FALSE" << std::endl;
				//debug context end
				return false;
			}
			break;
		case HttpRequest::HEADERS:
			if (!parseHeader(line)) { 
				//debug context begin
				std::cout << std::endl << "HttpRequest::parse(Buffer& buff) -> ( (!parseHeader(line)) )   FALSE" << std::endl;
				//debug context end
				return false; 
			}
			break;
		case HttpRequest::BODY:
			if (!parseBody(line)) {
				//debug context begin
				std::cout << std::endl << "HttpRequest::parse(Buffer& buff) -> ( (!parseBody(line)) )   FALSE" << std::endl;
				//debug context end
				return false;
			}
			return true;
			break;
		case HttpRequest::FINISH:
			return true;
			break;
		default:

			break;
		}
		if (lineEndPos == buff.getEndPOS()) {
			//debug context begin
			std::cout << std::endl << "HttpRequest::parse(Buffer& buff) -> ( (lineEndPos == buff.getEndPOS()) )   FALSE" << std::endl;
			//debug context end
			return false;
		}
		if (!buff.adjustReadByte(line.size() + 2)) {
			//debug context begin
			std::cout << std::endl << "HttpRequest::parse(Buffer& buff) -> ( !buff.adjustReadByte(line.size() + 2) )   FALSE" << std::endl;
			//debug context end
			return false;
		}
	}
	if(state_main==FINISH) return true;
	//debug context begin
	std::cout << std::endl << "HttpRequest::parse(Buffer& buff) -> NOT KNOW ERRPR   FALSE" << std::endl;
	//debug context end
	return false;
};

//1
bool HttpRequest::parseRequestLine(string & requestline_s)
{
	//requestline_s 不包含CRLF

	std::string::size_type begin = 0, n = 0;
	n = requestline_s.find(" ");
	if (n == std::string::npos)	return false;
	//构造拥有范围 [first, last) 内容的 string 
	method = string(&requestline_s[begin], &requestline_s[n]);
	if (method.empty()) return false;
	begin = n + 1;
	n = requestline_s.find(" ", begin);
	if (n == std::string::npos)	return false;
	URI = string(&requestline_s[begin], &requestline_s[n]);
	if (URI.empty()) return false;
	version = string(&requestline_s[n + 1], &requestline_s.back());
	if (version.empty()) return false;
	//全部正常,更新状态
	state_main = HEADERS;
	return true;
};

//1
bool HttpRequest::parseHeader(string & header_s)
{
	//头部全部解析完成,更新状态
	if (header_s.empty()) {
		if (method == "GET") 
			state_main = FINISH;
		else {
			state_main = BODY;
		}
		return true;
	}
	
	//解析头部
	std::string::size_type begin = 0, n = 0;
	string key_head, value;
	n=header_s.find(":");
	if (n == std::string::npos)	return false;
	//构造拥有范围 [first, last) 内容的 string 
	key_head = string(&header_s[begin], &header_s[n]);
	begin = n + 1;
	value = string(&header_s[begin],&*header_s.end());
	header[key_head] = value;	//添加头部和值

	return true;
};

//1
bool HttpRequest::parseBody(string & body_s)
{
	//结尾不包含 '\0'，String类构造时舍去 '\0'
	body = body_s;
	state_main = FINISH;
	return true;
};
