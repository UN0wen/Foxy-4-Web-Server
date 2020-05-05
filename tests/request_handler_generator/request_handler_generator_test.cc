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
    std::map<std::string, std::shared_ptr<RequestHandler>> get_map()
    {
        const char *path = "./test_config/example_config";
        NginxConfigParser config_parser;
        NginxConfig config;
        std::map<std::string, std::shared_ptr<RequestHandler>> mapping;
        config_parser.Parse(path, &config);
        config.get_map(&mapping);
        return mapping;
    }

    std::map<std::string, std::shared_ptr<RequestHandler>> mapping;
    RequestHandlerGeneratorTest()
        : mapping(get_map())
    {
    }
};

TEST_F(RequestHandlerGeneratorTest, TestGetStaticRequestHandler)
{
    RequestHandlerGenerator request_handler_generator = RequestHandlerGenerator(mapping);
    RequestHandler *rh = request_handler_generator.dispatch_handler("/static/fake.html").get();
    EXPECT_TRUE(rh == mapping.at("/static/").get());
}

TEST_F(RequestHandlerGeneratorTest, TestGetImageRequestHandler)
{
    RequestHandlerGenerator request_handler_generator = RequestHandlerGenerator(mapping);
    RequestHandler *rh = request_handler_generator.dispatch_handler("/images/fake.jpg").get();
    EXPECT_TRUE(rh == mapping.at("/images/").get());
}

TEST_F(RequestHandlerGeneratorTest, TestGetImageEchoHandler)
{
    RequestHandlerGenerator request_handler_generator = RequestHandlerGenerator(mapping);
    RequestHandler *rh = request_handler_generator.dispatch_handler("/echo/").get();
    EXPECT_TRUE(rh == mapping.at("/echo/").get());
}
