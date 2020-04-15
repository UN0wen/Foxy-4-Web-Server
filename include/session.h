#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

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

  // Handles the additional read in case the initial message is missing data
  void handle_final_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  void handle_write(const boost::system::error_code& error);
  Reply process_request(bool status);
  
  RequestHandler request_handler_;
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  char buffer_[max_length];
  Request request_;
  RequestParser request_parser_;
};
