//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "echo_request_handler.h"

EchoRequestHandler::EchoRequestHandler()
{
}

RequestHandler* EchoRequestHandler::Init(const std::string& location_path, const NginxConfig& config)
{
	EchoRequestHandler* echo_request_handler = new EchoRequestHandler();
	return echo_request_handler;
}

void EchoRequestHandler::handle_request(Request &request, Response &response, RequestParser::result_type parse_result)
{
  response.code_ = parse_result == RequestParser::result_type::good ? Response::ok : Response::bad_request;
  response.body_ = std::string(request.raw_request);
  //response.headers.resize(2);
  response.headers_["Content-Length"] = std::to_string(response.body_.size());
  response.headers_["content-type"] = "text/plain";  
  return;
}

