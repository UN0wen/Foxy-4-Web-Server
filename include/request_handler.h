#pragma once

#include "reply.h"
#include "request.h"
#include "request_parser.h"

class RequestHandler
{
public:
  RequestHandler() {}
  virtual void handle_request(
      Request &request,
      Reply &reply,
      RequestParser::result_type parse_result) = 0;
};