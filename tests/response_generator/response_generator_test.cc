#include "gtest/gtest.h"
//#include "/usr/src/projects/Foxy-4/src/response.cc"
#include <string>
#include "response_generator.h"

class ResponseGeneratorTest : public ::testing::Test
{
protected:
  ResponseGenerator response_generator_;
};

TEST_F(ResponseGeneratorTest, OkBuffer)
{
  std::string expected = "HTTP/1.1 200 OK\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::ok);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, ExpectedBuffer)
{
  std::string expected = "HTTP/1.0 201 Created\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::created);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, AcceptedBuffer)
{
  std::string expected = "HTTP/1.0 202 Accepted\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::accepted);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, NoContentBuffer)
{
  std::string expected = "HTTP/1.0 204 No Content\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::no_content);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, MultipleChoicesBuffer)
{
  std::string expected = "HTTP/1.0 300 Multiple Choices\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::multiple_choices);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, MovedPermanentlyBuffer)
{
  std::string expected = "HTTP/1.0 301 Moved Permanently\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::moved_permanently);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, MovedTemporarilyBuffer)
{
  std::string expected = "HTTP/1.0 302 Moved Temporarily\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::moved_temporarily);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, NotModifiedBuffer)
{
  std::string expected = "HTTP/1.0 304 Not Modified\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::not_modified);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, BadRequestBuffer)
{
  std::string expected = "HTTP/1.1 400 Bad Request\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::bad_request);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, UnauthorizedBuffer)
{
  std::string expected = "HTTP/1.0 401 Unauthorized\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::unauthorized);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, ForbiddenBuffer)
{
  std::string expected = "HTTP/1.0 403 Forbidden\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::forbidden);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, NotFoundBuffer)
{
  std::string expected = "HTTP/1.0 404 Not Found\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::not_found);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, InternalServerErrorBuffer)
{
  std::string expected = "HTTP/1.0 500 Internal Server Error\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::internal_server_error);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, NotImplementedBuffer)
{
  std::string expected = "HTTP/1.0 501 Not Implemented\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::not_implemented);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, BadGatewayBuffer)
{
  std::string expected = "HTTP/1.0 502 Bad Gateway\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::bad_gateway);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, ServiceUnavailableBuffer)
{
  std::string expected = "HTTP/1.0 503 Service Unavailable\r\n";
  boost::asio::const_buffer buffer = response_generator_.status_string_accessor(Response::service_unavailable);
  const char *p = boost::asio::buffer_cast<const char *>(buffer);
  std::string buffer_data = std::string(p);
  EXPECT_TRUE(expected.compare(buffer_data) == 0);
}

TEST_F(ResponseGeneratorTest, OkStockResponse)
{
  Response ok = response_generator_.stock_response(Response::ok);
  bool header_zero_name = ok.headers_.find("Content-Length") == ok.headers_.end() ? false : true;
  bool header_zero_val = ok.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = ok.headers_.find("Content-Type") == ok.headers_.end() ? false : true;
  bool header_one_val = ok.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, CreatedStockResponse)
{
  Response created = response_generator_.stock_response(Response::created);
  bool header_zero_name = created.headers_.find("Content-Length") == created.headers_.end() ? false : true;
  bool header_zero_val = created.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = created.headers_.find("Content-Type") == created.headers_.end() ? false : true;
  bool header_one_val = created.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, AcceptedStockResponse)
{
  Response accepted = response_generator_.stock_response(Response::accepted);
  bool header_zero_name = accepted.headers_.find("Content-Length") == accepted.headers_.end() ? false : true;
  bool header_zero_val = accepted.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = accepted.headers_.find("Content-Type") == accepted.headers_.end() ? false : true;
  bool header_one_val = accepted.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, NoContentStockResponse)
{
  Response no_content = response_generator_.stock_response(Response::no_content);
  bool header_zero_name = no_content.headers_.find("Content-Length") == no_content.headers_.end() ? false : true;
  bool header_zero_val = no_content.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = no_content.headers_.find("Content-Type") == no_content.headers_.end() ? false : true;
  bool header_one_val = no_content.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, MultipleChoicesStockResponse)
{
  Response multiple_choices = response_generator_.stock_response(Response::multiple_choices);
  bool header_zero_name = multiple_choices.headers_.find("Content-Length") == multiple_choices.headers_.end() ? false : true;
  bool header_zero_val = multiple_choices.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = multiple_choices.headers_.find("Content-Type") == multiple_choices.headers_.end() ? false : true;
  bool header_one_val = multiple_choices.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, MovedPermanentlyStockResponse)
{
  Response moved_permanently = response_generator_.stock_response(Response::moved_permanently);
  bool header_zero_name = moved_permanently.headers_.find("Content-Length") == moved_permanently.headers_.end() ? false : true;
  bool header_zero_val = moved_permanently.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = moved_permanently.headers_.find("Content-Type") == moved_permanently.headers_.end() ? false : true;
  bool header_one_val = moved_permanently.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, MovedTemporarilyStockResponse)
{
  Response moved_temporarily = response_generator_.stock_response(Response::moved_temporarily);
  bool header_zero_name = moved_temporarily.headers_.find("Content-Length") == moved_temporarily.headers_.end() ? false : true;
  bool header_zero_val = moved_temporarily.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = moved_temporarily.headers_.find("Content-Type") == moved_temporarily.headers_.end() ? false : true;
  bool header_one_val = moved_temporarily.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, NotModifiedStockResponse)
{
  Response not_modified = response_generator_.stock_response(Response::not_modified);
  bool header_zero_name = not_modified.headers_.find("Content-Length") == not_modified.headers_.end() ? false : true;
  bool header_zero_val = not_modified.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = not_modified.headers_.find("Content-Type") == not_modified.headers_.end() ? false : true;
  bool header_one_val = not_modified.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, BadRequestStockResponse)
{
  Response bad_request = response_generator_.stock_response(Response::bad_request);
  bool header_zero_name = bad_request.headers_.find("Content-Length") == bad_request.headers_.end() ? false : true;
  bool header_zero_val = bad_request.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = bad_request.headers_.find("Content-Type") == bad_request.headers_.end() ? false : true;
  bool header_one_val = bad_request.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, UnauthorizedStockResponse)
{
  Response unauthorized = response_generator_.stock_response(Response::unauthorized);
  bool header_zero_name = unauthorized.headers_.find("Content-Length") == unauthorized.headers_.end() ? false : true;
  bool header_zero_val = unauthorized.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = unauthorized.headers_.find("Content-Type") == unauthorized.headers_.end() ? false : true;
  bool header_one_val = unauthorized.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, ForbiddenStockResponse)
{
  Response forbidden = response_generator_.stock_response(Response::forbidden);
  bool header_zero_name = forbidden.headers_.find("Content-Length") == forbidden.headers_.end() ? false : true;
  bool header_zero_val = forbidden.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = forbidden.headers_.find("Content-Type") == forbidden.headers_.end() ? false : true;
  bool header_one_val = forbidden.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, NotFoundStockResponse)
{
  Response not_found = response_generator_.stock_response(Response::not_found);
  bool header_zero_name = not_found.headers_.find("Content-Length") == not_found.headers_.end() ? false : true;
  bool header_zero_val = not_found.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = not_found.headers_.find("Content-Type") == not_found.headers_.end() ? false : true;
  bool header_one_val = not_found.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, ISEStockResponse)
{
  Response internal_server_error = response_generator_.stock_response(Response::internal_server_error);
  bool header_zero_name = internal_server_error.headers_.find("Content-Length") == internal_server_error.headers_.end() ? false : true;
  bool header_zero_val = internal_server_error.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = internal_server_error.headers_.find("Content-Type") == internal_server_error.headers_.end() ? false : true;
  bool header_one_val = internal_server_error.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, NotImplementedStockResponse)
{
  Response not_implemented = response_generator_.stock_response(Response::not_implemented);
  bool header_zero_name = not_implemented.headers_.find("Content-Length") == not_implemented.headers_.end() ? false : true;
  bool header_zero_val = not_implemented.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = not_implemented.headers_.find("Content-Type") == not_implemented.headers_.end() ? false : true;
  bool header_one_val = not_implemented.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, BadGatewayStockResponse)
{
  Response bad_gateway = response_generator_.stock_response(Response::bad_gateway);
  bool header_zero_name = bad_gateway.headers_.find("Content-Length") == bad_gateway.headers_.end() ? false : true;
  bool header_zero_val = bad_gateway.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = bad_gateway.headers_.find("Content-Type") == bad_gateway.headers_.end() ? false : true;
  bool header_one_val = bad_gateway.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, ServiceUnavailableStockResponse)
{
  Response service_unavailable = response_generator_.stock_response(Response::service_unavailable);
  bool header_zero_name = service_unavailable.headers_.find("Content-Length") == service_unavailable.headers_.end() ? false : true;
  bool header_zero_val = service_unavailable.headers_["Content-Length"] == std::to_string(response_generator_.body_.size());
  bool header_one_name = service_unavailable.headers_.find("Content-Type") == service_unavailable.headers_.end() ? false : true;
  bool header_one_val = service_unavailable.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(header_zero_name && header_one_name && header_one_val);
}

TEST_F(ResponseGeneratorTest, ToBuffersWithHeader)
{
  Response response = ResponseGenerator::stock_response(Response::accepted);
  response_generator_.init(response);
  std::vector<boost::asio::const_buffer> return_buffers = response_generator_.to_buffers();
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

TEST_F(ResponseGeneratorTest, ToBuffersNoHeader)
{
  Response response = ResponseGenerator::stock_response(Response::accepted);
  response.headers_.clear();
  response_generator_.init(response);
  std::vector<boost::asio::const_buffer> return_buffers = response_generator_.to_buffers();
  
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
