#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request.h"
#include "request_parser.h"

using boost::asio::ip::tcp;
#include "reply.h"
#include "request_handler.h"

class Session
{
public:
  Session(boost::asio::io_service& io_service);

  tcp::socket& socket();

  void start();

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
  void handle_final_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);
  http::server::Reply process_request(bool status);
  
  RequestHandler rh;
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char buffer[max_length];
  http::server::Request request_;
  http::server::RequestParser request_parser_;
};
