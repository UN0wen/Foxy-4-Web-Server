#include <boost/log/trivial.hpp>
#include "response_generator.h"
#include <fstream>
#include <sstream>
#include <string>
#include "data_collector.h"
#include "mime_types.h"
#include "proxy_request_handler.h"
#include "utility.h"

ProxyRequestHandler::ProxyRequestHandler(const std::string &host, const int &port)
	: host_(host), port_(port)
{
}

RequestHandler* ProxyRequestHandler::Init(const std::string& location_path, const NginxConfig& config)
{
	std::string host = "";
	int port = 0;
	DataCollector::get_instance()->add_handler(location_path, "Proxy Request Handler");
	for (const auto &s : config.statements_)
	{
		std::vector<std::string>::iterator find_host = std::find(s->tokens_.begin(), s->tokens_.end(), "host");
		std::vector<std::string>::iterator find_port = std::find(s->tokens_.begin(), s->tokens_.end(), "port");

		if (find_host != s->tokens_.end())
		{
			host = *(find_host + 1);
		}

		if (find_port != s->tokens_.end())
		{
			port = std::stoi(*(find_port + 1));
		}
	}
	if (!utility::is_quoted(&host))
		return nullptr;

	BOOST_LOG_TRIVIAL(trace) << "Host: " + host + " Port: " + std::to_string(port);

	ProxyRequestHandler* proxy_request_handler = new ProxyRequestHandler(host, port);
	return proxy_request_handler;
}

Response ProxyRequestHandler::handle_request(const Request &request)
{
	Response response = Response();
	BOOST_LOG_TRIVIAL(trace) << "Proxy Reqeust: " << request.uri_;
	response.code_ = Response::moved_temporarily;
	response.body_ = "HTTP/" 
					+ request.http_version_ 
					+ " 302 Found\r\n";
					+ "Location www.ucla.edu\r\n";
	
	return response;

}