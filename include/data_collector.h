#pragma once

#include <string>
#include <map>
#include "response.h"

class DataCollector {
  public:
    // <uri, <status code, times>>
    static std::map<std::string, std::map<std::string, int>> status_map;

    // <path, request_handler_name>
    static std::map<std::string, std::string> uri_request_handler;

    // Increment the count of request with uri and status_code by 1
    static void increment_request(std::string uri, Response::status_code status_code);
  
    static std::string map_status_to_string(Response::status_code status_code);

    void init();
};

