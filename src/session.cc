//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <boost/move/utility_core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>

#include "session.h"
#include "reply.h"
#include "request.h"
#include "static_request_handler.h"
#include "echo_request_handler.h"
#define BOOST_LOG_DYN_LINK 1
BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());

Session::Session(boost::asio::io_service &io_service, std::map<std::string, std::string> path_to_root, std::map<std::string, std::string> path_to_root_echo)
    : socket_(io_service),
      path_to_root(path_to_root),
      path_to_root_echo(path_to_root_echo)
{
}

tcp::socket &Session::socket()
{
  return socket_;
}

int Session::common_prefix_length(std::string a, std::string b){
  char s1[a.size()+1];
  char s2[b.size()+1];
  strcpy(s1, a.c_str());
  strcpy(s2, a.c_str());
  int counter = 0;
  for(int i = 0; i < a.size()+1; i++){
    if(s1[i] == s2[i]){
      counter ++;
    }
    else{
      break;
    }
  }
  return counter;
}

Session::MappingType Session::map_uri_to_request_handler(std::string uri)
{
  std::string longest_match_root;
  std::string longest_match_path;
  int longest_length;
  std::string method;
  for (std::map<std::string, std::string>::iterator it = path_to_root.begin(); it != path_to_root.end(); ++it)
  {
    std::string path = it->first;
    std::string root = it->second;
    int current_length = common_prefix_length(uri, path);
    if(longest_length < current_length){
      longest_match_path = path;
      longest_match_root = root;
      longest_length = current_length;
      method = "static";
    }
  }
  for (std::map<std::string, std::string>::iterator it = path_to_root_echo.begin(); it != path_to_root_echo.end(); ++it)
  {
    std::string path = it->first;
    std::string root = it->second;
    int current_length = common_prefix_length(uri, path);
    if(longest_length < current_length){
      longest_match_path = path;
      longest_match_root = root;
      longest_length = current_length;
      method = "echo";
    }
  }
  MappingType result;
  if(method == "echo"){
    EchoRequestHandler rh;
    result.rh = rh;
    result.method = method;
    result.root = longest_match_root;
  }
  else{
    StaticRequestHandler sh;
    result.sh = sh;
    result.method = method;
    result.root = longest_match_root;
  }
  return result;
}

void Session::start()
{

  socket_.async_read_some(boost::asio::buffer(data_, max_length),
                          boost::bind(&Session::handle_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void Session::handle_final_read(const boost::system::error_code &error,
                                size_t bytes_transferred)
{
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  std::string remote_ip = this->socket().remote_endpoint().address().to_string();
  BOOST_LOG_TRIVIAL(trace) << "Additonal data fetched for IP (" << remote_ip << ")";
  char *full_text;
  full_text = (char *)malloc(strlen(buffer_) + strlen(data_) + 1);
  strcpy(full_text, data_);
  strcat(full_text, buffer_);

  Reply reply;
  reply.status = Reply::ok;
  request_.raw_request = full_text;

  request_handler_.handle_request(request_, reply);
  boost::asio::async_write(socket_,
                           reply.to_buffers(),
                           boost::bind(&Session::handle_write, this,
                                       boost::asio::placeholders::error));
  BOOST_LOG_TRIVIAL(trace) << "Reply sent back to IP ("
                           << remote_ip
                           << ") successfully!";
  memset(data_, 0, sizeof(data_));
  memset(buffer_, 0, sizeof(buffer_));
}

void Session::handle_read(const boost::system::error_code &error,
                          size_t bytes_transferred)
{
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  std::string remote_ip = this->socket().remote_endpoint().address().to_string();
  if (!error)
  {
    BOOST_LOG_TRIVIAL(info) << "Message received from IP address ("
                            << remote_ip
                            << ")...handling request";
    // Prechecking if an incoming HTTP message is valid or not
    RequestParser::result_type result = request_parser_.parse_data(request_, data_, bytes_transferred);

    // Result for HTTP request is good, send out HTTP response with code 200 back to client

    // TODO(Nelson and Duy): Need to Log the http request type(Post and Get).

    if (result == RequestParser::good)
    {
      BOOST_LOG_TRIVIAL(info) << "HTTP format check from IP ("
                              << remote_ip
                              << ") passed, preparing reply...";
      Reply reply;
      reply.status = Reply::ok;
      request_handler_.handle_request(request_, reply);
      boost::asio::async_write(socket_,
                               reply.to_buffers(),
                               boost::bind(&Session::handle_write, this,
                                           boost::asio::placeholders::error));
      BOOST_LOG_TRIVIAL(trace) << "Reply sent back to IP ("
                               << remote_ip
                               << ") successfully!";
    }

    // Result from HTTP request is good but the data portion is missing
    // Server tries to read again from socket to get the missing data
    else if (result == RequestParser::missing_data)
    {
      BOOST_LOG_TRIVIAL(info) << "HTTP format check for IP (" << remote_ip << ")passed but message is missing data, fetching...";
      int content_length = request_.get_content_length();

      socket_.async_read_some(boost::asio::buffer(buffer_, content_length),
                              boost::bind(&Session::handle_final_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    //result for http request is bad, async_write will send out http response 400 back to the client
    else if (result == RequestParser::bad)
    {
      BOOST_LOG_TRIVIAL(warning) << "HTTP format check failed from IP (" << remote_ip << "), preparing reply...";
      Reply reply;
      reply.status = Reply::bad_request;
      request_handler_.handle_request(request_, reply);
      boost::asio::async_write(socket_,
                               reply.to_buffers(),
                               boost::bind(&Session::handle_write, this,
                                           boost::asio::placeholders::error));
      BOOST_LOG_TRIVIAL(warning) << "Bad status reply sent back to IP (" << remote_ip << ").";
    }

    if (result != RequestParser::missing_data)
      memset(data_, 0, sizeof(data_));
  }
  else
  {
    BOOST_LOG_TRIVIAL(warning) << "Session is closed with IP (" << remote_ip << ")";
    delete this;
  }
}

void Session::handle_write(const boost::system::error_code &error)
{
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  std::string remote_ip = this->socket().remote_endpoint().address().to_string();
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    BOOST_LOG_TRIVIAL(warning) << "Session is closed with IP (" << remote_ip << ")";
    delete this;
  }
}
