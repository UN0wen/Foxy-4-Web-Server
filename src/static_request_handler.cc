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
#include <fstream>
#include <sstream>
#include <string>
#include <boost/log/trivial.hpp>
#include "mime_types.h"

StaticRequestHandler::StaticRequestHandler(const std::string &root, const std::string &path)
    : root_(root), path_(path)
{
}

RequestHandler* StaticRequestHandler::Init(const std::string& location_path, const NginxConfig& config)
{
    std::string root = "";
    for (const auto &s : config.statements_)
    {
        std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "root");
        if(find_root != s->tokens_.end())
        {
            root = *(find_root + 1);
        }
    }
    if(!is_quoted(&root))
        return nullptr;

    StaticRequestHandler* static_request_handler = new StaticRequestHandler(root, location_path);
    return static_request_handler;
}

void StaticRequestHandler::handle_request(Request &request, Reply &reply, RequestParser::result_type parse_result)
{
    if (parse_result != RequestParser::result_type::good)
    {
        reply = Reply::stock_reply(Reply::bad_request);
        return;
    }

    // Decode url to path.
    std::string request_path;

    BOOST_LOG_TRIVIAL(info) << "Request URI " << request.uri;
    if (!check_request_path(request.uri, request_path))
    {
        BOOST_LOG_TRIVIAL(error) << "Request URI contains error(s)... returning 400 response";
        reply = Reply::stock_reply(Reply::bad_request);
        return;
    }

    if (!replace_path(request_path))
    {
        BOOST_LOG_TRIVIAL(error) << "Static handler's path not found in URI... returning 500 response";
        reply = Reply::stock_reply(Reply::internal_server_error);
        return;
    }

    std::string extension = get_extension(request_path);

    // Open the file to send back.
    std::string full_path = "." + request_path;
    BOOST_LOG_TRIVIAL(info) << "Full path " << full_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is)
    {
        reply = Reply::stock_reply(Reply::not_found);
        return;
    }

    // Fill out the reply to be sent to the client.
    reply.status = Reply::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        reply.content.append(buf, is.gcount());

    reply.headers.resize(2);
    reply.headers[0].name = "Content-Length";
    reply.headers[0].value = std::to_string(reply.content.size());
    reply.headers[1].name = "Content-Type";
    reply.headers[1].value = mime_types::extension_to_type(extension);
    BOOST_LOG_TRIVIAL(warning) << "static request handler finish preparing reply";  
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
