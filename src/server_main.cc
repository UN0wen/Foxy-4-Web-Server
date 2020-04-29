//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//reference: https://www.boost.org/doc/libs/1_55_0/libs/log/example/bounded_async_log/main.cpp
//https://www.boost.org/doc/libs/1_55_0/libs/log/example/*.cpp

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <thread>
#include <map>

#include "server.h"
#include "config_parser.h"
namespace expr = boost::log::expressions;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)
using boost::asio::ip::tcp;


void signal_handler( int signum ) {
   BOOST_LOG_TRIVIAL(fatal) << "Server terminated successfully with interrupt signal: " << signum;
   exit(0);  
}


int main(int argc, char* argv[])
{
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  logging::add_console_log(std::clog, keywords::format = expr::format("%1% [Thread-ID: %2%]: <%3%> %4%")
            % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
            % expr::attr< boost::thread::id >("ThreadID")
            % logging::trivial::severity
            % expr::smessage);
  logging::add_file_log(
    "sample1.log", 
    keywords::auto_flush = true, 
    keywords::rotation_size = 10 * 1024 * 1024, 
    keywords::format =
        (
           expr::format("lineID:%1% %2% [Thread-ID: %3%]: <%4%> %5%")
            % expr::attr< unsigned int >("LineID")
            % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
            % expr::attr< boost::thread::id >("ThreadID")
            % logging::trivial::severity
            % expr::smessage
        )
    );
  logging::add_common_attributes();
  src::logger_mt& lg = my_logger::get();
  
  try
  {
    if (argc != 2) {
    printf("Usage: ./server <path to config file>\n");
    return 1;
    }

    signal(SIGTERM, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGINT, signal_handler);  
    NginxConfigParser config_parser;
    NginxConfig config;

    int port;

    bool parse_success = config_parser.Parse(argv[1], &config);
    bool get_port = config.GetPort(&port);
    if(parse_success && get_port) {
      BOOST_LOG_TRIVIAL(trace) << "Config was parsed successfully";
      BOOST_LOG_TRIVIAL(trace) << "Starting server now";
      boost::asio::io_service io_service;
      config.GetMap();
      std::map<std::string, std::string> root_path = config.root_to_path_map;
      std::map<std::string, std::string> root_path_echo = config.root_to_path_map_echo;
      Server s(io_service, port, root_path, root_path_echo);
      io_service.run();
    }
    else {
      BOOST_LOG_TRIVIAL(error) << "Config was parsed unsuccessfully";
      return 1;
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
