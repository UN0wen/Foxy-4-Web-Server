//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "status_request_handler.h"
#include <string>

namespace status_response
{
  const std::string html_header_begin = "<!DOCTYPE html><html><body>";
  const std::string title = "<head><title>Server Status</title></head>";
  const std::string header_count = "<h1>Count for server's request and response</h1>";
  const std::string header_map = "<h1>Path to RequestHandler</h1>";
  const std::string request_count = "<p>Number of request received from URI ";
  const std::string request_handler_header = "<p>URI prefix for ";

  //single string token
  const std::string for_token = "for";
  const std::string status_token = " with status code ";
  const std::string semicolon_token = " : ";
  const std::string paragraph_end = "</p>";
  const std::string html_header_end = "</body></html>";

  std::string create_body()
  {
    std::string body = "";
    body += html_header_begin;
    body += title;
    body += header_count;
    std::map<std::string, std::map<std::string, int>>::iterator itr;
    std::map<std::string, int>::iterator ptr;
    DataCollector* data_collector = DataCollector::get_instance();
    auto status_map = data_collector->status_map();
    auto uri_request_handler = data_collector->uri_request_handler();

    for (itr = status_map.begin(); itr != status_map.end(); itr++)
    {
      for (ptr = itr->second.begin(); ptr != itr->second.end(); ptr++)
      {
        body += request_count;
        body += itr->first;
        body += status_token;
        body += ptr->first;
        body += semicolon_token;
        body += std::to_string(ptr->second);
        body += paragraph_end;
      }
    }
    std::map<std::string, std::string>::iterator mtr;
    body += header_map;
    for (mtr = uri_request_handler.begin(); mtr != uri_request_handler.end(); mtr++)
    {
      body += request_handler_header;
      body += mtr->second;
      body += semicolon_token;
      body += mtr->first;
      body +=paragraph_end;
    }
    body += html_header_end;
    return body;
  }
} // namespace status_response

StatusRequestHandler::StatusRequestHandler()
{
}

RequestHandler *StatusRequestHandler::Init(const std::string &location_path, const NginxConfig &config)
{
  StatusRequestHandler *status_request_handler = new StatusRequestHandler();
  DataCollector::get_instance()->add_handler(location_path, "Status Request Handler");
  return status_request_handler;
}


Response StatusRequestHandler::handle_request(const Request &request)
{
  BOOST_LOG_TRIVIAL(info) << "[StatusRequestHandler] status request handler is handling request.";
  Response response = Response();
  std::string body = status_response::create_body();
  response.code_ = Response::ok;
  response.body_ = body;
  response.headers_["Content-Length"] = std::to_string(response.body_.size());
  response.headers_["Content-Type"] = "text/html";
  BOOST_LOG_TRIVIAL(info) << "[StatusRequestHandler] finish handling request.";
  BOOST_LOG_TRIVIAL(info) << "[ResponseLength] " << std::to_string(response.body_.size());
  
  return response;
}
