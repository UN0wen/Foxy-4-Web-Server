#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


using boost::asio::ip::tcp;
#include "session.h"
#include <map>

class Server
{

public:
    Server(boost::asio::io_service &io_service, short port, RequestHandlerGenerator generator, int num_threads);

private:
    void start_accept();
    
    void handle_accept(Session *new_session,
                       const boost::system::error_code &error);

    void setup_ssl();
    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
    RequestHandlerGenerator generator_;
    int num_threads_;
    boost::asio::ssl::context context_;
};
