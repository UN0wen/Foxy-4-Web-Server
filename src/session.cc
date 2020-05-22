//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
#include "response.h"
#include "request.h"
#include "data_collector.h"
#include "utility.h"

#define BOOST_LOG_DYN_LINK 1
BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());

Session::Session(boost::asio::io_service &io_service, RequestHandlerGenerator generator)
    : socket_(io_service),
      generator_(generator)
{
  data_collector_ = DataCollector::get_instance();
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

void Session::deep_copy_response(Response response)
{
  response_generator_.init(response);
}

void Session::handle_bad_request()
{
  BOOST_LOG_TRIVIAL(trace) << "generating bad request for client";
  Response response = ResponseGenerator::stock_response(Response::status_code::bad_request);
  data_collector_->increment_request(request_.uri_, response.code_);
  response_generator_.init(response);
}

void Session::handle_final_read(const boost::system::error_code &error,
                                size_t bytes_transferred, RequestHandler *request_handler)
{
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  std::string remote_ip = this->socket().remote_endpoint().address().to_string();
  BOOST_LOG_TRIVIAL(trace) << "Additonal data fetched for IP (" << remote_ip << ")";

  request_.body_ = buffer_;

  Response response = request_handler->handle_request(request_);
  deep_copy_response(response);
  data_collector_->increment_request(request_.uri_, response.code_);
  boost::asio::async_write(socket_,
                           response_generator_.to_buffers(),
                           boost::bind(&Session::handle_write, this,
                                       boost::asio::placeholders::error));
  BOOST_LOG_TRIVIAL(trace) << "Response sent back to IP ("
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
    RequestParser::result_type result = RequestParser::indeterminate;
    result = request_parser_.parse_data(request_, data_, bytes_transferred);
    boost::system::error_code error;

    while(result == RequestParser::indeterminate)
    {
      boost::asio::read(socket_, boost::asio::buffer(data_, max_length), boost::asio::transfer_at_least(1), error);
      result = request_parser_.parse_data(request_, data_, bytes_transferred);
      request_parser_.reset();
      if(error)
        break;
    }

    if(error)
    {
      BOOST_LOG_TRIVIAL(warning) << "Invalid or incomplete request for IP (" << remote_ip <<") closing...";
      return;
    }

    BOOST_LOG_TRIVIAL(warning) << "Request Parser finish parsing request (" << remote_ip << ")";
    RequestHandler *request_handler = generator_.dispatch_handler(request_.uri_).get();
    BOOST_LOG_TRIVIAL(warning) << "Request handler generator finish assigning specific request handler(" << remote_ip << ")";
    request_parser_.reset();
    // Result for HTTP request is good, send out HTTP response with code 200 back to client

    if (result == RequestParser::good)
    {
      BOOST_LOG_TRIVIAL(info) << "HTTP format check from IP ("
                              << remote_ip
                              << ") passed, preparing response...";
      Response response = request_handler->handle_request(request_);
      deep_copy_response(response);
      data_collector_->increment_request(request_.uri_, response.code_);
      boost::asio::async_write(socket_,
                               response_generator_.to_buffers(),
                               boost::bind(&Session::handle_write, this,
                                           boost::asio::placeholders::error));
      BOOST_LOG_TRIVIAL(trace) << "Response sent back to IP ("
                               << remote_ip
                               << ") successfully!";
    }

    // Result from HTTP request is good but the data portion is missing
    // Server tries to read again from socket to get the missing data
    else if (result == RequestParser::missing_data)
    {
      BOOST_LOG_TRIVIAL(info) << "HTTP format check for IP (" << remote_ip << ")passed but message is missing data, fetching...";
      
      int content_length = utility::get_content_length(request_);
      socket_.async_read_some(boost::asio::buffer(buffer_, max_length),
                              boost::bind(&Session::handle_final_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred, request_handler));
    }
    //result for http request is bad, async_write will send out http response 400 back to the client
    else if (result == RequestParser::bad)
    {
      BOOST_LOG_TRIVIAL(warning) << "HTTP format check failed from IP (" << remote_ip << "), preparing response...";
      handle_bad_request();
      boost::asio::async_write(socket_,
                               response_generator_.to_buffers(),
                               boost::bind(&Session::handle_write, this,
                                           boost::asio::placeholders::error));
      BOOST_LOG_TRIVIAL(warning) << "Bad status response sent back to IP (" << remote_ip << ").";
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
