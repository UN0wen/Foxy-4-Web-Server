#include "gtest/gtest.h"
#include <fstream>
#include "status_request_handler.h"

class StatusRequestHandlerTest : public ::testing::Test
{
protected:
  StatusRequestHandler* srh = new StatusRequestHandler();  
  Request req;
};

TEST_F(StatusRequestHandlerTest, HandleRequestTest)
{
  Response resp = srh->handle_request(req);
  bool code = resp.code_ == Response::ok;
  bool body_len = resp.headers_["Content-Length"] == std::to_string(resp.body_.size());
  bool content = resp.headers_["Content-Type"] == "text/html";
  EXPECT_TRUE(code && body_len && content);
}
