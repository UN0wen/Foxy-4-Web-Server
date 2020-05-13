#pragma once

#include "request_handler.h"

class EchoRequestHandler : public RequestHandler
{
public:
    EchoRequestHandler() ;
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    void handle_request(
        Request &request,
        Response &response,
        RequestParser::result_type parse_result);
};
