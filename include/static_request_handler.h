#pragma once

#include "request_handler.h"
#include <string>

class StaticRequestHandler : public RequestHandler
{
public:
    StaticRequestHandler(const std::string &root, const std::string &path);
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    StaticRequestHandler() {
        root_ = "";
        path_ = "";
    }
    Response handle_request(const Request &request);

private:
    bool url_decode(const std::string &in, std::string &out);
    bool check_request_path(const std::string &request_uri, std::string &request_path);
    std::string get_extension(const std::string &request_path);
    bool replace_path(std::string &request_path);

private:
    std::string root_;
    std::string path_;
};
