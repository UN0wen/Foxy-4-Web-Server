#pragma once

#include "request_handler.h"

class EchoRequestHandler : public RequestHandler
{
public:
    EchoRequestHandler() ;

    void handle_request(
        Request &request,
        Reply &reply,
        RequestParser::result_type parse_result);
};