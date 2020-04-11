//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "server.h"
#include "config_parser.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2) {
    printf("Usage: ./config_parser <path to config file>\n");
    return 1;
    }

    NginxConfigParser config_parser;
    NginxConfig config;

    int port;

    bool parse_success = config_parser.Parse(argv[1], &config);
    bool get_port = config.GetPort(&port);
    
    if(parse_success && get_port) {
      boost::asio::io_service io_service;

      server s(io_service, port);

      io_service.run();
    }
    else {
      std::cerr << "Error: Invalid config file." << "\n";
      return 0;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
