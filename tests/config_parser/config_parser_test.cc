#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public ::testing::Test {
protected:
	NginxConfigParser parser;
	NginxConfig out_config;
};

//example test case
TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

//my test cases

TEST_F(NginxConfigParserTest, Nested) {
   bool success = parser.Parse("nested", &out_config);
   EXPECT_TRUE(success);
}//tests to see if nested statements work


TEST_F(NginxConfigParserTest, Emptynested) {
   bool success = parser.Parse("nestedempty", &out_config);
   EXPECT_TRUE(success);
}//tests to see if an empty statement works

TEST_F(NginxConfigParserTest, InvalidBracket) {
   bool success = parser.Parse("iBracket", &out_config);
   EXPECT_FALSE(success);
}//tests to see if unclosed bracket is caught

TEST_F(NginxConfigParserTest, InvalidSemicolon) {
   bool success = parser.Parse("iSemicolon", &out_config);
   EXPECT_FALSE(success);
}//tests to see if no semicolon is caught

TEST_F(NginxConfigParserTest, Whitespace) {
   bool success = parser.Parse("whitespace", &out_config);
   EXPECT_TRUE(success);
}//tests to see if whitespace changes anything; it shouldn't

TEST_F(NginxConfigParserTest, Empty) {
   bool success = parser.Parse("empty", &out_config);
   EXPECT_TRUE(success);
}//Empty files should be valid according to the grammar defined in the slides

TEST_F(NginxConfigParserTest, Emptyblock) {
   bool success = parser.Parse("emptyblock", &out_config);
   EXPECT_TRUE(success);
}//tests to see if an empty statement works within brackets

TEST_F(NginxConfigParserTest, Extrabracketstart) {
   bool success = parser.Parse("extrabracket_end", &out_config);
   EXPECT_FALSE(success);
}//tests to see if an extra opening bracket at the start is caught


TEST_F(NginxConfigParserTest, Extrabracketend) {
   bool success = parser.Parse("extrabracket_start", &out_config);
   EXPECT_FALSE(success);
}//tests to see if an extra closing bracket at the end is caught

TEST_F(NginxConfigParserTest, Emptybracketend) {
   bool success = parser.Parse("emptybracket_end", &out_config);
   EXPECT_FALSE(success);
}//tests to see if an extra closing bracket at the end is caught
 //basically the same as above, but for more branch coverage

TEST_F(NginxConfigParserTest, Singlequote) {
   bool success = parser.Parse("singlequote", &out_config);
   EXPECT_TRUE(success);
}//tests to see if singlequote escape is valid

TEST_F(NginxConfigParserTest, Comment) {
   bool success = parser.Parse("comment", &out_config);
   EXPECT_TRUE(success);
}//tests to see if comments are valid

TEST_F(NginxConfigParserTest, Unclosedquote) {
   bool success = parser.Parse("quoteerror", &out_config);
   EXPECT_FALSE(success);
}//tests to see if unclosed singlequote is caught

TEST_F(NginxConfigParserTest, UnclosedDoublequote) {
   bool success = parser.Parse("doublequoteerror", &out_config);
   EXPECT_FALSE(success);
}//tests to see if unclosed doublequote is caught

TEST_F(NginxConfigParserTest, DoubleSemicolon) {
   bool success = parser.Parse("doublesemi", &out_config);
   EXPECT_FALSE(success);
}//tests to see if double semicolon is caught