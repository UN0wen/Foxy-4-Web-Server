#pragma once 

#include "request_handler.h"
#include <string>

class ProxyRequestHandler : public RequestHandler
{
public:
	ProxyRequestHandler(const std::string &host, const int &port);
	ProxyRequestHandler()
	{
		host_ = "";
		port_ = 0;
	}
	static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
	Response handle_request(const Request &request);
private:
	std::string host_;
	int port_;
};
