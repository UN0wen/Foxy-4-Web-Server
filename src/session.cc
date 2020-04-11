//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "reply.h"

session::session(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

tcp::socket& session::socket()
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

http::server::reply session::process_request(){
  http::server::reply rep;
  std::printf("msg recieved: %s", data_);
  rep.status =  http::server::reply::ok;
      rep.content = data_;
      rep.headers.resize(2);
      rep.headers[0].name = "Content-Length";
      rep.headers[0].value = std::to_string(rep.content.size());
      rep.headers[1].name = "content-type";
      rep.headers[1].value = "text/plain";
  return rep;
}

void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      //TODO: Remove this and add to @Joshuas code.
      http::server::request_parser::result_type result;
      std::tie(result, std::ignore) = request_parser_.parse(request_,
							     data_,
							     data_ + bytes_transferred);
      std::cout << "result_type = " << result << std::endl;
      if(result == http::server::request_parser::good) {
	
	boost::asio::async_write(socket_,
				 session::process_request().to_buffers(),
				 boost::bind(&session::handle_write, this,
					     boost::asio::placeholders::error));
	std::printf("Request complete %s", data_);
      }
      else if (result == http::server::request_parser::bad) {
	//TODO: @Joshua
      }
    }
    else
    {
      delete this;
    }
  }

void session::handle_write(const boost::system::error_code& error)
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
