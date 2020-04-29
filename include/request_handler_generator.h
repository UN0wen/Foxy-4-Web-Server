#pragma once

#include "request_handler.h"

class RequestHandlerGenerator {
    public:
    RequestHandlerGenerator(std::map<std::string, std::shared_ptr<RequestHandler>> map);
 
    std::shared_ptr<RequestHandler> dispatch_handler(std::string uri);
    private:
    int common_prefix_length(std::string a, std::string b);
    std::map<std::string, std::shared_ptr<RequestHandler>> map_;
};