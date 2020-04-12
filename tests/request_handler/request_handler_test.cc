#include "gtest/gtest.h"
#include "request_handler.h"

class RequestHandlerTest : public ::testing::Test {
protected:
	request_handler rh;
};

//TODO(Nelson): please write 2 more unit tests on parser. Follow my format
TEST_F(RequestHandlerTest, InvalidHttpParseTest) {
    char msg[] = "hello\n";
    http::server::request_parser::result_type result = rh.http_format_precheck(msg, 6);
    EXPECT_TRUE(result == http::server::request_parser::bad);
}

TEST_F(RequestHandlerTest, SendingGoodStatus) {
    char msg[] = "hello\n";
    http::server::reply rep = rh.process_request(true, msg);
    bool success_header_name = rep.headers[1].name == "content-type";
    bool success_content = rep.content == "hello\n";
    bool success_header_value = rep.headers[1].value == "text/plain";
    bool success_status = rep.status == http::server::reply::ok;
    
    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}

TEST_F(RequestHandlerTest, SendingBadStatus) {
    char msg[] = "hello\n";
    http::server::reply rep = rh.process_request(false, msg);
    bool bad_header_name = rep.headers[1].name == "content-type";
    bool bad_content = rep.content == "hello\n";
    bool bad_header_value = rep.headers[1].value == "text/plain";
    bool bad_status = rep.status == http::server::reply::bad_request;

    EXPECT_TRUE(bad_header_name && bad_content && bad_header_value && bad_status);
}