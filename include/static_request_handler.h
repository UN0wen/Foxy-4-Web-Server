#include "request_handler.h"
#pragma once

class StaticRequestHandler : public RequestHandler
{
public:
    StaticRequestHandler();
    void handle_request(
        Request &request,
        Reply &reply);

};