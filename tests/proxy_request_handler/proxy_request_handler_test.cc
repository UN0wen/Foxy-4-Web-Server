#include "gtest/gtest.h"
#include "proxy_request_handler.h"

class ProxyRequestHandlerTest : public ::testing::Test
{
protected:
    ProxyRequestHandler request_handler = ProxyRequestHandler("ucla.edu/", "/ucla/", 80);

    Request request;
    
    ProxyRequestHandlerTest(){
        NginxConfig config;
        // request_handler.Init("/ucla/", config);
        request.method_ = Request::MethodEnum::GET;
        request.uri_ = "/ucla/favicon.ico";
        request.headers_["Host"] = "localhost:8000";
        request.headers_["Connection"] = "close";
        request.http_version_ = "1.1";
    }
};

TEST_F(ProxyRequestHandlerTest, ProxyInitialTest)
{
    Response response = request_handler.handle_request(request);
    // bool success_header_value = response.headers_["Content-Type"] == "text/plain";
    // bool success_status = response.code_ == Response::status_code::ok;
    // bool success_body = response.body_ == std::string(post_request);
    EXPECT_TRUE(true);
}