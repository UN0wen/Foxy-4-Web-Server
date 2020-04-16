// An nginx config file parser.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
 public:
  std::string ToString(int depth = 0);
  //gets the port number specified in nginx file
  bool GetPort(int* port);
  std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
};