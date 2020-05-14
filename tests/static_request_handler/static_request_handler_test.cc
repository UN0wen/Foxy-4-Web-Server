#include "gtest/gtest.h"
#include <fstream>
#include "static_request_handler.h"

class StaticRequestHandlerTest : public ::testing::Test
{
protected:
    StaticRequestHandler request_handler = StaticRequestHandler("./test_data/", "/test/");
    const char *get_request = "GET /test/ HTTP/1.1\r\n\
Host: localhost:8000\r\n\r\n";

    Request request;
    StaticRequestHandlerTest(){
        request.method_ = Request::MethodEnum::POST;
        request.uri_ = "/test/";
        request.http_version_ = "1.1";
        request.headers_["Host"] = "localhost:8000";
        request.body_ = "";
    }
};

TEST_F(StaticRequestHandlerTest, GetIndexFile)
{    
    const char* path = "./test_data/index.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;

    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());
    request.uri_ = "/test/";
    Response response = request_handler.handle_request(request);
    bool success_header = response.headers_["Content-Type"] == "text/html";    
    bool success_content = response.body_ == content;
    bool success_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(success_header && success_status && success_content);
}

TEST_F(StaticRequestHandlerTest, GetIndexFileWithSpace)
{    
    const char* path = "./test_data/index 2.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;

    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());

//     const char *space_request = "POST /test/index+2.html HTTP/1.1\r\n\
// Host: localhost:8000\r\n\r\n";
//     request.raw_request = space_request;
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/test/index+2.html";

    Response response = request_handler.handle_request(request);
    bool success_header = response.headers_["Content-Type"] ==  "text/html";
    bool success_content = response.body_ == content;
    bool success_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(success_header && success_content  && success_status);
}

TEST_F(StaticRequestHandlerTest, FileNotFound)
{
    request.uri_ = "/test/nofile.html";
    request.method_ = Request::MethodEnum::POST;
    Response response = request_handler.handle_request(request);
    bool bad_status = response.code_ == Response::status_code::not_found;

    EXPECT_TRUE(bad_status);
}

TEST_F(StaticRequestHandlerTest, UrlEncodingError)
{    
    

    request.uri_ = "/test/index%GG.html";
    request.method_ = Request::MethodEnum::POST;

    Response response = request_handler.handle_request(request);
    bool status = response.code_ == Response::status_code::bad_request;

    EXPECT_TRUE(status);
}

TEST_F(StaticRequestHandlerTest, UnexpectedEndOfURL)
{    
    
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/test/index.html%20%2";

    Response response = request_handler.handle_request(request);
    bool status = response.code_ == Response::status_code::bad_request;

    EXPECT_TRUE(status);
}

TEST_F(StaticRequestHandlerTest, UrlRelativePath)
{    
    
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "./test/index.html";

    Response response = request_handler.handle_request(request);
    bool status = response.code_ == Response::status_code::bad_request;

    EXPECT_TRUE(status);
}

TEST_F(StaticRequestHandlerTest, WrongPath)
{    
    
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/not_test/index.html";

    Response response = request_handler.handle_request(request);
    bool status = response.code_ == Response::internal_server_error;

    EXPECT_TRUE(status);
}

