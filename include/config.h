// An nginx config file parser.
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct PathToRoot {
  std::string root;
  std::string path;
};

class NginxConfig;

// The parsed representation of a single config statement.
class NginxConfigStatement {
 public:
  std::string ToString(int depth);
  std::vector<std::string> tokens_;
  std::unique_ptr<NginxConfig> child_block_;
};

// The parsed representation of the entire config.
class NginxConfig {
 private:
  bool valid_keyword_listen(std::vector<std::string>::iterator find,
			    const auto &statement);
 public:
  std::string ToString(int depth = 0);
  //gets the port number specified in nginx file
  bool get_port(int* port);
  std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
  
};

bool is_quoted(std::string* s);
