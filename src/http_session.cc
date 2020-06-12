//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/request_handler.cpp
//reference usage: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "http_session.h"
#include "request.h"
#include "utility.h"

#define BOOST_LOG_DYN_LINK 1
BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());

HttpSession::HttpSession(boost::asio::io_service &io_service, std::string hostname)
    : socket_(io_service),
      hostname_(hostname)
{
    data_collector_ = DataCollector::get_instance();
}

tcp::socket &HttpSession::socket()
{
    return socket_;
}

void HttpSession::start()
{
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&HttpSession::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void HttpSession::deep_copy_response(Response response)
{
    response_generator_.init(response);
}

Response HttpSession::handle_request(const Request &request)
{
    Response response;
    BOOST_LOG_TRIVIAL(info) << "[HttpSession] Http Redirector is handling request";
    std::string location = "https://" + hostname_ + request.uri_;

    if (request.uri_[0] != '/')
    {
        response = ResponseGenerator::stock_response(Response::status_code::bad_request);
    }
    else
    {
        response.code_ = Response::moved_temporarily;
        response.headers_["Content-Length"] = std::to_string(response.body_.size());
        response.headers_["Content-Type"] = "text/plain";
        response.headers_["Connection"] = "close";
        response.headers_["Location"] = location;
    }

    BOOST_LOG_TRIVIAL(info) << "[ResponseLength] " << std::to_string(response.body_.size());
    BOOST_LOG_TRIVIAL(info) << "[HttpSession] Finished processing request.";
    return response;
}

void HttpSession::handle_bad_request()
{
    BOOST_LOG_TRIVIAL(info) << "[Request] Bad request";
    Response response = ResponseGenerator::stock_response(Response::status_code::bad_request);
    data_collector_->increment_request(request_.uri_, response.code_);
    deep_copy_response(response);
}

void HttpSession::handle_final_read(const boost::system::error_code &error,
                                    size_t bytes_transferred)
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    std::string remote_ip = this->socket().remote_endpoint().address().to_string();
    BOOST_LOG_TRIVIAL(trace) << "[HttpSession] Additonal data fetched for IP: " << remote_ip;
    request_.body_ = buffer_;
    Response response = handle_request(request_);
    deep_copy_response(response);
    data_collector_->increment_request(request_.uri_, response.code_);
    boost::asio::async_write(socket_,
                             response_generator_.to_buffers(),
                             boost::bind(&HttpSession::handle_write, this,
                                         boost::asio::placeholders::error));
    memset(data_, 0, sizeof(data_));
    memset(buffer_, 0, sizeof(buffer_));
}

void HttpSession::handle_read(const boost::system::error_code &error,
                              size_t bytes_transferred)
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    std::string remote_ip = this->socket().remote_endpoint().address().to_string();
    if (!error)
    {
        BOOST_LOG_TRIVIAL(info) << "[Request] IP: " << remote_ip;
        // Prechecking if an incoming HTTP message is valid or not
        request_ = Request();
        RequestParser::result_type result = RequestParser::indeterminate;
        result = request_parser_.parse_data(request_, data_, bytes_transferred);
        boost::system::error_code error;
        while (result == RequestParser::indeterminate)
        {
            boost::asio::read(socket_, boost::asio::buffer(data_, max_length), boost::asio::transfer_at_least(1), error);
            result = request_parser_.parse_data(request_, data_, bytes_transferred);
            request_parser_.reset();
            if (error)
                break;
        }
        if (error)
        {
            BOOST_LOG_TRIVIAL(warning) << "[HttpSession] Invalid or incomplete request for IP: " << remote_ip;
            return;
        }
        BOOST_LOG_TRIVIAL(trace) << "[HttpSession] Finished parsing request from IP: " << remote_ip;
        request_parser_.reset();
        // Result for HTTP request is good, send out HTTP response with code 200 back to client
        if (result == RequestParser::good)
        {
            BOOST_LOG_TRIVIAL(trace) << "[HttpSession] HTTP format check from IP ("
                                     << remote_ip
                                     << ") passed, preparing response...";
            BOOST_LOG_TRIVIAL(info) << "[Request] URI: " << request_.uri_;
            Response response = handle_request(request_);
            deep_copy_response(response);
            data_collector_->increment_request(request_.uri_, response.code_);
            boost::asio::async_write(socket_,
                                     response_generator_.to_buffers(),
                                     boost::bind(&HttpSession::handle_write, this,
                                                 boost::asio::placeholders::error));
        }
        // Result from HTTP request is good but the data portion is missing
        // Server tries to read again from socket to get the missing data
        else if (result == RequestParser::missing_data)
        {
            BOOST_LOG_TRIVIAL(trace) << "[HttpSession] HTTP format check for IP (" << remote_ip << ") passed but message is missing data, fetching...";
            int content_length = utility::get_content_length(request_);
            socket_.async_read_some(boost::asio::buffer(buffer_, max_length),
                                    boost::bind(&HttpSession::handle_final_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
        //result for http request is bad, async_write will send out http response 400 back to the client
        else if (result == RequestParser::bad)
        {
            BOOST_LOG_TRIVIAL(trace) << "[HttpSession] HTTP format check failed from IP (" << remote_ip << "), preparing response...";
            handle_bad_request();
            boost::asio::async_write(socket_,
                                     response_generator_.to_buffers(),
                                     boost::bind(&HttpSession::handle_write, this,
                                                 boost::asio::placeholders::error));
        }
        if (result != RequestParser::missing_data)
            memset(data_, 0, sizeof(data_));
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "[HttpSession] Closed session with IP " << remote_ip;
        delete this;
    }
}

void HttpSession::handle_write(const boost::system::error_code &error)
{
    BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
    std::string remote_ip = this->socket().remote_endpoint().address().to_string();
    if (!error)
    {
        BOOST_LOG_TRIVIAL(info) << "[Response] " << std::to_string(response_generator_.code_) << " response sent back to " << remote_ip;
        // Close connection if client sends Connection:close
        if (request_.headers_["Connection"] == "close" || request_.headers_["connection"] == "close")
        {
            BOOST_LOG_TRIVIAL(info) << "[HttpSession] Closed session with IP " << remote_ip;
            socket_.close();
            delete this;
        }
        else
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&HttpSession::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "[HttpSession] Closed session with IP " << remote_ip;
        socket_.close();
        delete this;
    }
}