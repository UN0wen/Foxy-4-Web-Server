#include "gtest/gtest.h"
#include "request_handler.h"

class RequestHandlerTest : public ::testing::Test
{
protected:
    RequestHandler request_handler;
    RequestHandler empty_request_handler;
    const int CONTENT_LENGTH = 19;
    const char *post_request = "POST / HTTP/1.1\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\r\n";

    const int POST_REQUEST_LENGTH = 93;
    void SetUp()
    {
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
            "POST", // std::string method;
            "/",    // std::string uri;
            1,      // int http_version_major;
            1,      // int http_version_minor;
            headers // std::vector<Header> headers;
        };

        request_handler.set_request(request);
    }
};

TEST_F(RequestHandlerTest, InvalidHttpParseTest)
{
    char msg[] = "hello\n";
    RequestParser::result_type result = empty_request_handler.http_format_precheck(msg, 6);
    EXPECT_TRUE(result == RequestParser::bad);
}

TEST_F(RequestHandlerTest, SendingGoodStatus)
{
    char msg[] = "hello\n";
    Reply rep = empty_request_handler.process_request(true, msg);
    bool success_header_name = rep.headers[1].name == "content-type";
    bool success_content = rep.content == "hello\n";
    bool success_header_value = rep.headers[1].value == "text/plain";
    bool success_status = rep.status == Reply::ok;

    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}

TEST_F(RequestHandlerTest, SendingBadStatus)
{
    char msg[] = "hello\n";
    Reply rep = empty_request_handler.process_request(false, msg);
    bool bad_header_name = rep.headers[1].name == "content-type";
    bool bad_content = rep.content == "hello\n";
    bool bad_header_value = rep.headers[1].value == "text/plain";
    bool bad_status = rep.status == Reply::bad_request;

    EXPECT_TRUE(bad_header_name && bad_content && bad_header_value && bad_status);
}

TEST_F(RequestHandlerTest, ContentLength)
{
    int content_length;

    content_length = request_handler.get_content_length();

    EXPECT_EQ(content_length, CONTENT_LENGTH);
}

TEST_F(RequestHandlerTest, NoContentLength)
{
    int content_length;

    content_length = empty_request_handler.get_content_length();

    EXPECT_EQ(content_length, 0);
}

TEST_F(RequestHandlerTest, PostRequestNoData)
{
    RequestParser::result_type result = empty_request_handler.http_format_precheck(post_request, POST_REQUEST_LENGTH);

    EXPECT_EQ(result, RequestParser::result_type::missing_data);
}

TEST_F(RequestHandlerTest, PostRequestWithData)
{
    const char *data_request = "POST / HTTP/1.1\r\n\
Content-Length: 19\r\n\
Content-Type: application/json\r\n\
Host: localhost:8000\r\n\r\n\
{'test_value': '1'}";
    RequestParser::result_type result = empty_request_handler.http_format_precheck(data_request, POST_REQUEST_LENGTH + CONTENT_LENGTH);

    EXPECT_EQ(result, RequestParser::result_type::good);
}

TEST_F(RequestHandlerTest, AddingRequestToHandler)
{
    Request req = request_handler.get_request();
    empty_request_handler.set_request(req);

    int content_length = empty_request_handler.get_content_length();

    EXPECT_EQ(content_length, CONTENT_LENGTH);
}   

TEST_F(RequestHandlerTest, EmptyData)
{
    const char* empty_data;

    RequestParser::result_type result = empty_request_handler.http_format_precheck(empty_data, 0);

    EXPECT_EQ(result, RequestParser::result_type::indeterminate);
}