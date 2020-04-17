#include "gtest/gtest.h"
//#include "/usr/src/projects/Foxy-4/src/reply.cc"
#include <string>
#include "reply.h"

class ReplyMessageTest : public ::testing::Test
{
protected:
  Reply msg;
};

TEST_F(ReplyMessageTest, OkBuffer)
{
  std::string expected = "HTTP/1.1 200 OK\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::ok);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, ExpectedBuffer)
{
  std::string expected = "HTTP/1.0 201 Created\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::created);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, AcceptedBuffer)
{
  std::string expected = "HTTP/1.0 202 Accepted\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::accepted);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, NoContentBuffer)
{
  std::string expected = "HTTP/1.0 204 No Content\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::no_content);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, MultipleChoicesBuffer)
{
  std::string expected = "HTTP/1.0 300 Multiple Choices\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::multiple_choices);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, MovedPermanentlyBuffer)
{
  std::string expected = "HTTP/1.0 301 Moved Permanently\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::moved_permanently);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, MovedTemporarilyBuffer)
{
  std::string expected = "HTTP/1.0 302 Moved Temporarily\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::moved_temporarily);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, NotModifiedBuffer)
{
  std::string expected = "HTTP/1.0 304 Not Modified\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::not_modified);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, BadRequestBuffer)
{
  std::string expected = "HTTP/1.1 400 Bad Request\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::bad_request);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, UnauthorizedBuffer)
{
  std::string expected = "HTTP/1.0 401 Unauthorized\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::unauthorized);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, ForbiddenBuffer)
{
  std::string expected = "HTTP/1.0 403 Forbidden\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::forbidden);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, NotFoundBuffer)
{
  std::string expected = "HTTP/1.0 404 Not Found\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::not_found);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, InternalServerErrorBuffer)
{
  std::string expected = "HTTP/1.0 500 Internal Server Error\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::internal_server_error);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, NotImplementedBuffer)
{
  std::string expected = "HTTP/1.0 501 Not Implemented\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::not_implemented);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, BadGatewayBuffer)
{
  std::string expected = "HTTP/1.0 502 Bad Gateway\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::bad_gateway);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ReplyMessageTest, ServiceUnavailableBuffer)
{
  std::string expected = "HTTP/1.0 503 Service Unavailable\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Reply::service_unavailable);
  const char* p = boost::asio::buffer_cast<const char*>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}


TEST_F(ReplyMessageTest, OkStockReply)
{
  Reply ok = msg.stock_reply(Reply::ok);
  bool header_zero_name = ok.headers[0].name == "Content-Length";
  bool header_zero_val = ok.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = ok.headers[1].name == "Content-Type";
  bool header_one_val = ok.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, CreatedStockReply)
{
  Reply created = msg.stock_reply(Reply::created);
  bool header_zero_name = created.headers[0].name == "Content-Length";
  bool header_zero_val = created.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = created.headers[1].name == "Content-Type";
  bool header_one_val = created.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, AcceptedStockReply)
{
  Reply accepted = msg.stock_reply(Reply::accepted);
  bool header_zero_name = accepted.headers[0].name == "Content-Length";
  bool header_zero_val = accepted.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = accepted.headers[1].name == "Content-Type";
  bool header_one_val = accepted.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, NoContentStockReply)
{
  Reply no_content = msg.stock_reply(Reply::no_content);
  bool header_zero_name = no_content.headers[0].name == "Content-Length";
  bool header_zero_val = no_content.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = no_content.headers[1].name == "Content-Type";
  bool header_one_val = no_content.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, MultipleChoicesStockReply)
{
  Reply multiple_choices = msg.stock_reply(Reply::multiple_choices);
  bool header_zero_name = multiple_choices.headers[0].name == "Content-Length";
  bool header_zero_val = multiple_choices.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = multiple_choices.headers[1].name == "Content-Type";
  bool header_one_val = multiple_choices.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, MovedPermanentlyStockReply)
{
  Reply moved_permanently = msg.stock_reply(Reply::moved_permanently);
  bool header_zero_name = moved_permanently.headers[0].name == "Content-Length";
  bool header_zero_val = moved_permanently.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = moved_permanently.headers[1].name == "Content-Type";
  bool header_one_val = moved_permanently.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, MovedTemporarilyStockReply)
{
  Reply moved_temporarily = msg.stock_reply(Reply::moved_temporarily);
  bool header_zero_name = moved_temporarily.headers[0].name == "Content-Length";
  bool header_zero_val = moved_temporarily.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = moved_temporarily.headers[1].name == "Content-Type";
  bool header_one_val = moved_temporarily.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, NotModifiedStockReply)
{
  Reply not_modified = msg.stock_reply(Reply::not_modified);
  bool header_zero_name = not_modified.headers[0].name == "Content-Length";
  bool header_zero_val = not_modified.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = not_modified.headers[1].name == "Content-Type";
  bool header_one_val = not_modified.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, BadRequestStockReply)
{
  Reply bad_request = msg.stock_reply(Reply::bad_request);
  bool header_zero_name = bad_request.headers[0].name == "Content-Length";
  bool header_zero_val = bad_request.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = bad_request.headers[1].name == "Content-Type";
  bool header_one_val = bad_request.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, UnauthorizedStockReply)
{
  Reply unauthorized = msg.stock_reply(Reply::unauthorized);
  bool header_zero_name = unauthorized.headers[0].name == "Content-Length";
  bool header_zero_val = unauthorized.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = unauthorized.headers[1].name == "Content-Type";
  bool header_one_val = unauthorized.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, ForbiddenStockReply)
{
  Reply forbidden = msg.stock_reply(Reply::forbidden);
  bool header_zero_name = forbidden.headers[0].name == "Content-Length";
  bool header_zero_val = forbidden.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = forbidden.headers[1].name == "Content-Type";
  bool header_one_val = forbidden.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, NotFoundStockReply)
{
  Reply not_found = msg.stock_reply(Reply::not_found);  
  bool header_zero_name = not_found.headers[0].name == "Content-Length";
  bool header_zero_val = not_found.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = not_found.headers[1].name == "Content-Type";
  bool header_one_val = not_found.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, ISEStockReply)
{
  Reply internal_server_error = msg.stock_reply(Reply::internal_server_error);
  bool header_zero_name = internal_server_error.headers[0].name == "Content-Length";
  bool header_zero_val = internal_server_error.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = internal_server_error.headers[1].name == "Content-Type";
  bool header_one_val = internal_server_error.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, NotImplementedStockReply)
{
  Reply not_implemented = msg.stock_reply(Reply::not_implemented);
  bool header_zero_name = not_implemented.headers[0].name == "Content-Length";
  bool header_zero_val = not_implemented.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = not_implemented.headers[1].name == "Content-Type";
  bool header_one_val = not_implemented.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, BadGatewayStockReply)
{
  Reply bad_gateway = msg.stock_reply(Reply::bad_gateway);
  bool header_zero_name = bad_gateway.headers[0].name == "Content-Length";
  bool header_zero_val = bad_gateway.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = bad_gateway.headers[1].name == "Content-Type";
  bool header_one_val = bad_gateway.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ReplyMessageTest, ServiceUnavailableStockReply)
{  
  Reply service_unavailable = msg.stock_reply(Reply::service_unavailable);
  bool header_zero_name = service_unavailable.headers[0].name == "Content-Length";
  bool header_zero_val = service_unavailable.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = service_unavailable.headers[1].name == "Content-Type";
  bool header_one_val = service_unavailable.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

