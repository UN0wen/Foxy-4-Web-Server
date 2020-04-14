#include "gtest/gtest.h"
#include "request_handler.h"

class RequestHandlerTest : public ::testing::Test {
protected:
	RequestHandler rh;
};

TEST_F(RequestHandlerTest, InvalidHttpParseTest) {
    char msg[] = "hello\n";
    RequestParser::result_type result = rh.http_format_precheck(msg, 6);
    EXPECT_TRUE(result == RequestParser::bad);
}

TEST_F(RequestHandlerTest, SendingGoodStatus) {
    char msg[] = "hello\n";
    Reply rep = rh.process_request(true, msg);
    bool success_header_name = rep.headers[1].name == "content-type";
    bool success_content = rep.content == "hello\n";
    bool success_header_value = rep.headers[1].value == "text/plain";
    bool success_status = rep.status == Reply::ok;
    
    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}

TEST_F(RequestHandlerTest, SendingBadStatus) {
    char msg[] = "hello\n";
    Reply rep = rh.process_request(false, msg);
    bool bad_header_name = rep.headers[1].name == "content-type";
    bool bad_content = rep.content == "hello\n";
    bool bad_header_value = rep.headers[1].value == "text/plain";
    bool bad_status = rep.status == Reply::bad_request;

    EXPECT_TRUE(bad_header_name && bad_content && bad_header_value && bad_status);
}

TEST_F(RequestHandlerTest, SendingBadStatusAgain) {
    char msg[] = "sendingBadStatusAgain\n";
    Reply rep = rh.process_request(false, msg);
    bool bad_header_name = rep.headers[1].name == "content-type";
    bool bad_content = rep.content == "sendingBadStatusAgain\n";
    bool bad_header_value = rep.headers[1].value == "text/plain";
    bool bad_status = rep.status == Reply::bad_request;

    EXPECT_TRUE(bad_header_name && bad_content && bad_header_value && bad_status);
}

TEST_F(RequestHandlerTest, SendingGoodStatusAgain) {
    char msg[] = "sendingGoodStatusAgain\n";
    Reply rep = rh.process_request(true, msg);
    bool success_header_name = rep.headers[1].name == "content-type";
    bool success_content = rep.content == "sendingGoodStatusAgain\n";
    bool success_header_value = rep.headers[1].value == "text/plain";
    bool success_status = rep.status == Reply::ok;
    
    EXPECT_TRUE(success_header_name && success_content && success_header_value && success_status);
}
