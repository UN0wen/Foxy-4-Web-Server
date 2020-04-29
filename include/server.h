#pragma once

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;
#include "session.h"
#include <map>

class Server
{

public:
    Server(boost::asio::io_service &io_service, short port, std::map<std::string, std::string> path_to_root, std::map<std::string, std::string> path_to_root_echo);

private:
    void start_accept();

    void handle_accept(Session *new_session,
                       const boost::system::error_code &error);

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
    std::map<std::string, std::string> path_to_root;
    std::map<std::string, std::string> path_to_root_echo;
};