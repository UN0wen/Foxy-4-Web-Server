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
  Reply process_request(bool status, char data[]);
  RequestParser::result_type http_format_precheck(char data[], size_t bytes_transferred);
  
  // Return the request generated from http_format_precheck
  Request get_request();

  // Return the content length of the request
  int get_content_length();

private:

  Request request_;
  RequestParser request_parser_;
};