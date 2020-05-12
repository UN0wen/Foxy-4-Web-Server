#pragma once

#include "reply.h"
#include "request.h"
#include "request_parser.h"
#include "config.h"

class RequestHandler
{
public:
  RequestHandler() {}
  static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
  virtual void handle_request(
      Request &request,
      Reply &reply,
      RequestParser::result_type parse_result) = 0;
};
