#include "gtest/gtest.h"
#include <fstream>
#include "login_request_handler.h"
#include "database.h"

class LoginRequestHandlerTest : public ::testing::Test
{
protected:
    LoginRequestHandler request_handler = LoginRequestHandler("./test_data/", "/test/");
    Request request;
    LoginRequestHandlerTest()
    {
        request.method_ = Request::MethodEnum::GET;
        request.uri_ = "/test/";
        request.http_version_ = "1.1";
        request.headers_["Host"] = "localhost:8000";
        request.body_ = "";
        database::db_create();
        database::db_insert("admin", "admin");
    }
};

TEST_F(LoginRequestHandlerTest, GetIndexFile)
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

TEST_F(LoginRequestHandlerTest, getLoginFailure)
{
    const char* path = "./test_data/login_failure.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/test/";
    request.body_ = "username=bad_testing&password=testing";
    Response response = request_handler.handle_request(request);
    bool fail_header = response.headers_["Content-Type"] == "text/html";    
    bool fail_content = response.body_ == content;
    bool fail_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(fail_header && fail_status && fail_content);
}

TEST_F(LoginRequestHandlerTest, getLoginSuccess)
{    
    const char* path = "./test_data/login_success.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/test/";
    request.body_ = "username=admin&password=admin";
    Response response = request_handler.handle_request(request);
    bool success_header = response.headers_["Content-Type"] == "text/html";    
    bool success_content = response.body_ == content;
    bool success_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(success_header && success_status && success_content);
}

TEST_F(LoginRequestHandlerTest, getSignupIndex)
{    
    const char* path = "./test_data/signup/index.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());
    request.method_ = Request::MethodEnum::GET;
    request.uri_ = "/test/signup/";
    Response response = request_handler.handle_request(request);
    bool success_header = response.headers_["Content-Type"] == "text/html";    
    bool success_content = response.body_ == content;
    bool success_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(success_header && success_status && success_content);
}

TEST_F(LoginRequestHandlerTest, getSignupSuccess)
{    
    const char* path = "./test_data/signup/signup_success.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/test/signup/";
    request.body_ = "username=testing&password=testing";
    Response response = request_handler.handle_request(request);
    bool success_header = response.headers_["Content-Type"] == "text/html";    
    bool success_content = response.body_ == content;
    bool success_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(success_header && success_status && success_content);
}

TEST_F(LoginRequestHandlerTest, getSignupFailed)
{    
    const char* path = "./test_data/signup/signup_failure.html";
    std::ifstream is(path , std::ios::in | std::ios::binary);
    std::string content;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        content.append(buf, is.gcount());
    request.method_ = Request::MethodEnum::POST;
    request.uri_ = "/test/signup/";
    request.body_ = "bogus";
    Response response = request_handler.handle_request(request);
    bool fail_header = response.headers_["Content-Type"] == "text/html";    
    bool fail_content = response.body_ == content;
    bool fail_status = response.code_ == Response::status_code::ok;

    EXPECT_TRUE(fail_header && fail_status && fail_content);
}






