#pragma once

#include "request_handler.h"
#include "response.h"


class NotFoundRequestHandler : public RequestHandler
{
public:
    NotFoundRequestHandler() ;
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    Response handle_request(const Request &request);
};

