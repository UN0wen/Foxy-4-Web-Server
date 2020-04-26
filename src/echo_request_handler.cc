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

void EchoRequestHandler::handle_request(Request &request, Reply &reply, RequestParser::result_type parse_result)
{
  reply.status = parse_result == RequestParser::result_type::good ? Reply::ok : Reply::bad_request;
  reply.content = std::string(request.raw_request);
  reply.headers.resize(2);
  reply.headers[0].name = "Content-Length";
  reply.headers[0].value = std::to_string(reply.content.size());
  reply.headers[1].name = "content-type";
  reply.headers[1].value = "text/plain";
  
  return;
}

