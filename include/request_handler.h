#pragma once

#include "response.h"
#include "request.h"
#include "request_parser.h"
#include "config.h"

class RequestHandler
{
public:
  RequestHandler() {}
  static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
  virtual Response handle_request(const Request &request) = 0;
};
