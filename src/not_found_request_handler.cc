#include "not_found_request_handler.h"
#include "response_generator.h"
#include "data_collector.h"


NotFoundRequestHandler::NotFoundRequestHandler()
{
}

RequestHandler *NotFoundRequestHandler::Init(const std::string &location_path, const NginxConfig &config)
{
  NotFoundRequestHandler *not_found_request_handler = new NotFoundRequestHandler();
  std::string temp = "404 Not Found Request Handler";
	DataCollector::uri_request_handler[location_path] = temp;
  return not_found_request_handler;
}

Response NotFoundRequestHandler::handle_request(const Request &request){
    Response response = ResponseGenerator::stock_response(Response::not_found);
    return response;
}