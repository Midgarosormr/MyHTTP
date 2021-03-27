#pragma once
#define HTTP_CONTINUE                  100
#define HTTP_SWITCHING_PROTOCOLS       101
#define HTTP_PROCESSING                102

#define HTTP_OK                        200
#define HTTP_CREATED                   201
#define HTTP_ACCEPTED                  202
#define HTTP_NO_CONTENT                204
#define HTTP_PARTIAL_CONTENT           206

#define HTTP_SPECIAL_RESPONSE          300
#define HTTP_MOVED_PERMANENTLY         301
#define HTTP_MOVED_TEMPORARILY         302
#define HTTP_SEE_OTHER                 303
#define HTTP_NOT_MODIFIED              304
#define HTTP_TEMPORARY_REDIRECT        307
#define HTTP_PERMANENT_REDIRECT        308

#define HTTP_BAD_REQUEST               400
#define HTTP_UNAUTHORIZED              401
#define HTTP_FORBIDDEN                 403
#define HTTP_NOT_FOUND                 404
#define HTTP_NOT_ALLOWED               405
#define HTTP_REQUEST_TIME_OUT          408
#define HTTP_CONFLICT                  409
#define HTTP_LENGTH_REQUIRED           411
#define HTTP_PRECONDITION_FAILED       412
#define HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define HTTP_REQUEST_URI_TOO_LARGE     414
#define HTTP_UNSUPPORTED_MEDIA_TYPE    415
#define HTTP_RANGE_NOT_SATISFIABLE     416
#define HTTP_MISDIRECTED_REQUEST       421
#define HTTP_TOO_MANY_REQUESTS         429

#include <unordered_map>
#include <string>
using std::string;

const std::unordered_map<string, string> FILE_TYPE = {
	{ ".html",  "text/html" },
	{ ".xml",   "text/xml" },
	{ ".xhtml", "application/xhtml+xml" },
	{ ".txt",   "text/plain" },
	{ ".rtf",   "application/rtf" },
	{ ".pdf",   "application/pdf" },
	{ ".word",  "application/nsword" },
	{ ".png",   "image/png" },
	{ ".gif",   "image/gif" },
	{ ".jpg",   "image/jpeg" },
	{ ".jpeg",  "image/jpeg" },
	{ ".au",    "audio/basic" },
	{ ".mpeg",  "video/mpeg" },
	{ ".mpg",   "video/mpeg" },
	{ ".avi",   "video/x-msvideo" },
	{ ".gz",    "application/x-gzip" },
	{ ".tar",   "application/x-tar" },
	{ ".css",   "text/css "},
	{ ".js",    "text/javascript "},
};

static std::unordered_map< int , string>CODE_RESPONSE_STATUS = {
	{ 200, "OK" },
	{ 400, "Bad Request" },
	{ 403, "Forbidden" },
	{ 404, "Not Found" },
};

const std::unordered_map<int, string>CODE_RESPONSE_PATH = {
	{ 400, "/400.html" },
	{ 403, "/403.html" },
	{ 404, "/404.html" },
};