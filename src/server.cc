#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "server.h"

Server::Server(boost::asio::io_service &io_service, short port, RequestHandlerGenerator generator, int num_threads)
    : io_service_(io_service),
      acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
      generator_(generator),
      num_threads_(num_threads),
      context_(boost::asio::ssl::context::sslv23)
{
    setup_ssl();
    start_accept();

    std::vector<boost::shared_ptr<boost::thread>> threads;

    for (int i = 0; i < num_threads_; i++)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_)));
        threads.push_back(thread);
    }

    for (auto thread : threads)
    {
        thread->join();
    }
}

void Server::start_accept()
{
    Session *new_session = new Session(io_service_, context_, generator_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&Server::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void Server::handle_accept(Session *new_session,
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

void Server::setup_ssl()
{
    context_.set_options(
        boost::asio::ssl::context::default_workarounds);
    context_.set_verify_mode(boost::asio::ssl::verify_none);
    context_.use_certificate_chain_file("/usr/src/project/certs/foxy_bundle.crt");
    context_.use_certificate_file("/usr/src/project/certs/foxy_cert.crt", boost::asio::ssl::context::pem);
    context_.use_private_key_file("/usr/src/project/certs/foxy.key", boost::asio::ssl::context::pem);
}