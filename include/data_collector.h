#pragma once

#include <string>
#include <map>
#include "response.h"

class DataCollector {
  public:
    //<uri, <status code, times>>
    static std::map<std::string, std::map<std::string, int>> status_map;
    static std::map<std::string, std::string> uri_request_handler;
    static void increment_request(std::string uri, Response::status_code status_code);
    static std::string map_status_to_string(Response::status_code status_code);
    void init();
};

