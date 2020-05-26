#include "gtest/gtest.h"
#include "proxy_request_handler.h"
#include "config_parser.h"

class ProxyRequestHandlerTest : public ::testing::Test
{
protected:
    ProxyRequestHandler request_handler = ProxyRequestHandler("ucla.edu", "/ucla/", 80);

    Request request;
    
    ProxyRequestHandlerTest(){
        request.method_ = Request::MethodEnum::GET;
        request.uri_ = "/ucla/admissions";
        request.headers_["Host"] = "localhost:8000";
        request.headers_["Connection"] = "close";
        request.http_version_ = "1.1";
    }
};

TEST_F(ProxyRequestHandlerTest, ProxyMethodsTest)
{
    Response response = request_handler.handle_request(request);
    bool success_status1 = response.code_ == Response::status_code::moved_temporarily;

    request.method_ = Request::MethodEnum::POST;
    response = request_handler.handle_request(request);
    bool success_status2 = response.code_ == Response::status_code::moved_temporarily;

    request.method_ = Request::MethodEnum::PUT;
    response = request_handler.handle_request(request);
    bool success_status3 = response.code_ == Response::status_code::moved_temporarily;

    request.method_ = Request::MethodEnum::UNKNOWN;
    response = request_handler.handle_request(request);
    bool success_status4 = response.code_ == Response::status_code::moved_temporarily;
    
    EXPECT_TRUE(success_status1 && success_status2 && success_status3 && success_status4);
}

TEST_F(ProxyRequestHandlerTest, ProxRedirectTest)
{
    NginxConfigParser parser;
    NginxConfig config;
    const char* filename = "proxy_test.conf";
    parser.Parse(filename, &config);
    request_handler.Init("/ucla/", config);
    //request.uri_ = "/ucla/admissions";
    Response response = request_handler.handle_request(request);
    bool success_status = response.code_ == Response::status_code::moved_temporarily;

    EXPECT_TRUE(success_status);
}

TEST_F(ProxyRequestHandlerTest, ProxyInitTest)
{
    NginxConfigParser parser;
    NginxConfig config;
    const char* filename = "proxy_test.conf";
    parser.Parse(filename, &config);
    request_handler.Init("/bad_path/", config);
    Response response = request_handler.handle_request(request);
    bool success_status = response.code_ == Response::status_code::moved_temporarily;
    EXPECT_TRUE(success_status);
}

