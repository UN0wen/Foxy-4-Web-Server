#include "gtest/gtest.h"
#include <fstream>
#include "request_handler_generator.h"
#include "config_parser.h"
#include "static_request_handler.h"
#include "echo_request_handler.h"
#include <map>

class RequestHandlerGeneratorTest : public ::testing::Test
{
protected:
    const char *path = "./test_config/example_config";
    NginxConfigParser config_parser;
    NginxConfig config;
    bool parse = config_parser.Parse(path, &config);
};

TEST_F(RequestHandlerGeneratorTest, TestGetStaticRequestHandler)
{
    RequestHandlerGenerator request_handler_generator;
    request_handler_generator.get_map(&config);
    RequestHandler *rh = request_handler_generator.dispatch_handler("/static/fake.html").get();
    EXPECT_TRUE(rh == request_handler_generator.mapping().at("/static/").get());
}

TEST_F(RequestHandlerGeneratorTest, TestGetImageRequestHandler)
{
    RequestHandlerGenerator request_handler_generator;
    request_handler_generator.get_map(&config);
    RequestHandler *rh = request_handler_generator.dispatch_handler("/images/fake.jpg").get();
    EXPECT_TRUE(rh == request_handler_generator.mapping().at("/images/").get());
}

TEST_F(RequestHandlerGeneratorTest, TestGetImageEchoHandler)
{
    RequestHandlerGenerator request_handler_generator;
    request_handler_generator.get_map(&config);
    RequestHandler *rh = request_handler_generator.dispatch_handler("/echo/").get();
    EXPECT_TRUE(rh == request_handler_generator.mapping().at("/echo/").get());
}
