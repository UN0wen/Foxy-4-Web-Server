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
      std::printf("http request is valid, now processing....\n");
      boost::asio::async_write(socket_,
                               rh.process_request(true, data_).to_buffers(),
                               boost::bind(&session::handle_write, this,
                                           boost::asio::placeholders::error));
    }

    //result for http request is bad, async_write will send out http response 400 back to the client
    else if (result == http::server::request_parser::bad)
    {
      std::printf("http request is not valid, sending back bad status...\n");
      boost::asio::async_write(socket_,
                               rh.process_request(false, data_).to_buffers(),
                               boost::bind(&session::handle_write, this,
                                           boost::asio::placeholders::error));
    }
    std::printf("Request complete\n");
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
