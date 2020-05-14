#pragma once

#include "request_handler.h"
#include "data_collector.h"

class StatusRequestHandler : public RequestHandler
{
public:
    StatusRequestHandler() ;
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    Response handle_request(const Request &request);
};
