#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;
#include "reply.h"
#include "echo_request_handler.h"
#include "request_parser.h"
#include <boost/thread/thread.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include "request_handler.h"
#include "static_request_handler.h"
#include <map>


class Session
{
public:

  struct MappingType {
    EchoRequestHandler rh;
    StaticRequestHandler sh;
    std::string method;
    std::string root;
    MappingType(){}
  };

  Session(boost::asio::io_service& io_service, std::map<std::string, std::string> path_to_root, std::map<std::string, std::string> path_to_root_echo);
  tcp::socket& socket();

  void start();

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  // Handles the additional read in case the initial message is missing data
  void handle_final_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);
  Reply process_request(bool status);

  int common_prefix_length(std::string a, std::string b);
  MappingType map_uri_to_request_handler(std::string uri);
  RequestParser request_parser_;
  EchoRequestHandler request_handler_;
  Request request_;
  Reply reply_;
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char buffer_[max_length];
  std::map<std::string, std::string> path_to_root;
  std::map<std::string, std::string> path_to_root_echo;

};
