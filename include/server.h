#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;
#include "session.h"

class Server
{

public:
    Server(boost::asio::io_service &io_service, short port);

private:
    void start_accept();

    void handle_accept(Session *new_session,
                       const boost::system::error_code &error);

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
};