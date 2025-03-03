//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// reference: https://www.boost.org/doc/libs/1_65_1/doc/html/boost_asio/example/cpp11/http/server/reply.hpp

#pragma once

#include <string>
#include <map>
#include <boost/asio.hpp>


/// A reply to be sent to a client.
struct Response
{
  /// The status of the reply.
  enum status_code
  {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } code_;
  
  /// The headers to be included in the reply.
  std::map<std::string, std::string> headers_;

  /// The content to be sent in the reply.
  std::string body_;

};

