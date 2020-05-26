#include "health_request_handler.h"
#include "response_generator.h"
#include "data_collector.h"

HealthRequestHandler::HealthRequestHandler()
{
}

RequestHandler *HealthRequestHandler::Init(const std::string &location_path, const NginxConfig &config)
{
    HealthRequestHandler *health_request_handler = new HealthRequestHandler();
    DataCollector::get_instance()->add_handler(location_path, "Health Request Handler");
    return health_request_handler;
}

Response HealthRequestHandler::handle_request(const Request &request)
{
    Response response;
    response.code_ = Response::ok;
    response.body_ = "OK";

    response.headers_["Content-Length"] = std::to_string(response.body_.size());
    response.headers_["Content-Type"] = "text/plain";
    
    return response;
}