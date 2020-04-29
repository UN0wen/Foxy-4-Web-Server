// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>

#include "config_parser.h"
#include "static_request_handler.h"
#include "echo_request_handler.h"
std::string NginxConfig::ToString(int depth)
{
  std::string serialized_config;
  for (const auto &statement : statements_)
  {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

/*Prototype get_port() function, kind of hacky and needs adjustments.

  Currently returns the first port number found in the first server
  block and assumes correct format was given from NginxConfig.Parse().

  May want to incorporate catching multiple ports if multiple server 
  blocks are required later.
*/
bool NginxConfig::get_port(int *port)
{
  //bfs through config blocks
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "listen");
    if (find != statement->tokens_.end() &&
        find != std::prev(statement->tokens_.end()) &&
        statement->tokens_.size() == 2)
    {

      std::string portstring = *(find + 1);
      if (portstring.find_first_not_of("0123456789") == std::string::npos)
      {
        *port = std::stoi(portstring);
        return true;
      }
      else
      { //token after listen is not numeric
        return false;
      }
    }
    if (statement->child_block_.get() != nullptr)
    {
      return statement->child_block_->get_port(port);
    };
  }
  return false;
}

std::map<std::string, std::shared_ptr<RequestHandler>> NginxConfig::get_map()
{
  std::map<std::string, std::shared_ptr<RequestHandler>> map;
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "server");
    if (find != statement->tokens_.end())
    {
      std::string path, root, method;
      for (const auto &s : statement->child_block_->statements_)
      {
        std::vector<std::string>::iterator find_path = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "path");
        std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "root");
        std::vector<std::string>::iterator find_method = std::find(s->tokens_.begin(),
                                                                   s->tokens_.end(),
                                                                   "method");
        if (find_path != s->tokens_.end() &&
            find_path != std::prev(s->tokens_.end()) && s->tokens_.size() == 2)
        {
          path = *(find_path + 1);
        }

        if (find_root != s->tokens_.end() &&
            find_root != std::prev(s->tokens_.end()) && s->tokens_.size() == 2)
        {
          root = *(find_root + 1);
        }
        if (find_method != s->tokens_.end() &&
            find_method != std::prev(s->tokens_.end()) && s->tokens_.size() == 2)
        {
          method = *(find_method + 1);
        }
      }
      if (method == "static")
      {
        map.insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new StaticRequestHandler(root, path)));
      }
      else
      {
        EchoRequestHandler request_handler;
        map.insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new EchoRequestHandler()));
      }
    }
  }

  return map;
}
