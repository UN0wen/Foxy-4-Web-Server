//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "request_handler.h"
#include "reply.h"

RequestHandler::RequestHandler()
{
}

int RequestHandler::get_content_length()
{
  for (int i = 0; i < request_.headers.size(); i++)
  {
    if (request_.headers[i].name == "Content-Length")
      return std::stoi(request_.headers[i].value);
  }

  return 0;
}

RequestParser::result_type RequestHandler::http_format_precheck(const char data[], size_t bytes_transferred)
{
  RequestParser::result_type result;
  std::tie(result, std::ignore) = request_parser_.parse(request_,
                                                        data,
                                                        data + bytes_transferred);

  int char_amount = request_parser_.get_char_amount();
  int content_length = get_content_length();

  if (content_length > 0 && char_amount == strlen(data))
  {
    return result = RequestParser::result_type::missing_data;
  }

  return result;
}

Request RequestHandler::get_request()
{
  return request_;
}

void RequestHandler::set_request(const Request& request) {
    request_ = request;
}

Reply RequestHandler::process_request(bool status, char data[])
{
  Reply rep;

  rep.status = status ? Reply::ok : Reply::bad_request;
  rep.content = data;
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "content-type";
  rep.headers[1].value = "text/plain";
  return rep;
}