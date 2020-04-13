//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "reply.h"

session::session(boost::asio::io_service &io_service)
    : socket_(io_service)
{
}

tcp::socket &session::socket()
{
  return socket_;
}

void session::start()
{
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
                          boost::bind(&session::handle_read, this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred));
}

void session::handle_final_read(const boost::system::error_code &error,
                                size_t bytes_transferred)
{
  char *full_text;
  full_text = (char *)malloc(strlen(buffer) + strlen(data_) + 1);
  strcpy(full_text, data_);
  strcat(full_text, buffer);
  std::cout << "my data :" << buffer << std::endl;
  std::cout << "my full_text :" << full_text << std::endl;
  boost::asio::async_write(socket_,
                           rh.process_request(true, full_text).to_buffers(),
                           boost::bind(&session::handle_write, this,
                                       boost::asio::placeholders::error));
  std::printf("Request complete\n");                                       
  memset(data_, 0, sizeof(data_));
  memset(buffer, 0, sizeof(buffer));
}

void session::handle_read(const boost::system::error_code &error,
                          size_t bytes_transferred)
{
  if (!error)
  {
    //reference of parser from boost library, precheck if a http comming msg is valid or not
    http::server::request_parser::result_type result = rh.http_format_precheck(data_, bytes_transferred);

    //result for http request is good, async_write helps to send out http response with 200 back to client
    if (result == http::server::request_parser::good)
    {
      http::server::request req = rh.get_request();

      std::printf("http request is valid, now processing....\n");

      boost::asio::async_write(socket_,
                               rh.process_request(true, data_).to_buffers(),
                               boost::bind(&session::handle_write, this,
                                           boost::asio::placeholders::error));
    std::printf("Request complete\n");                                           
    }

    else if (result == http::server::request_parser::missing_data)
    {
      std::printf("http request is missing data, now processing....\n");
      std::cout << "buffer established" << std::endl;
      socket_.async_read_some(boost::asio::buffer(buffer, max_length),
                              boost::bind(&session::handle_final_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    }
    //result for http request is bad, async_write will send out http response 400 back to the client
    else if (result == http::server::request_parser::bad)
    {
      std::printf("http request is not valid, sending back bad status...\n");
      boost::asio::async_write(socket_,
                               rh.process_request(false, data_).to_buffers(),
                               boost::bind(&session::handle_write, this,
                                           boost::asio::placeholders::error));
    std::printf("Request complete\n");                                           
    }
    
    if (result != http::server::request_parser::missing_data)  
      memset(data_, 0, sizeof(data_));
  }
  else
  {
    delete this;
  }
}

void session::handle_write(const boost::system::error_code &error)
{
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    delete this;
  }
}
