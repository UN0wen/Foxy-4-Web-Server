#include "gtest/gtest.h"
#include "config_parser.h"


class GetPortTest : public ::testing::Test {
protected:
   NginxConfigParser parser;
   NginxConfig out_config;
   int port;
};

TEST_F(GetPortTest, SimpleConfig) {
  parser.Parse("simple_config", &out_config);
  bool success = out_config.GetPort(&port);
  bool success_value = false;
  if(success) {
   success_value = (port == 8000);
  }
  EXPECT_TRUE(success && success_value);
} //basic test to see if port can be read in a simple config file

TEST_F(GetPortTest, BlockConfig) {
  parser.Parse("block", &out_config);
  bool success = out_config.GetPort(&port);
  bool success_value = false;
  if(success) {
   success_value = (port == 8000);
  }
  EXPECT_TRUE(success && success_value);
} //test to see if port can be read in a server block

TEST_F(GetPortTest, BasicConfig) {
  parser.Parse("example_config", &out_config);
  bool success = out_config.GetPort(&port);
  bool success_value = false;
  if(success) {
   success_value = (port == 8000);
  }
  EXPECT_TRUE(success && success_value);
} //test to see if port can be read from a slightly more complex config
  //based on the example_config provided in Assignment 1

TEST_F(GetPortTest, NoPortConfig) {
  parser.Parse("no_port", &out_config);
  bool success = out_config.GetPort(&port);
  EXPECT_FALSE(success);
} //GetPort should return false if no port is found (no listen keyword)

TEST_F(GetPortTest, NoPortNumConfig) {
  parser.Parse("no_port_num", &out_config);
  bool success = out_config.GetPort(&port);
  EXPECT_FALSE(success);
} //GetPort should return false if no port number is found after the listen keyword

TEST_F(GetPortTest, InvalidPortConfig) {
  parser.Parse("invalid_port", &out_config);
  bool success = out_config.GetPort(&port);
  EXPECT_FALSE(success);
} //GetPort should return false if the token after the listen keyword is not numeric

TEST_F(GetPortTest, InvalidPortStatementBeforeConfig) {
  parser.Parse("invalid_port_statement_before", &out_config);
  bool success = out_config.GetPort(&port);
  EXPECT_FALSE(success);
} //GetPort should return false if there are any tokens before the listen
  //keyword in the same statement

TEST_F(GetPortTest, InvalidPortStatementAfterConfig) {
  parser.Parse("invalid_port_statement_after", &out_config);
  bool success = out_config.GetPort(&port);
  EXPECT_FALSE(success);
} //GetPort should return false if there are any tokens after the port number
  //token in the same statement

TEST_F(GetPortTest, InvalidPortStatementBetweenConfig) {
  parser.Parse("invalid_port_statement_between", &out_config);
  bool success = out_config.GetPort(&port);
  EXPECT_FALSE(success);
} //GetPort should return false if there are any tokens in between the listen
  //keyword and the port number