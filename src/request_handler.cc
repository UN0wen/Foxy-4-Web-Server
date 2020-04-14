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

http::server::RequestParser::result_type  RequestHandler::http_format_precheck(char data[], size_t bytes_transferred)
{
    http::server::RequestParser::result_type result;
    std::tie(result, std::ignore) = request_parser_.parse(request_,
                                                          data,
                                                          data + bytes_transferred);
    int char_amount = request_parser_.get_char_amount();                                                          
    for (int i = 0; i < request_.headers.size(); i++)
      {
        if (request_.headers[i].name == "Content-Length" && request_.headers[i].value != "0" && char_amount == strlen(data))
        {
          result = http::server::RequestParser::result_type::missing_data;
        }
      }
    return result;                                                          
}

http::server::Request RequestHandler::get_request()
{
  return request_;
}

http::server::Reply RequestHandler::process_request(bool status, char data[]){
  http::server::Reply rep;
  std::printf("msg recieved: %s \n", data);
  rep.status = status ? http::server::Reply::ok : http::server::Reply::bad_request;
  rep.content = data;
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "content-type";
  rep.headers[1].value = "text/plain";
  return rep;

}