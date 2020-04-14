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
  Request get_request();

private:

  Request request_;
  RequestParser request_parser_;
};