//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "static_request_handler.h"
#include "response_generator.h"
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include "mime_types.h"
#include "data_collector.h"
#include "utility.h"
#include "cookie.h"

StaticRequestHandler::StaticRequestHandler(const std::string &root, const std::string &auth, const std::string &path)
    : root_(root), path_(path), auth_path_(auth)
{
}

RequestHandler *StaticRequestHandler::Init(const std::string &location_path, const NginxConfig &config)
{
    std::string root = "";
    std::string auth = "";
    DataCollector::get_instance()->add_handler(location_path, "Static Request Handler");
    for (const auto &s : config.statements_)
    {
        std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "root");
        if (find_root != s->tokens_.end())
        {
            root = *(find_root + 1);
        }

        std::vector<std::string>::iterator find_auth = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "auth");
        if (find_auth != s->tokens_.end())
        {
            auth = *(find_auth + 1);
        }
    }
    if (!utility::is_quoted(&root))
        return nullptr;

    if (!utility::is_quoted(&auth))
        auth = "";

    StaticRequestHandler *static_request_handler = new StaticRequestHandler(root, auth, location_path);
    return static_request_handler;
}

bool StaticRequestHandler::check_cookie(std::string cookie_string)
{
    std::string cookie_value;
    std::vector<std::string> cookies;
    boost::split(cookies, cookie_string, boost::is_any_of("; "));

    for (auto &cookie : cookies)
    {
        // String starts with 
        if (cookie.rfind("jwt=", 0) == 0)
        {
            cookie_value = cookie.substr(4);
            break;
        }
    }

    return Cookie::verify_cookie(cookie_value);
}

Response StaticRequestHandler::prepare_redirect()
{
    Response response;
    response.code_ = Response::moved_temporarily;
    response.headers_["Content-Length"] = std::to_string(response.body_.size());
    response.headers_["Content-Type"] = "text/plain";
    response.headers_["Connection"] = "close";
    response.headers_["Location"] = auth_path_;

    return response;
}

Response StaticRequestHandler::handle_request(const Request &request)
{
    // Decode url to path.
    BOOST_LOG_TRIVIAL(error) << "[StaticRequestHandler] Static request handler is handling request.";
    Response response = Response();
    std::string request_path;

    std::string cookie_string = "";

    // Find cookie
    auto cookie_it = request.headers_.find("Cookie");

    if (cookie_it != request.headers_.end())
    {
        cookie_string = cookie_it->second;
    }

    // Only check for cookie if there is an auth path
    if (auth_path_.length() && !check_cookie(cookie_string))
    {
        response = prepare_redirect();
        BOOST_LOG_TRIVIAL(info) << "[StaticRequestHandler] Cookie check failed, returning redirect.";
        return response;
    }

    if (!check_request_path(request.uri_, request_path))
    {
        BOOST_LOG_TRIVIAL(error) << "[StaticRequestHandler] Error: Request URI contains errors.";
        response = ResponseGenerator::stock_response(Response::bad_request);
        return response;
    }

    if (request_path == "")
    {
        request_path = "/";
    }

    if (!replace_path(request_path))
    {
        BOOST_LOG_TRIVIAL(error) << "[StaticRequestHandler] Error: path not found in URI.";

        response = ResponseGenerator::stock_response(Response::internal_server_error);
        return response;
    }

    std::string extension = get_extension(request_path);

    // Open the file to send back.
    std::string full_path = request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is)
    {
        response = ResponseGenerator::stock_response(Response::not_found);
        return response;
    }

    // Fill out the response to be sent to the client.
    response.code_ = Response::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        response.body_.append(buf, is.gcount());

    //response.headers.resize(2);
    std::string content_length = std::to_string(response.body_.size());
    response.headers_["Content-Length"] = content_length;
    response.headers_["Content-Type"] = mime_types::extension_to_type(extension);
    BOOST_LOG_TRIVIAL(info) << "[StaticRequestHandler] finish handling request";
    BOOST_LOG_TRIVIAL(info) << "[ResponseLength] " << content_length;
    return response;
}

std::string StaticRequestHandler::get_extension(const std::string &request_path)
{
    // Determine the file extension.
    std::string extension = "";
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");

    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }

    return extension;
}

bool StaticRequestHandler::replace_path(std::string &request_path)
{
    size_t start_pos = request_path.find(path_);
    if (start_pos == std::string::npos)
        return false;
    request_path.replace(start_pos, path_.length(), root_);
    return true;
}

bool StaticRequestHandler::check_request_path(const std::string &request_uri, std::string &request_path)
{
    // Decode URI to path
    if (!url_decode(request_uri, request_path))
    {
        return false;
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos)
    {
        return false;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/')
    {
        request_path += "index.html";
    }

    return true;
}

bool StaticRequestHandler::url_decode(const std::string &in, std::string &out)
{
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value)
                {
                    out += static_cast<char>(value);
                    i += 2;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (in[i] == '+')
        {
            out += ' ';
        }
        else
        {
            out += in[i];
        }
    }
    return true;
}
