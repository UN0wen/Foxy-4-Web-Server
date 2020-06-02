#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "http_server.h"

HttpServer::HttpServer(boost::asio::io_service &io_service, short port, int num_threads, std::string hostname)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      num_threads_(num_threads),
      hostname_(hostname)
{
    start_accept();

    for (int i = 0; i < num_threads_; i++)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_)));
        threads_.push_back(thread);
    }
}

HttpServer::~HttpServer()
{
    for (auto thread : threads_)
    {
        thread->join();
    }
}

void HttpServer::start_accept()
{
    HttpSession *new_session = new HttpSession(io_service_, hostname_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&HttpServer::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void HttpServer::handle_accept(HttpSession *new_session,
                               const boost::system::error_code &error)
{
    if (!error)
    {
        new_session->start();
    }
    else
    {
        delete new_session;
    }

    start_accept();
}