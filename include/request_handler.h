#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request.h"
#include "request_parser.h"

using boost::asio::ip::tcp;
#include "reply.h"

class request_handler
{
public:
  request_handler();
  http::server::reply process_request(bool status, char data[]);
  http::server::request_parser::result_type http_format_precheck(char data[], size_t bytes_transferred);

private:

  http::server::request request_;
  http::server::request_parser request_parser_;
};