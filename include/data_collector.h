#pragma once

#include <string>
#include <map>
#include "response.h"

class DataCollector {
  public:
    // Disable copy and assignment operators
    DataCollector(DataCollector &other) = delete;

    void operator=(const DataCollector& other) = delete;

    // Return the instance of DataCollector
    static DataCollector* get_instance();

    // Increment the count of request with uri and status_code by 1
    void increment_request(std::string uri, Response::status_code status_code);
  
    std::string map_status_to_string(Response::status_code status_code);

    // Add a handler with name and path to the status page
    void add_handler(std::string path, std::string handler_name);

    std::map<std::string, std::map<std::string, int>> status_map() {
      return status_map_;
    }

    std::map<std::string, std::string> uri_request_handler() {
      return uri_request_handler_;
    }

  protected:
    DataCollector() {};
    static DataCollector* singleton_;

    // <uri, <status code, times>>
    std::map<std::string, std::map<std::string, int>> status_map_;

    // <path, request_handler_name>
    std::map<std::string, std::string> uri_request_handler_;
};

