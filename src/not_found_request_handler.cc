#include "not_found_request_handler.h"
#include "response_generator.h"
#include "data_collector.h"
#include <boost/log/trivial.hpp>


NotFoundRequestHandler::NotFoundRequestHandler()
{
}

RequestHandler *NotFoundRequestHandler::Init(const std::string &location_path, const NginxConfig &config)
{
  NotFoundRequestHandler *not_found_request_handler = new NotFoundRequestHandler();
  DataCollector::get_instance()->add_handler(location_path, "404 Not Found Request Handler");
  return not_found_request_handler;
}

Response NotFoundRequestHandler::handle_request(const Request &request){
    BOOST_LOG_TRIVIAL(info) << "[404RequestHandler] 404 request handler is handling request.";
    Response response = ResponseGenerator::stock_response(Response::not_found);
    BOOST_LOG_TRIVIAL(info) << "[404RequestHandler] 404 request handler finish processing request";
    return response;
}