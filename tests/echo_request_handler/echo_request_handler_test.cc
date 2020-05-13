#include "gtest/gtest.h"
#include "echo_request_handler.h"

class EchoRequestHandlerTest : public ::testing::Test
{
protected:
    EchoRequestHandler request_handler;
    const int CONTENT_LENGTH = 19;
    const char *post_request = "POST / HTTP/1.1\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\r\n{'test_value': '1'}";

    const int POST_REQUEST_LENGTH = 93;

    Header header_1 = {
        "Host",          //
        "localhost:8000" //value
    };

    Header header_2 = {
        "Content-Length", //name
        "19"              //value
    };

    Header header_3 = {
        "Content-Type",    //name
        "application/json" //value
    };

    std::vector<Header> headers = {header_1, header_2, header_3};

    Request request = {
      Request::POST,                // std::string method;
        "/",                   // std::string uri;
      "1.1",
        headers,               // std::vector<Header> headers;
        "{'test_value': '1'}", // std::string data;
        post_request           // const char raw_request
    };
};

TEST_F(EchoRequestHandlerTest, SendingGoodStatus)
{
    Response response;
    request_handler.handle_request(request, response, RequestParser::result_type::good);
    bool success_header_name = response.headers[1].name == "content-type";
    bool success_content = response.content == post_request;
    bool success_header_value = response.headers[1].value == "text/plain";
    bool success_status = response.status == Response::ok;

    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}

TEST_F(EchoRequestHandlerTest, SendingBadStatus)
{
    Response response;
    response.status = Response::bad_request;
    request_handler.handle_request(request, response, RequestParser::result_type::bad);
    bool bad_header_name = response.headers[1].name == "content-type";
    bool bad_content = response.content == post_request;
    bool bad_header_value = response.headers[1].value == "text/plain";
    bool bad_status = response.status == Response::bad_request;

    EXPECT_TRUE(bad_header_name && bad_content && bad_header_value && bad_status);
}


