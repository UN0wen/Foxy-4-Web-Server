// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <string>
#include <vector>
#include <algorithm>
#include "config_parser.h"
#include "utility.h"

bool NginxConfig::valid_keyword_listen(std::vector<std::string>::iterator find,
                                       const std::shared_ptr<NginxConfigStatement> &statement)
{
  if (find != statement->tokens_.end() &&
      find != std::prev(statement->tokens_.end()))
    return true;
  return false;
}

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
bool NginxConfig::get_port(std::vector<std::pair<int, std::string>> &vec)
{
  int port;
  std::string type;
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "listen");

    if (valid_keyword_listen(find, statement) && statement->tokens_.size() == 3)
    {
      //if all the listen keyword is found, the port number is the token after "listen"
      std::string portstring = *(find + 2);
      if (portstring.find_first_not_of("0123456789") == std::string::npos)
      {
        port = std::stoi(portstring);
        type = *(find + 1);
        if (type != "https" && type != "http")
          return false;
        vec.push_back(std::pair<int, std::string>(port, type));
        continue;
      }
      else
      {
        return false;
      }
    }

    if (statement->child_block_.get() != nullptr)
    {
      statement->child_block_->get_port(vec);
    };
  }
  return !vec.empty();
}

bool NginxConfig::get_threads(int *threads)
{
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "threads");

    if (valid_keyword_listen(find, statement))
    {
      std::string threadstring = *(find + 1);
      if (threadstring.find_first_not_of("0123456789") == std::string::npos)
      {
        *threads = std::stoi(threadstring);
        return true;
      }
      else
      {
        return false;
      }
    }
    if (statement->child_block_.get() != nullptr)
    {
      return statement->child_block_->get_threads(threads);
    };
  }
  return false;
}

bool NginxConfig::get_dir(std::string *dir)
{
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "dir");

    if (valid_keyword_listen(find, statement))
    {
      *dir = *(find + 1);
      if (!utility::is_quoted(dir))
        return false;
      return true;
    }
  }
  return true;
}

bool NginxConfig::get_hostname(std::string *hostname)
{
  for (const auto &statement : statements_)
  {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
                                                        statement->tokens_.end(),
                                                        "hostname");

    if (valid_keyword_listen(find, statement))
    {
      *hostname = *(find + 1);
      if (!utility::is_quoted(hostname))
        return false;
      return true;
    }
  }
  return true;
}