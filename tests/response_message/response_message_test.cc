#include "gtest/gtest.h"
//#include "/usr/src/projects/Foxy-4/src/response.cc"
#include <string>
#include "response.h"

class ResponseMessageTest : public ::testing::Test
{
protected:
  Response msg;
};

TEST_F(ResponseMessageTest, OkBuffer)
{
  std::string expected = "HTTP/1.1 200 OK\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::ok);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, ExpectedBuffer)
{
  std::string expected = "HTTP/1.0 201 Created\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::created);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, AcceptedBuffer)
{
  std::string expected = "HTTP/1.0 202 Accepted\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::accepted);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, NoContentBuffer)
{
  std::string expected = "HTTP/1.0 204 No Content\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::no_content);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, MultipleChoicesBuffer)
{
  std::string expected = "HTTP/1.0 300 Multiple Choices\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::multiple_choices);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, MovedPermanentlyBuffer)
{
  std::string expected = "HTTP/1.0 301 Moved Permanently\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::moved_permanently);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, MovedTemporarilyBuffer)
{
  std::string expected = "HTTP/1.0 302 Moved Temporarily\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::moved_temporarily);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, NotModifiedBuffer)
{
  std::string expected = "HTTP/1.0 304 Not Modified\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::not_modified);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, BadRequestBuffer)
{
  std::string expected = "HTTP/1.1 400 Bad Request\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::bad_request);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, UnauthorizedBuffer)
{
  std::string expected = "HTTP/1.0 401 Unauthorized\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::unauthorized);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, ForbiddenBuffer)
{
  std::string expected = "HTTP/1.0 403 Forbidden\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::forbidden);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, NotFoundBuffer)
{
  std::string expected = "HTTP/1.0 404 Not Found\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::not_found);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, InternalServerErrorBuffer)
{
  std::string expected = "HTTP/1.0 500 Internal Server Error\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::internal_server_error);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, NotImplementedBuffer)
{
  std::string expected = "HTTP/1.0 501 Not Implemented\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::not_implemented);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, BadGatewayBuffer)
{
  std::string expected = "HTTP/1.0 502 Bad Gateway\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::bad_gateway);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, ServiceUnavailableBuffer)
{
  std::string expected = "HTTP/1.0 503 Service Unavailable\r\n";
  boost::asio::const_buffer buffer = msg.status_string_accessor(Response::service_unavailable);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseMessageTest, OkStockResponse)
{
  Response ok = msg.stock_response(Response::ok);
  bool header_zero_name = ok.headers[0].name == "Content-Length";
  bool header_zero_val = ok.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = ok.headers[1].name == "Content-Type";
  bool header_one_val = ok.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, CreatedStockResponse)
{
  Response created = msg.stock_response(Response::created);
  bool header_zero_name = created.headers[0].name == "Content-Length";
  bool header_zero_val = created.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = created.headers[1].name == "Content-Type";
  bool header_one_val = created.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, AcceptedStockResponse)
{
  Response accepted = msg.stock_response(Response::accepted);
  bool header_zero_name = accepted.headers[0].name == "Content-Length";
  bool header_zero_val = accepted.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = accepted.headers[1].name == "Content-Type";
  bool header_one_val = accepted.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, NoContentStockResponse)
{
  Response no_content = msg.stock_response(Response::no_content);
  bool header_zero_name = no_content.headers[0].name == "Content-Length";
  bool header_zero_val = no_content.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = no_content.headers[1].name == "Content-Type";
  bool header_one_val = no_content.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, MultipleChoicesStockResponse)
{
  Response multiple_choices = msg.stock_response(Response::multiple_choices);
  bool header_zero_name = multiple_choices.headers[0].name == "Content-Length";
  bool header_zero_val = multiple_choices.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = multiple_choices.headers[1].name == "Content-Type";
  bool header_one_val = multiple_choices.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, MovedPermanentlyStockResponse)
{
  Response moved_permanently = msg.stock_response(Response::moved_permanently);
  bool header_zero_name = moved_permanently.headers[0].name == "Content-Length";
  bool header_zero_val = moved_permanently.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = moved_permanently.headers[1].name == "Content-Type";
  bool header_one_val = moved_permanently.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, MovedTemporarilyStockResponse)
{
  Response moved_temporarily = msg.stock_response(Response::moved_temporarily);
  bool header_zero_name = moved_temporarily.headers[0].name == "Content-Length";
  bool header_zero_val = moved_temporarily.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = moved_temporarily.headers[1].name == "Content-Type";
  bool header_one_val = moved_temporarily.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, NotModifiedStockResponse)
{
  Response not_modified = msg.stock_response(Response::not_modified);
  bool header_zero_name = not_modified.headers[0].name == "Content-Length";
  bool header_zero_val = not_modified.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = not_modified.headers[1].name == "Content-Type";
  bool header_one_val = not_modified.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, BadRequestStockResponse)
{
  Response bad_request = msg.stock_response(Response::bad_request);
  bool header_zero_name = bad_request.headers[0].name == "Content-Length";
  bool header_zero_val = bad_request.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = bad_request.headers[1].name == "Content-Type";
  bool header_one_val = bad_request.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, UnauthorizedStockResponse)
{
  Response unauthorized = msg.stock_response(Response::unauthorized);
  bool header_zero_name = unauthorized.headers[0].name == "Content-Length";
  bool header_zero_val = unauthorized.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = unauthorized.headers[1].name == "Content-Type";
  bool header_one_val = unauthorized.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, ForbiddenStockResponse)
{
  Response forbidden = msg.stock_response(Response::forbidden);
  bool header_zero_name = forbidden.headers[0].name == "Content-Length";
  bool header_zero_val = forbidden.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = forbidden.headers[1].name == "Content-Type";
  bool header_one_val = forbidden.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, NotFoundStockResponse)
{
  Response not_found = msg.stock_response(Response::not_found);
  bool header_zero_name = not_found.headers[0].name == "Content-Length";
  bool header_zero_val = not_found.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = not_found.headers[1].name == "Content-Type";
  bool header_one_val = not_found.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, ISEStockResponse)
{
  Response internal_server_error = msg.stock_response(Response::internal_server_error);
  bool header_zero_name = internal_server_error.headers[0].name == "Content-Length";
  bool header_zero_val = internal_server_error.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = internal_server_error.headers[1].name == "Content-Type";
  bool header_one_val = internal_server_error.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, NotImplementedStockResponse)
{
  Response not_implemented = msg.stock_response(Response::not_implemented);
  bool header_zero_name = not_implemented.headers[0].name == "Content-Length";
  bool header_zero_val = not_implemented.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = not_implemented.headers[1].name == "Content-Type";
  bool header_one_val = not_implemented.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, BadGatewayStockResponse)
{
  Response bad_gateway = msg.stock_response(Response::bad_gateway);
  bool header_zero_name = bad_gateway.headers[0].name == "Content-Length";
  bool header_zero_val = bad_gateway.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = bad_gateway.headers[1].name == "Content-Type";
  bool header_one_val = bad_gateway.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, ServiceUnavailableStockResponse)
{
  Response service_unavailable = msg.stock_response(Response::service_unavailable);
  bool header_zero_name = service_unavailable.headers[0].name == "Content-Length";
  bool header_zero_val = service_unavailable.headers[0].value == std::to_string(msg.content.size());
  bool header_one_name = service_unavailable.headers[1].name == "Content-Type";
  bool header_one_val = service_unavailable.headers[1].value == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseMessageTest, ToBuffersWithHeader)
{
  msg = Response::stock_response(Response::accepted);
  std::vector<boost::asio::const_buffer> return_buffers = msg.to_buffers();
  char sent_message[1024] = "";
  for (auto &buffer : return_buffers)
  {
    const char *data = boost::asio::buffer_cast<const char *>(buffer);
    strcat(sent_message, data);
  }

  const char expected_response[] = "HTTP/1.0 202 Accepted\r\n\
Content-Length: \r\n83\r\n\
Content-Type: \r\ntext/html\r\n\r\n\
<html><head><title>Accepted</title></head><body><h1>202 Accepted</h1></body></html>";

  EXPECT_EQ(std::strcmp(sent_message, expected_response), 0);
}

TEST_F(ResponseMessageTest, ToBuffersNoHeader)
{
  msg = Response::stock_response(Response::accepted);
  msg.headers.resize(0);
  std::vector<boost::asio::const_buffer> return_buffers = msg.to_buffers();
  
  char sent_message[1024] = "";
  for (auto &buffer : return_buffers)
  {
    const char *data = boost::asio::buffer_cast<const char *>(buffer);
    strcat(sent_message, data);
  }

  const char expected_response[] = "HTTP/1.0 202 Accepted\r\n\r\n\
<html><head><title>Accepted</title></head><body><h1>202 Accepted</h1></body></html>";

  EXPECT_EQ(std::strcmp(sent_message, expected_response), 0);
}
