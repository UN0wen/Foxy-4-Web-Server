#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


using boost::asio::ip::tcp;
#include "http_session.h"
#include <map>

class HttpServer
{

public:
    HttpServer(boost::asio::io_service &io_service, short port, int num_threads, std::string hostname);
    ~HttpServer();
private:
    void start_accept();
    
    void handle_accept(HttpSession *new_session,
                       const boost::system::error_code &error);

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
    int num_threads_;
    std::string hostname_;

    std::vector<boost::shared_ptr<boost::thread>> threads_;
};
