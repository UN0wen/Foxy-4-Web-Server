#pragma once

#include "request_handler.h"
#include "response.h"
#include <set>

class LoginRequestHandler : public RequestHandler
{
public:
    LoginRequestHandler(const std::string &root, const std::string &path);
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    Response handle_request(const Request &request);
private:
    std::set<std::string> extract_username(const std::string text);
    std::string extract_password(const std::string text, int username_length);
    Response perpare_html_response(const std::string file);
    std::string root_;
    std::string path_;
};