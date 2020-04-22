#pragma once

#include "reply.h"
#include "request.h"

class RequestHandler
{
public:
  RequestHandler() {}
  virtual void handle_request(
      Request &request,
      Reply &reply) = 0;
};