#include "gtest/gtest.h"
#include "config_parser.h"
#include "request_handler.h"


class ConfigTest : public ::testing::Test {
protected:
   NginxConfigParser parser;
   NginxConfig out_config;
   int port;
   std::map<std::string, std::shared_ptr<RequestHandler>> mapping;
};

//Tests for get_port function

TEST_F(ConfigTest, SimpleConfig) {
  parser.Parse("simple_config", &out_config);
  bool success = out_config.get_port(&port);
  bool success_value = false;
  if(success) {
   success_value = (port == 8000);
  }
  EXPECT_TRUE(success && success_value);
} //basic test to see if port can be read in a simple config file

TEST_F(ConfigTest, BlockConfig) {
  parser.Parse("block", &out_config);
  bool success = out_config.get_port(&port);
  bool success_value = false;
  if(success) {
   success_value = (port == 8000);
  }
  EXPECT_TRUE(success && success_value);
} //test to see if port can be read in a server block

TEST_F(ConfigTest, BasicConfig) {
  parser.Parse("example_config", &out_config);
  bool success = out_config.get_port(&port);
  bool success_value = false;
  if(success) {
   success_value = (port == 8000);
  }
  EXPECT_TRUE(success && success_value);
} //test to see if port can be read from a slightly more complex config
  //based on the example_config provided in Assignment 1

TEST_F(ConfigTest, NoPortConfig) {
  parser.Parse("no_port", &out_config);
  bool success = out_config.get_port(&port);
  EXPECT_FALSE(success);
} //get_port should return false if no port is found (no listen keyword)

TEST_F(ConfigTest, NoPortNumConfig) {
  parser.Parse("no_port_num", &out_config);
  bool success = out_config.get_port(&port);
  EXPECT_FALSE(success);
} //get_port should return false if no port number is found after the listen keyword

TEST_F(ConfigTest, InvalidPortConfig) {
  parser.Parse("invalid_port", &out_config);
  bool success = out_config.get_port(&port);
  EXPECT_FALSE(success);
} //get_port should return false if the token after the listen keyword is not numeric

TEST_F(ConfigTest, InvalidPortStatementBeforeConfig) {
  parser.Parse("invalid_port_statement_before", &out_config);
  bool success = out_config.get_port(&port);
  EXPECT_FALSE(success);
} //get_port should return false if there are any tokens before the listen
  //keyword in the same statement

TEST_F(ConfigTest, InvalidPortStatementAfterConfig) {
  parser.Parse("invalid_port_statement_after", &out_config);
  bool success = out_config.get_port(&port);
  EXPECT_FALSE(success);
} //get_port should return false if there are any tokens after the port number
  //token in the same statement

TEST_F(ConfigTest, InvalidPortStatementBetweenConfig) {
  parser.Parse("invalid_port_statement_between", &out_config);
  bool success = out_config.get_port(&port);
  EXPECT_FALSE(success);
} //get_port should return false if there are any tokens in between the listen
  //keyword and the port number

//ToString function sanity checks

TEST_F(ConfigTest, ToStringSanityCheck) {
  parser.Parse("ToString", &out_config);
  bool success = (out_config.ToString() == "This is a string.");
  EXPECT_FALSE(success);
} //just tests to see if ToString is functional

//ToString in NginxConfigStatement

class NginxConfigStatementTest : public ::testing::Test {
protected:
   NginxConfigStatement Statement;
};

TEST_F(NginxConfigStatementTest, EmptyStatement) {
   bool success = (Statement.ToString(0) == "\n");
   EXPECT_TRUE(success);
}

TEST_F(NginxConfigStatementTest, DepthEqualsOne) {
   bool success = (Statement.ToString(1) == "\t\n");
   EXPECT_TRUE(success);
}

TEST_F(NginxConfigStatementTest, EmptyChild) {
   Statement.child_block_.reset(new NginxConfig);
   Statement.tokens_.push_back("Test");
   bool success = (Statement.ToString(0) == "Test {\n}\n");
   EXPECT_TRUE(success);
}

TEST_F(NginxConfigStatementTest, EmptyChildDepthOne) {
   Statement.child_block_.reset(new NginxConfig);
   Statement.tokens_.push_back("Test");
   bool success = (Statement.ToString(1) == "\tTest {\n\t}\n");
   EXPECT_TRUE(success);
}