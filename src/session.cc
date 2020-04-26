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
  std::vector<std::string> v1;
  std::vector<std::string> v2;
  char s1[a.size()+1];
  char s2[b.size()+1];
  strcpy(s1, a.c_str());
  strcpy(s2, b.c_str());
  std::string x = "";
  for(int i = 0; i < strlen(s1); i++){
    if(s1[i] == '/'){
      v1.push_back(x);
      x = "";
      continue;
    }
    x.push_back(s1[i]);
  }

  v1.push_back(x);
  x = "";
  for(int i = 0; i < strlen(s2); i++){
    if(s2[i] == '/'){
      v2.push_back(x);
      x = "";
      continue;
    }
    x.push_back(s2[i]);
  }
  v2.push_back(x);
  std::vector<std::string>::iterator it_1 = v1.begin();
  std::vector<std::string>::iterator it_2 = v2.begin();
  int counter = 0;
  while(it_2!=v2.end()){
    if(*it_2 == *it_1){
      counter ++;
    }
    it_2 ++;
    it_1 ++;
  }
  return counter;
}

Session::MappingType Session::map_uri_to_request_handler(std::string uri)
{
  std::string longest_match_root;
  std::string longest_match_path;
  int longest_length = 0;
  std::string method;
  for (std::map<std::string, std::string>::iterator it = path_to_root.begin(); it != path_to_root.end(); ++it)
  {
    std::string path = it->first;
    std::string root = it->second;
    int current_length = common_prefix_length(uri, path);
    if (longest_length < current_length)
    {
      longest_match_path = path;
      longest_match_root = root;
      longest_length = current_length;
      method = "static";
    }
     if(longest_length == current_length && path.length() < longest_match_path.length()){
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
    if (longest_length < current_length)
    {
      longest_match_path = path;
      longest_match_root = root;
      longest_length = current_length;
      method = "echo";
    }
     if(longest_length == current_length && path.length() < longest_match_path.length()){
      longest_match_path = path;
      longest_match_root = root;
      longest_length = current_length;
      method = "echo";
    }
  }
  MappingType result;
  if (method == "echo")
  {
    EchoRequestHandler rh;
    result.rh = rh;
    result.method = method;
    result.root = longest_match_root;
  }
  else
  {
    StaticRequestHandler sh(longest_match_root, longest_match_path);
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
                                size_t bytes_transferred, RequestHandler* request_handler)
{
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  std::string remote_ip = this->socket().remote_endpoint().address().to_string();
  BOOST_LOG_TRIVIAL(trace) << "Additonal data fetched for IP (" << remote_ip << ")";
  char *full_text;
  full_text = (char *)malloc(strlen(buffer_) + strlen(data_) + 1);
  strcpy(full_text, data_);
  strcat(full_text, buffer_);

  request_.raw_request = full_text;

  request_handler->handle_request(request_, reply_, RequestParser::result_type::good);
  boost::asio::async_write(socket_,
                           reply_.to_buffers(),
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
    request_ = Request();
    reply_ = Reply();
    RequestParser::result_type result = request_parser_.parse_data(request_, data_, bytes_transferred);

    request_parser_.reset();
    // Result for HTTP request is good, send out HTTP response with code 200 back to client
    MappingType mapping = map_uri_to_request_handler(request_.uri);

    RequestHandler *request_handler;
    if (mapping.method == "static")
    {
      request_handler = &mapping.sh;
    } 
    else {
      request_handler = &mapping.rh;
    }
    // TODO(Nelson and Duy): Need to Log the http request type(Post and Get).

    if (result == RequestParser::good)
    {
      BOOST_LOG_TRIVIAL(info) << "HTTP format check from IP ("
                              << remote_ip
                              << ") passed, preparing reply...";

      request_handler->handle_request(request_, reply_, result);
      boost::asio::async_write(socket_,
                               reply_.to_buffers(),
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
                                          boost::asio::placeholders::bytes_transferred, request_handler));
    }
    //result for http request is bad, async_write will send out http response 400 back to the client
    else if (result == RequestParser::bad)
    {
      BOOST_LOG_TRIVIAL(warning) << "HTTP format check failed from IP (" << remote_ip << "), preparing reply...";
      Reply reply;
      request_handler->handle_request(request_, reply, result);
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
