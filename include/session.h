#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <map>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;
#include "response.h"
#include "request_parser.h"
#include "request_handler_generator.h"
#include "response_generator.h"
#include "data_collector.h"


class Session
{
public:

  Session(boost::asio::io_service& io_service, boost::asio::ssl::context& context, RequestHandlerGenerator generator);
  boost::asio::ssl::stream<tcp::socket>::lowest_layer_type& socket();


  void start();

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  // Handles the additional read in case the initial message is missing data
  void handle_final_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_handshake(const boost::system::error_code &error);

  void handle_write(const boost::system::error_code& error);
  Response process_request(bool status);
  void deep_copy_response(Response response);
  void handle_bad_request();

  void close_socket();
  
  boost::asio::ssl::stream<tcp::socket> socket_;
  
  RequestParser request_parser_;
  ResponseGenerator response_generator_;
  Request request_;
  
  enum { max_length = 1024 };
  char data_[max_length];
  char buffer_[max_length];
  RequestHandlerGenerator generator_;
  DataCollector* data_collector_ = nullptr;
};
