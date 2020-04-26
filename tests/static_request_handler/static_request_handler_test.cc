#include "gtest/gtest.h"
#include <fstream>
#include "static_request_handler.h"

class StaticRequestHandlerTest : public ::testing::Test
{
protected:
    StaticRequestHandler request_handler = StaticRequestHandler("/test_data", "/test");
    const char *get_request = "GET /test/ HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    Reply reply;
    Header header_1 = {
        "Host",          //
        "localhost:8000" //value
    };

    std::vector<Header> headers = {header_1};

    Request request = {
        "GET",                // std::string method;
        "/test/",                   // std::string uri;
        1,                     // int http_version_major;
        1,                     // int http_version_minor;
        headers,               // std::vector<Header> headers;
        "",                    // std::string data;
        get_request           // const char raw_request
    };
};

TEST_F(StaticRequestHandlerTest, GetIndexFile)
{    
    const char* path = "./test_data/index.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;

    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());

    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool success_header_name = reply.headers[1].name == "Content-Type";
    bool success_content = reply.content == content;
    bool success_header_value = reply.headers[1].value == "text/html";
    bool success_status = reply.status == Reply::ok;

    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}

TEST_F(StaticRequestHandlerTest, GetIndexFileWithSpace)
{    
    const char* path = "./test_data/index 2.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;

    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());

    const char *space_request = "POST /test/index+2.html HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = space_request;
    request.uri = "/test/index+2.html";

    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool success_header_name = reply.headers[1].name == "Content-Type";
    bool success_content = reply.content == content;
    bool success_header_value = reply.headers[1].value == "text/html";
    bool success_status = reply.status == Reply::ok;

    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}

TEST_F(StaticRequestHandlerTest, FileNotFound)
{
    const char *not_found_request = "POST /test/nofile.html HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = not_found_request;
    request.uri = "/test/nofile.html";

    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool bad_header_name = reply.headers[1].name == "Content-Type";
    bool bad_header_value = reply.headers[1].value == "text/html";
    bool bad_status = reply.status == Reply::not_found;

    EXPECT_TRUE(bad_header_name && bad_header_value && bad_status);
}

TEST_F(StaticRequestHandlerTest, UrlEncodingError)
{    
    const char *unexpected_request = "POST /test/index%GG.html HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = unexpected_request;
    request.uri = "/test/index%GG.html";
    
    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool header_value = reply.headers[1].value == "text/html";
    bool status = reply.status == Reply::bad_request;

    EXPECT_TRUE(header_value && status);
}

TEST_F(StaticRequestHandlerTest, UnexpectedEndOfURL)
{    
    const char *unexpected_request = "POST /test/index.html%20%2 HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = unexpected_request;
    request.uri = "/test/index.html%20%2";

    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool header_value = reply.headers[1].value == "text/html";
    bool status = reply.status == Reply::bad_request;

    EXPECT_TRUE(header_value && status);
}

TEST_F(StaticRequestHandlerTest, UrlRelativePath)
{    
    const char *relative_path_request = "POST ./test/index.html HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = relative_path_request;
    request.uri = "./test/index.html";

    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool header_value = reply.headers[1].value == "text/html";
    bool status = reply.status == Reply::bad_request;

    EXPECT_TRUE(header_value && status);
}

TEST_F(StaticRequestHandlerTest, WrongPath)
{    
    const char *wrong_path_request = "POST /not_test/index.html HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = wrong_path_request;
    request.uri = "/not_test/index.html";

    request_handler.handle_request(request, reply, RequestParser::result_type::good);
    bool header_value = reply.headers[1].value == "text/html";
    bool status = reply.status == Reply::internal_server_error;

    EXPECT_TRUE(header_value && status);
}

TEST_F(StaticRequestHandlerTest, BadRequest)
{    
    const char *bad_request = "POST /test/index.html BAD/1.1\r\n\
Host: localhost:8000\r\n\r\n";
    request.raw_request = bad_request;
    request.uri = "/test/index.html";

    request_handler.handle_request(request, reply, RequestParser::result_type::bad);
    bool header_value = reply.headers[1].value == "text/html";
    bool status = reply.status == Reply::bad_request;

    EXPECT_TRUE(header_value && status);
}

