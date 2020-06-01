#include "gtest/gtest.h"
#include <fstream>
#include "data_collector.h"

class DataCollectorTest : public ::testing::Test
{
protected:
  DataCollector* test = DataCollector::get_instance();
};

TEST_F(DataCollectorTest, CreatedStatusCode)
{
  std::string code = test->map_status_to_string(Response::created);
  EXPECT_TRUE(code == "201");
}

TEST_F(DataCollectorTest, AcceptedStatusCode)
{
  std::string code = test->map_status_to_string(Response::accepted);
  EXPECT_TRUE(code == "202");
}

TEST_F(DataCollectorTest, NoContentStatusCode)
{
  std::string code = test->map_status_to_string(Response::no_content);
  EXPECT_TRUE(code == "204");
} 

TEST_F(DataCollectorTest, MultipleChoicesStatusCode)
{
  std::string code = test->map_status_to_string(Response::multiple_choices);
  EXPECT_TRUE(code == "300");
}

TEST_F(DataCollectorTest, MovedPermanentlyStatusCode)
{
  std::string code = test->map_status_to_string(Response::moved_permanently);
  EXPECT_TRUE(code == "301");
}

TEST_F(DataCollectorTest, NotModifiedStatusCode)
{
  std::string code = test->map_status_to_string(Response::not_modified);
  EXPECT_TRUE(code == "304");
}

TEST_F(DataCollectorTest, BadRequestStatusCode)
{
  std::string code = test->map_status_to_string(Response::bad_request);
  EXPECT_TRUE(code == "400");
}

TEST_F(DataCollectorTest, UnauthorizedStatusCode)
{
  std::string code = test->map_status_to_string(Response::unauthorized);
  EXPECT_TRUE(code == "401");
}

TEST_F(DataCollectorTest, ForbiddenStatusCode)
{
  std::string code = test->map_status_to_string(Response::forbidden);
  EXPECT_TRUE(code == "403");
}

TEST_F(DataCollectorTest, NotFoundStatusCode)
{
  std::string code = test->map_status_to_string(Response::not_found);
  EXPECT_TRUE(code == "404");
}

TEST_F(DataCollectorTest, ISEStatusCode)
{
  std::string code = test->map_status_to_string(Response::internal_server_error);
  EXPECT_TRUE(code == "500");
}

TEST_F(DataCollectorTest, NotImplementedStatusCode)
{
  std::string code = test->map_status_to_string(Response::not_implemented);
  EXPECT_TRUE(code == "501");
}

TEST_F(DataCollectorTest, BadGatewayStatusCode)
{
  std::string code = test->map_status_to_string(Response::bad_gateway);
  EXPECT_TRUE(code == "502");
}

TEST_F(DataCollectorTest, ServiceUnavailableStatusCode)
{
  std::string code = test->map_status_to_string(Response::service_unavailable);
  EXPECT_TRUE(code == "503");
}


