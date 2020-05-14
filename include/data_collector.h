#pragma once

#include <string>
#include <map>

class DataCollector {
  public:
    //I was thinking this could be (URI, times recieved, response code), feel free to change
    static std::map<std::string, std::map<int, int>> status_map;
    static void increment_request(std::string uri);
};

