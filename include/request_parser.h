//
// RequestParser.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <tuple>
#include <iostream>

#include "request.h"

/// Parser for incoming requests.
class RequestParser
{
public:
  /// Construct ready to parse the request method.
  RequestParser();

  /// Reset to initial parser state.
  void reset();

  int get_char_amount();

  /// Result of parse.
  enum result_type
  {
    good,
    bad,
    indeterminate,
    missing_data
  };

  result_type parse_data(Request &request, const char data[], size_t bytes_transferred);

  /// Parse some data. The enum return value is good when a complete request has
  /// been parsed, bad if the data is invalid, indeterminate when more data is
  /// required. The InputIterator return value indicates how much of the input
  /// has been consumed.
  template <typename InputIterator>
  std::tuple<result_type, InputIterator> parse(Request &req,
                                               InputIterator begin, InputIterator end)
  {
    while (begin != end)
    {
      RequestParser::result_type result = consume(req, *begin++);
      n_char_parsed++;
      if (result == good || result == bad)
      {
        return std::make_tuple(result, begin);
      }
    }
    return std::make_tuple(indeterminate, begin);
  }

private:
  int major, minor;
  std::string method_helper = "";
  std::string header_helper = "";
  
  int n_char_parsed;
  /// Handle the next character of input.
  result_type consume(Request &req, char input);

  /// Check if a byte is an HTTP character.
  static bool is_char(int c);

  /// Check if a byte is an HTTP control character.
  static bool is_ctl(int c);

  /// Check if a byte is defined as an HTTP tspecial character.
  static bool is_tspecial(int c);

  /// Check if a byte is a digit.
  static bool is_digit(int c);

  /// The current state of the parser.
  enum state
  {
    method_start,
    method,
    uri,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3
  } state_;
};
