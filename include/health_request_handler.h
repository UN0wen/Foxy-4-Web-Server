#pragma once

#include "request_handler.h"
#include "response.h"

class HealthRequestHandler : public RequestHandler
{
public:
    HealthRequestHandler() ;
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    Response handle_request(const Request &request);
};

