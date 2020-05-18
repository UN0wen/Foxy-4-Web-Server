#include "data_collector.h"

DataCollector* DataCollector::singleton_ = nullptr;

DataCollector *DataCollector::get_instance()
{
      // Lazy instantiation
      if (singleton_ == nullptr)
      {
            singleton_ = new DataCollector();
      }

      return singleton_;
}

std::string DataCollector::map_status_to_string(Response::status_code status_code)
{
      switch (status_code)
      {
      case Response::ok:
            return "200";
      case Response::created:
            return "201";
      case Response::accepted:
            return "202";
      case Response::no_content:
            return "204";
      case Response::multiple_choices:
            return "300";
      case Response::moved_permanently:
            return "301";
      case Response::moved_temporarily:
            return "302";
      case Response::not_modified:
            return "304";
      case Response::bad_request:
            return "400";
      case Response::unauthorized:
            return "401";
      case Response::forbidden:
            return "403";
      case Response::not_found:
            return "404";
      case Response::internal_server_error:
            return "500";
      case Response::not_implemented:
            return "501";
      case Response::bad_gateway:
            return "502";
      case Response::service_unavailable:
            return "503";
      default:
            return "500";
      }
}

void DataCollector::increment_request(std::string uri, Response::status_code status_code)
{
      //do stuff here
      std::string status = map_status_to_string(status_code);
      if (status_map_.count(uri) == 1)
      {
            if (status_map_[uri].count(status) == 1)
            {
                  status_map_[uri][status]++;
            }
            else
            {
                  status_map_[uri][status] = 1;
            }
      }
      else
      {
            status_map_[uri][status] = 1;
      }
      return;
}

void DataCollector::add_handler(std::string path, std::string handler_name) {
      uri_request_handler_[path] = handler_name;
}
