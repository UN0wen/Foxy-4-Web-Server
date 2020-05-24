#include <boost/log/trivial.hpp>
#include <boost/asio.hpp>
#include "response_generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "data_collector.h"
#include "mime_types.h"
#include "proxy_request_handler.h"
#include "utility.h"

using boost::asio::ip::tcp;

ProxyRequestHandler::ProxyRequestHandler(const std::string &root, const std::string &path, const int &port)
	: root_(root), path_(path), port_(port)
{
}

RequestHandler* ProxyRequestHandler::Init(const std::string& location_path, const NginxConfig& config)
{
    std::string root = "";
	std::string path = "";
	int port = 0;
	DataCollector::get_instance()->add_handler(location_path, "Proxy Request Handler");
	for (const auto &s : config.statements_)
	{
		std::vector<std::string>::iterator find_host = std::find(s->tokens_.begin(), s->tokens_.end(), "host");
		std::vector<std::string>::iterator find_port = std::find(s->tokens_.begin(), s->tokens_.end(), "port");

		if (find_host != s->tokens_.end())
		{
			root = *(find_host + 1);
		}

		if (find_port != s->tokens_.end())
		{
			port = std::stoi(*(find_port + 1));
		}
	}
	if (!utility::is_quoted(&root))
		return nullptr;

	BOOST_LOG_TRIVIAL(trace) << "Host: " + root + " Port: " + std::to_string(port);

	ProxyRequestHandler* proxy_request_handler = new ProxyRequestHandler(root, location_path, port);
	return proxy_request_handler;
}

std::string ProxyRequestHandler::convert_to_rawbody(Request request){
  std::string req = "";
  if(request.method_ == Request::MethodEnum::GET){
    req += "GET";
  }
  else if(request.method_ == Request::MethodEnum::POST){
    req += "POST";
  }
  else if(request.method_ == Request::MethodEnum::PUT){
    req += "PUT";
  }
  else{
    req += "UNKNOWN";
  }
  req += " ";
  req += request.uri_.substr(path_.length()-1, request.uri_.length());
  req += " HTTP";
  req += "/";
  req += request.http_version_;
  req += "\r\n";
  std::map<std::string, std::string>::iterator it;
  for(it = request.headers_.begin(); it != request.headers_.end(); it ++ ){
        if(it->second!=""){
          req += it->first;
          req += ": ";
          if(it->first == "Connection") {
            req += "close";
          }
          else
            req += it->second;
          req += "\r\n";
        }   
  }
  req += "\r\n";
  req += request.body_;
  return req;
}

Response ProxyRequestHandler::handle_request(const Request &request)
{
	
	Response return_response = Response();
    return_response.code_ = Response::not_found;
    return_response.body_ = "";
	BOOST_LOG_TRIVIAL(trace) << "Proxy Request: " << request.uri_;

	std::string raw_req = convert_to_rawbody(request);
    //we probably don't need this complete_address so commenting out for now but may be useful later
    //std::string complete_address = root_.substr(0,root_.length()-1) + ":" + std::to_string(port_);

    try
    {
        boost::asio::io_service io_service;
        std::ostringstream ss;
        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_service);
        //boost::system::error_code ec;
        //tcp::resolver::query query(complete_address, "http");
        tcp::resolver::query query(root_.substr(0,root_.length()-1), std::to_string(port_));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << raw_req;

        // Send the request.
        boost::asio::write(socket, request);
        // Read the response status line. The response streambuf will automatically
        // grow to accommodate the entire line. The growth may be limited by passing
        // a maximum size to the streambuf constructor.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
        std::cout << "Invalid response\n";
        return return_response;
        }
        if (status_code != 302 && status_code != 200)
        {
        std::cout << "Response returned with status code " << status_code << "\n";
        return return_response;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r")
        std::cout << header << "\n";
        std::cout << "\n";

        // Write whatever content we already have to output.
        // this is where response should be formed probably
        if (response.size() > 0)
        {
            std::cout << &response;
            ss << &response;
            return_response.body_ = ss.str();
        }
        boost::system::error_code error;
        while (boost::asio::read(socket, response,
            boost::asio::transfer_at_least(1), error))
        std::cout << &response;
        
        if (error != boost::asio::error::eof)
        throw boost::system::system_error(error);
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return_response.code_ = Response::moved_temporarily;

    std::cout << "response body: " << std::endl << return_response.body_ << std::endl;

    return return_response;
    
}