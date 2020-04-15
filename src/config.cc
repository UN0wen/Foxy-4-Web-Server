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

std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

/*Prototype GetPort() function, kind of hacky and needs adjustments.

  Currently returns the first port number found in the first server
  block and assumes correct format was given from NginxConfig.Parse().

  May want to incorporate catching multiple ports if multiple server 
  blocks are required later.
*/
bool NginxConfig::GetPort(int* port) {
  //bfs through config blocks
  for (const auto& statement : statements_) {
    std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(), 
                                                        statement->tokens_.end(), 
                                                        "listen");
    if (find != statement->tokens_.end() &&
      find != std::prev(statement->tokens_.end()) &&
      statement->tokens_.size() == 2) {

      std::string portstring = *(find+1);
  	  if(portstring.find_first_not_of("0123456789") == std::string::npos) {
	    *port = std::stoi(portstring);
	    return true;
	  }
	  else { //token after listen is not numeric
	  	return false;
	  }
    }
    if (statement->child_block_.get() != nullptr) {
      return statement->child_block_->GetPort(port);
    };
  }
  return false;
}