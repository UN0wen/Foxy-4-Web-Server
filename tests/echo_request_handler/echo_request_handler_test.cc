#include "gtest/gtest.h"
#include "echo_request_handler.h"

class EchoRequestHandlerTest : public ::testing::Test
{
protected:
    EchoRequestHandler request_handler;
    const int CONTENT_LENGTH = 19;
    const int POST_REQUEST_LENGTH = 93;
    const char *post_request = "POST / HTTP/1.1\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\r\n{'test_value': '1'}";

    Request request;
    
    EchoRequestHandlerTest(){
        NginxConfig config;
        request_handler.Init("/echo/", config);
        request.method_ = Request::MethodEnum::POST;
        request.uri_ = "/";
        request.http_version_ = "1.1";
        request.headers_["Host"] = "localhost:8000";
        request.headers_["Content-Length"] = "19";
        request.headers_[ "Content-Type"] =  "application/json";
        request.body_ = "{'test_value': '1'}";
    }
};

TEST_F(EchoRequestHandlerTest, SendingGoodStatus)
{
    Response response = request_handler.handle_request(request);
    bool success_header_value = response.headers_["Content-Type"] == "text/plain";
    bool success_status = response.code_ == Response::status_code::ok;
    bool success_body = response.body_ == std::string(post_request);
    EXPECT_TRUE(success_header_value  && success_status && success_body);
}



