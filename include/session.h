#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request.h"
#include "request_parser.h"

using boost::asio::ip::tcp;
#include "reply.h"

class session
{
public:
  session(boost::asio::io_service& io_service);

  tcp::socket& socket();

  void start();

private:
  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);
  http::server::reply process_request();

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  http::server::request request_;
  http::server::request_parser request_parser_;
};
