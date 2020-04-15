// An nginx config file parser.

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "config_statement.h"

// The parsed representation of the entire config.
class NginxConfig {
 public:
  std::string ToString(int depth = 0);
  //gets the port number specified in nginx file
  bool GetPort(int* port);
  std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
};