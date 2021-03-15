#pragma once
#include "Buffer.h"
#include "string"
#include <unordered_map>

using std::string;
using std::unordered_map;


class HttpRequest
{
public:
	HttpRequest();
	~HttpRequest();


	enum PARSE_STATE {
		REQUEST_LINE,
		HEADERS,
		BODY,
		FINISH,
	};

	enum HTTP_PARSE_CODE {
		NO_REQUEST = 0,
		GET_REQUEST,
		BAD_REQUEST,
		NO_RESOURSE,
		FORBIDDENT_REQUEST,
		FILE_REQUEST,
		INTERNAL_ERROR,
		CLOSED_CONNECTION,
	};

	bool parse(Buffer& buff);	//解析缓冲区内的请求
	bool parseRequestLine(string& requestline_s);
	bool parseHeader(string& header_s);
	bool parseBody(string& body_s);
public:
	PARSE_STATE state_main;
	HTTP_PARSE_CODE state_http;
	string method,URI,version;		//请求行第一行
	unordered_map<string, string> header;	//Request Header Fields	<request-header,value>
	string body;	   
};

