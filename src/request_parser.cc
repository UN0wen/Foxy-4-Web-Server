//
// RequestParser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_parser.h"
#include <string.h>

RequestParser::RequestParser()
    : state_(method_start)
{
  n_char_parsed = 0;
}

int RequestParser::get_char_amount()
{
  return n_char_parsed;
}

void RequestParser::reset()
{
  major = 0;
  minor = 0;
  method_helper = "";
  header_helper = "";
  state_ = method_start;
}

RequestParser::result_type RequestParser::parse_data(Request &request, const char data[], size_t bytes_transferred)
{
  RequestParser::result_type result;

  std::tie(result, std::ignore) = parse(request, data, data + bytes_transferred);

  int char_amount = get_char_amount();
  int content_length = 0;
  std::string content_length_str = request.headers_["Content-Length"];
  if (content_length_str != "")
    {
      content_length = std::stoi(content_length_str);
    }
  if (content_length > 0 && char_amount == strlen(data))
  {
    return result = RequestParser::result_type::missing_data;
  }
  std::string data_helper(data);
  request.body_ = data_helper.substr(data_helper.length() - content_length);

  return result;
}

RequestParser::result_type RequestParser::consume(Request &req, char input)
{
  switch (state_)
  {
  case method_start:
    if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return bad;
    }
    else
    {
      state_ = method;
      method_helper.push_back(input);
      return indeterminate;
    }
  case method:
    if (input == ' ')
    {
      if (method_helper == "POST")
	{
	  req.method_ = Request::POST;
	}
      else if (method_helper == "PUT")
	{
	  req.method_ = Request::PUT;
	}
      else if (method_helper == "GET")
	{
	  req.method_ = Request::GET;
	}
      else
	{
	  req.method_ = Request::UNKNOWN;
	}
      state_ = uri;
      return indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return bad;
    }
    else
    {
      method_helper.push_back(input);
      return indeterminate;
    }
  case uri:
    if (input == ' ')
    {
      state_ = http_version_h;
      return indeterminate;
    }
    else if (is_ctl(input))
    {
      return bad;
    }
    else
    {
      req.uri_.push_back(input);
      return indeterminate;
    }
  case http_version_h:
    if (input == 'H')
    {
      state_ = http_version_t_1;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_t_1:
    if (input == 'T')
    {
      state_ = http_version_t_2;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_t_2:
    if (input == 'T')
    {
      state_ = http_version_p;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_p:
    if (input == 'P')
    {
      state_ = http_version_slash;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_slash:
    if (input == '/')
    {
      major = 0;
      minor = 0;
      req.http_version_ = "";
      state_ = http_version_major_start;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_major_start:
    if (is_digit(input))
    {
      req.http_version_.push_back(input);
      state_ = http_version_major;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_major:
    if (input == '.')
    {
      req.http_version_.push_back('.');
      state_ = http_version_minor_start;
      return indeterminate;
    }
    else if (is_digit(input))
    {
      req.http_version_.push_back(input);
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_minor_start:
    if (is_digit(input))
    {
      req.http_version_.push_back(input);
      state_ = http_version_minor;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case http_version_minor:
    if (input == '\r')
    {
      state_ = expecting_newline_1;
      return indeterminate;
    }
    else if (is_digit(input))
    {
      req.http_version_.push_back(input);
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case expecting_newline_1:
    if (input == '\n')
    {
      state_ = header_line_start;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case header_line_start:
    if (input == '\r')
    {
      state_ = expecting_newline_3;
      return indeterminate;
    }
    else if (!req.headers_.empty() && (input == ' ' || input == '\t'))
    {
      state_ = header_lws;
      return indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return bad;
    }
    else
    {
      header_helper.push_back(input);
      state_ = header_name;
      return indeterminate;
    }
  case header_lws:
    if (input == '\r')
    {
      state_ = expecting_newline_2;
      return indeterminate;
    }
    else if (input == ' ' || input == '\t')
    {
      return indeterminate;
    }
    else if (is_ctl(input))
    {
      return bad;
    }
    else
    {
      state_ = header_value;
      header_helper.push_back(input);
      return indeterminate;
    }
  case header_name:
    if (input == ':')
    {
      state_ = space_before_header_value;
      return indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return bad;
    }
    else
    {
      header_helper.push_back(input);
      return indeterminate;
    }
  case space_before_header_value:
    if (input == ' ')
    {
      req.headers_.insert({header_helper, ""});
      state_ = header_value;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case header_value:
    if (input == '\r')
    {
      header_helper.clear();
      state_ = expecting_newline_2;
      return indeterminate;
    }
    else if (is_ctl(input))
    {
      return bad;
    }
    else
    {
      req.headers_[header_helper].push_back(input);
      return indeterminate;
    }
  case expecting_newline_2:
    if (input == '\n')
    {
      state_ = header_line_start;
      return indeterminate;
    }
    else
    {
      return bad;
    }
  case expecting_newline_3:
    return (input == '\n') ? good : bad;
  default:
    return bad;
  }
}

bool RequestParser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool RequestParser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::is_tspecial(int c)
{
  switch (c)
  {
  case '(':
  case ')':
  case '<':
  case '>':
  case '@':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '/':
  case '[':
  case ']':
  case '?':
  case '=':
  case '{':
  case '}':
  case ' ':
  case '\t':
    return true;
  default:
    return false;
  }
}

bool RequestParser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}
