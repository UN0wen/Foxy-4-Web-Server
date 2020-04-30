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

/*get_port() function:
  Currently returns the first port number found in the first server
  block and assumes correct format was given from NginxConfig.Parse().*/
bool NginxConfig::get_port(int *port)
{
  //bfs through config blocks
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "listen");
    //if find is at tokens_.end() that means "listen" was not found
    //listen also cannot be the last token (std::prev) 
    //the tokens vector must have 2 elements: "listeb" followed by the port number
    if (find != statement->tokens_.end() &&
        find != std::prev(statement->tokens_.end()) &&
        statement->tokens_.size() == 2)
    {
      //if all the if statement conditions are satisfied the port number is the token after "listen"
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
    //if "listen", check the child block
    if (statement->child_block_.get() != nullptr)
    {
      //if the child block exists, run getport for the child block
      return statement->child_block_->get_port(port);
    };
  }
  return false;
}

/*get_map() function:
  Crawls through config file and returns a mapping of paths to RequestHandler objects
  from every server block.
  Logic repeats many of the same concepts found in get_port()*/
bool NginxConfig::get_map(std::map<std::string, std::shared_ptr<RequestHandler>> *mapping)
{
  for (const auto &statement : statements_)
  {
  	//find server blocks
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "server");
    if (find != statement->tokens_.end())
    {
      std::string path, root, method = "";
      for (const auto &s : statement->child_block_->statements_)
      {
      	//search for keywords
        std::vector<std::string>::iterator find_path = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "path");
        std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "root");
        std::vector<std::string>::iterator find_method = std::find(s->tokens_.begin(),
                                                                   s->tokens_.end(),
                                                                   "method");

        if(s->tokens_.size() != 2 ||
           find_path == std::prev(s->tokens_.end()) ||
           find_root == std::prev(s->tokens_.end()) ||
           find_method == std::prev(s->tokens_.end()))
        {
        	return false;
        }
        //assign values if keywords are found
        if (find_path != s->tokens_.end())
        {
          path = *(find_path + 1);
        }

        if (find_root != s->tokens_.end())
        {
          root = *(find_root + 1);
        }
        if (find_method != s->tokens_.end())
        {
          method = *(find_method + 1);
        }
      }

      if (method == "static" &&
      	  !root.empty() && !path.empty())
      {
          mapping->insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new StaticRequestHandler(root, path)));
      }
      else if (method == "echo" &&
      	       !path.empty())
      {
          mapping->insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new EchoRequestHandler()));
      }
      else 
      {
      	return false;
      }
    }
  }

  return true;
}
