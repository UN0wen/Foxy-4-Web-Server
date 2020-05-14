//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "data_collector.h"
#include "echo_request_handler.h"

EchoRequestHandler::EchoRequestHandler()
{
}

RequestHandler* EchoRequestHandler::Init(const std::string& location_path, const NginxConfig& config)
{
	EchoRequestHandler* echo_request_handler = new EchoRequestHandler();
  std::string temp = "Echo Request Handler";
	DataCollector::uri_request_handler[location_path] = temp;
	return echo_request_handler;
}

std::string EchoRequestHandler::convert_to_rawbody(Request request){
  std::string req = "";
  if(request.method_ == Request::MethodEnum::GET){
    req += "GET";
  }
  else if(request.method_ == Request::MethodEnum::POST){
    req += "POST";
  }
  else if(request.method_ == Request::MethodEnum::PUT){
    req += "PUT";
  }
  else{
    req += "UNKNOWN";
  }
  req += " ";
  req += request.uri_;
  req += " HTTP";
  req += "/";
  req += request.http_version_;
  req += "\r\n";
  std::map<std::string, std::string>::iterator it;
  for(it = request.headers_.begin(); it != request.headers_.end(); it ++ ){
        if(it->second!=""){
          req += it->first;
          req += ": ";
          req += it->second;
          req += "\r\n";
        }   
  }
  req += "\r\n";
  req += request.body_;
  return req;
}

Response EchoRequestHandler::handle_request(const Request &request)
{
  Response response = Response();
  std::string body = convert_to_rawbody(request);
  response.body_ = body;
  response.code_ = Response::ok;
  response.headers_["Content-Length"] = std::to_string(response.body_.size());
  response.headers_["Content-Type"] = "text/plain";  
  return response;
}

