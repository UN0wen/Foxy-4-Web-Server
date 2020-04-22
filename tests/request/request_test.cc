#include "gtest/gtest.h"
#include "request.h"

class RequestTest : public ::testing::Test
{
protected:
    Request empty_request;

    const char *post_request = "POST / HTTP/1.1\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\r\n{'test_value': '1'}";
    const int CONTENT_LENGTH = 19;

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
        "POST",                // std::string method;
        "/",                   // std::string uri;
        1,                     // int http_version_major;
        1,                     // int http_version_minor;
        headers,               // std::vector<Header> headers;
        "{'test_value': '1'}", // std::string data;
        post_request           // const char raw_request
    };
};

TEST_F(RequestTest, ContentLength)
{
    int content_length;

    content_length = request.get_content_length();

    EXPECT_EQ(content_length, CONTENT_LENGTH);
}

TEST_F(RequestTest, NoContentLength)
{
    int content_length;

    content_length = empty_request.get_content_length();

    EXPECT_EQ(content_length, 0);
}
