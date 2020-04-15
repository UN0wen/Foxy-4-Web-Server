//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "reply.h"

Session::Session(boost::asio::io_service &io_service)
    : socket_(io_service)
{
}

tcp::socket &Session::socket()
{
  return socket_;
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
  std::cout << "Additonal data fetched: " << buffer_ << std::endl;
  char *full_text;
  full_text = (char *)malloc(strlen(buffer_) + strlen(data_) + 1);
  strcpy(full_text, data_);
  strcat(full_text, buffer_);
  
  boost::asio::async_write(socket_,
                           request_handler_.process_request(true, full_text).to_buffers(),
                           boost::bind(&Session::handle_write, this,
                                       boost::asio::placeholders::error));

  std::cout << "Reply sent." << std::endl; 
  memset(data_, 0, sizeof(data_));
  memset(buffer_, 0, sizeof(buffer_));
}

void Session::handle_read(const boost::system::error_code &error,
                          size_t bytes_transferred)
{
  if (!error)
  {
    std::cout << "Message received: " << data_ << std::endl;
    // Prechecking if an incoming HTTP message is valid or not
    RequestParser::result_type result = request_handler_.http_format_precheck(data_, bytes_transferred);

    // Result for HTTP request is good, send out HTTP response with code 200 back to client
    if (result == RequestParser::good)
    {
      std::cout << "HTTP format check passed, preparing reply..." << std::endl;

      Request req = request_handler_.get_request();

      boost::asio::async_write(socket_,
                               request_handler_.process_request(true, data_).to_buffers(),
                               boost::bind(&Session::handle_write, this,
                                           boost::asio::placeholders::error));
      std::cout << "Reply sent." << std::endl;                                  
    }

    // Result from HTTP request is good but the data portion is missing
    // Server tries to read again from socket to get the missing data
    else if (result == RequestParser::missing_data)
    {
      std::cout << "HTTP format check passed but message is missing data, fetching..." << std::endl;
      int content_length = request_handler_.get_content_length();
      socket_.async_read_some(boost::asio::buffer(buffer_, max_length),
                              boost::bind(&Session::handle_final_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    //result for http request is bad, async_write will send out http response 400 back to the client
    else if (result == RequestParser::bad)
    {
      std::cout << "HTTP format check failed, preparing reply..." << std::endl;
      boost::asio::async_write(socket_,
                               request_handler_.process_request(false, data_).to_buffers(),
                               boost::bind(&Session::handle_write, this,
                                           boost::asio::placeholders::error));
      std::cout << "Reply sent." << std::endl;                                            
    }
    
    if (result != RequestParser::missing_data)  
      memset(data_, 0, sizeof(data_));
  }
  else
  {
    delete this;
  }
}

void Session::handle_write(const boost::system::error_code &error)
{
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&Session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    delete this;
  }
}
