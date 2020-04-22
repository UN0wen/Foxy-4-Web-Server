#include "request_handler.h"
#pragma once

class EchoRequestHandler : public RequestHandler
{
public:
    EchoRequestHandler() ;

    void handle_request(
        Request &request,
        Reply &reply);
};