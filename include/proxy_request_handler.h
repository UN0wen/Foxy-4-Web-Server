#pragma once 

#include "request_handler.h"
#include <string>

class ProxyRequestHandler : public RequestHandler
{
public:
	ProxyRequestHandler(const std::string &root, const std::string &path, const int &port);
	ProxyRequestHandler()
	{
		root_ = "";
		path_ = "";
		port_ = 0;
	}
	static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
	Response handle_request(const Request &request);
private:
	std::string convert_to_rawbody(Request request);
	std::string root_;
	std::string path_;
	int port_;
};
