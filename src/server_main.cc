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
//reference: https://thispointer.com/get-current-date-time-in-c-example-using-boost-date-time-library/
//reference: https://www.boost.org/doc/libs/1_65_0/libs/log/example/doc/tutorial_file.cpp

#include <cstdlib>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/settings.hpp>
#include <boost/log/utility/setup/from_settings.hpp>
#include <thread>
#include <map>

#include "data_collector.h"
#include "server.h"
#include "config_parser.h"
namespace expr = boost::log::expressions;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)
using boost::asio::ip::tcp;

void signal_handler(int signum)
{
  BOOST_LOG_TRIVIAL(fatal) << "Server terminated successfully with interrupt signal: " << signum;
  exit(0);
}

void init_logging()
{
  logging::add_common_attributes();
  logging::settings log_settings;

  log_settings["Core"]["DisableLogging"] = false;
  
  // Console
  log_settings["Sinks.Console"]["Destination"] = "Console";
  log_settings["Sinks.Console"]["AutoFlush"] = true;
  log_settings["Sinks.Console"]["Format"] = "%TimeStamp% [Thread-ID: %ThreadID%]: <%Severity%> %Message%";

  // File
  log_settings["Sinks.File"]["RotationSize"] = 10 * 1024 * 1024;
  log_settings["Sinks.File"]["AutoFlush"] = true;
  log_settings["Sinks.File"]["Destination"] = "TextFile";
  log_settings["Sinks.File"]["Format"] = "LineID:%LineID% %TimeStamp% [Thread-ID: %ThreadID%]: <%Severity%> %Message%";
  log_settings["Sinks.File"]["RotationTimePoint"] = "07:00:00";
  // Extended ISO format: Year-month-day time-timezone
  log_settings["Sinks.File"]["FileName"] = "%Y-%m-%d %H:%M:%S%F%Q.log";

  logging::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");
  logging::register_simple_formatter_factory< boost::thread::id, char >("ThreadID");
  BOOST_LOG_SCOPED_THREAD_TAG("ThreadID", boost::this_thread::get_id());
  logging::init_from_settings(log_settings);
  src::logger_mt& lg = my_logger::get();
}

int main(int argc, char *argv[])
{
  init_logging();
  try
  {
    if (argc != 2)
    {
      printf("Usage: ./server <path to config file>\n");
      return 1;
    }

    signal(SIGTERM, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGINT, signal_handler);
    NginxConfigParser config_parser;
    NginxConfig config;

    int port;
    int threads = 4; //default number of threads
    std::string dir = "";
    std::map<std::string, std::shared_ptr<RequestHandler>> mapping;

    bool parse_success = config_parser.Parse(argv[1], &config);
    bool get_port = config.get_port(&port);
    bool get_dir = config.get_dir(&dir);
    bool get_threads = config.get_threads(&threads);

    RequestHandlerGenerator generator;
    bool get_map = generator.get_map(&config);
    if (parse_success && get_port && get_map && get_dir)
    {
      if(!dir.empty())
      {
        chdir(dir.c_str());
      }
      BOOST_LOG_TRIVIAL(trace) << "Config was parsed successfully";
      BOOST_LOG_TRIVIAL(trace) << "Starting server now";
      boost::asio::io_service io_service;

      Server s(io_service, port, generator, threads);
      io_service.run();
    }
    else
    {
      BOOST_LOG_TRIVIAL(error) << "Invalid Config";
      return 1;
    }
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}
