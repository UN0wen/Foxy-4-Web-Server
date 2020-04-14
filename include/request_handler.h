#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request.h"
#include "request_parser.h"

using boost::asio::ip::tcp;
#include "reply.h"

class RequestHandler
{
public:
  RequestHandler();
  http::server::Reply process_request(bool status, char data[]);
  http::server::RequestParser::result_type http_format_precheck(char data[], size_t bytes_transferred);
  http::server::Request get_request();

private:

  http::server::Request request_;
  http::server::RequestParser request_parser_;
};