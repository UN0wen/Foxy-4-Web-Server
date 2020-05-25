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
	std::string root_;
	std::string path_;
	int port_;
    
    std::string convert_to_rawbody(Request request, bool isRedirect);
    bool parse_header(const std::string &header, Response &response);
    std::string clean_html(std::string &body);
    size_t hex_to_uint(const std::string &hex);
    std::string uint_to_hex(const size_t value);
    std::string redirect_request(std::string &new_root, const Request &client_request);
};
