#include "gtest/gtest.h"
#include "health_request_handler.h"
#include <string>

class HealthRequestHandlerTest : public ::testing::Test
{
protected:
    RequestHandler* health_handler_;

    Request request;
    
    HealthRequestHandlerTest(){
        NginxConfig config;
        health_handler_ = HealthRequestHandler::Init("/health", config);
        request.method_ = Request::MethodEnum::POST;
        request.uri_ = "/health";
        request.http_version_ = "1.1";
        request.headers_["Host"] = "localhost:8000";
        request.headers_["Content-Length"] = "19";
        request.headers_[ "Content-Type"] =  "application/json";
        request.body_ = "{'test_value': '1'}";
    } 
};

TEST_F(HealthRequestHandlerTest, HealthTest)
{
    Response response;

    response = health_handler_->handle_request(request);

    bool success_header_value = response.headers_["Content-Type"] == "text/plain";
    bool success_status = response.code_ == Response::status_code::ok;
    bool success_body = response.body_ == "OK";
    EXPECT_TRUE(success_header_value  && success_status && success_body);
}



