#include "health_request_handler.h"
#include "response_generator.h"
#include "data_collector.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

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
    BOOST_LOG_TRIVIAL(info) << "[HealthRequestHandler] health request handler is handling request.";
    response.code_ = Response::ok;
    response.body_ = "OK";

    response.headers_["Content-Length"] = std::to_string(response.body_.size());
    response.headers_["Content-Type"] = "text/plain";
    BOOST_LOG_TRIVIAL(info) << "[ResponseLength] " << std::to_string(response.body_.size());
    BOOST_LOG_TRIVIAL(info) << "[HealthRequestHandler] finish processing request.";
    return response;
}