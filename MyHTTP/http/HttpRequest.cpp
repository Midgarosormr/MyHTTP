#include "HttpRequest.h"
#include <algorithm>
#include <iostream>

//RFC 2616�ж����ͷ��(Request Header Fields & Entity Header Fields)��42��
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
���� general-header | request-header | entity-header �� CRLF��* ; 
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
	//requestline_s ������CRLF

	std::string::size_type begin = 0, n = 0;
	n = requestline_s.find(" ");
	if (n == std::string::npos)	return false;
	//����ӵ�з�Χ [first, last) ���ݵ� string 
	method = string(&requestline_s[begin], &requestline_s[n]);
	if (method.empty()) return false;
	begin = n + 1;
	n = requestline_s.find(" ", begin);
	if (n == std::string::npos)	return false;
	URI = string(&requestline_s[begin], &requestline_s[n]);
	if (URI.empty()) return false;
	version = string(&requestline_s[n + 1], &requestline_s.back());
	if (version.empty()) return false;
	//ȫ������,����״̬
	state_main = HEADERS;
	return true;
};

//1
bool HttpRequest::parseHeader(string & header_s)
{
	//ͷ��ȫ���������,����״̬
	if (header_s.empty()) {
		if (method == "GET") 
			state_main = FINISH;
		else {
			state_main = BODY;
		}
		return true;
	}
	
	//����ͷ��
	std::string::size_type begin = 0, n = 0;
	string key_head, value;
	n=header_s.find(":");
	if (n == std::string::npos)	return false;
	//����ӵ�з�Χ [first, last) ���ݵ� string 
	key_head = string(&header_s[begin], &header_s[n]);
	begin = n + 1;
	value = string(&header_s[begin],&*header_s.end());
	header[key_head] = value;	//���ͷ����ֵ

	return true;
};

//1
bool HttpRequest::parseBody(string & body_s)
{
	//��β������ '\0'��String�๹��ʱ��ȥ '\0'
	body = body_s;
	state_main = FINISH;
	return true;
};
