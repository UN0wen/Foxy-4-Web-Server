#pragma once

#include "request_handler.h"
#include "config.h"

class RequestHandlerGenerator {
    public:
    RequestHandlerGenerator();
    std::shared_ptr<RequestHandler> dispatch_handler(std::string uri);
    RequestHandler* createHandler(std::string path, std::string method, NginxConfig& config);
    bool get_map(NginxConfig* config);
    std::map<std::string, std::shared_ptr<RequestHandler>> mapping();
    
    private:
    int common_prefix_length(std::string a, std::string b);
    std::map<std::string, std::shared_ptr<RequestHandler>> map_;
    bool invalid_config(const auto &statement);
};
