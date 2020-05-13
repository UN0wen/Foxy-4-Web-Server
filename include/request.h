//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <string>
#include <map>
#include "header.h"

/// A request received from a client.
struct Request
{
  enum MethodEnum {
    GET,
    PUT,
    POST,
    UNKNOWN
  };
  MethodEnum method_;
  std::string uri_;
  std::string http_version_;
  std::map<std::string, std::string> headers_;
  std::string body_;

};
