#include "gtest/gtest.h"
#include "not_found_request_handler.h"
#include <string>

class NotFoundRequestHandlerTest : public ::testing::Test
{
protected:
    RequestHandler* nf_handler_;

    Request request;
    
    NotFoundRequestHandlerTest(){
        NginxConfig config;
        nf_handler_ = NotFoundRequestHandler::Init("/echo/", config);
        request.method_ = Request::MethodEnum::POST;
        request.uri_ = "/";
        request.http_version_ = "1.1";
        request.headers_["Host"] = "localhost:8000";
        request.headers_["Content-Length"] = "19";
        request.headers_[ "Content-Type"] =  "application/json";
        request.body_ = "{'test_value': '1'}";
    } 
};

TEST_F(NotFoundRequestHandlerTest, NotFoundTest)
{
    Response response;

    response = nf_handler_->handle_request(request);

    bool success_header_value = response.headers_["Content-Type"] == "text/html";
    bool success_status = response.code_ == Response::status_code::not_found;
    bool success_body = response.body_.find("404 Not Found") != std::string::npos;
    EXPECT_TRUE(success_header_value  && success_status && success_body);
}



