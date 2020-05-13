#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;
#include "response.h"
#include "echo_request_handler.h"
#include "request_parser.h"
#include "static_request_handler.h"
#include <boost/thread/thread.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include "request_handler.h"
#include "request_handler_generator.h"
#include <map>
#include "response_generator.h"


class Session
{
public:

  Session(boost::asio::io_service& io_service, RequestHandlerGenerator generator);
  tcp::socket& socket();

  void start();

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  // Handles the additional read in case the initial message is missing data
  void handle_final_read(const boost::system::error_code& error,
      size_t bytes_transferred, RequestHandler* request_handler);

  void handle_write(const boost::system::error_code& error);
  Response process_request(bool status);
  void deep_copy_response(Response response);
  void handle_bad_request();

  RequestParser request_parser_;
  ResponseGenerator response_generator_;
  Request request_;
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char buffer_[max_length];
  RequestHandlerGenerator generator_;
};
