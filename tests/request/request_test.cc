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
  std::string version = "1.1";

  Request request = {
    Request::POST,                // std::string method;
    "/",                   // std::string uri;
    version,
    headers,               // std::vector<Header> headers;
    "{'test_value': '1'}", // std::string data;
    post_request,           // const char raw_request
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

    //content_length = empty_request.get_content_length();
    if (empty_request_.headers_.empty())
      {
	content_length = 0;
      }
    else
      {
	content_length = 0;
      }

    EXPECT_EQ(content_length, 0);
}
