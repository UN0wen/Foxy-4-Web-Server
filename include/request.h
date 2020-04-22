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
#include <vector>
#include "header.h"

/// A request received from a client.
struct Request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<Header> headers;
  std::string data;

  // the entire raw request for echo functionality
  const char *raw_request;

  // Return the content length of the request
  int get_content_length()
  {
    {
      for (int i = 0; i < headers.size(); ++i)
      {
        if (headers[i].name == "Content-Length")
        {
          return std::stoi(headers[i].value);
        }
      }

      return 0;
    }
  }
};
