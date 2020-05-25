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

std::string ProxyRequestHandler::convert_to_rawbody(Request request, bool isRedirect)
{
    std::string req = "";
    if(request.method_ == Request::MethodEnum::GET) {
        req += "GET";
    }
    else if(request.method_ == Request::MethodEnum::POST) {
        req += "POST";
    }
    else if(request.method_ == Request::MethodEnum::PUT) {
        req += "PUT";
    }
    else {
        req += "UNKNOWN";
    }
    req += " ";
    if (isRedirect) {
        req += request.uri_;
    } else {
        req += request.uri_.substr(path_.length()-1, request.uri_.length());
    }
    req += " HTTP";
    req += "/";
    req += "1.0";
    req += "\r\n";
    std::map<std::string, std::string>::iterator it;
    for(it = request.headers_.begin(); it != request.headers_.end(); it ++ ) {
        // Remove compression request
        if(it->first == "Accept-Encoding") {
            continue;
        }
        else if(it->second!="") {
            req += it->first;
            req += ": ";
            if(it->first == "Connection") {
                req += "close";
            }
            else {
                req += it->second;
            }
            req += "\r\n";
        }   
    }
    req += "\r\n";
    req += request.body_;
    return req;
}

Response ProxyRequestHandler::handle_request(const Request &client_request)
{
    // Initialize Response object with default set to not_found
    Response return_response = Response();
    return_response.code_ = Response::not_found;
    return_response.body_ = "";
    BOOST_LOG_TRIVIAL(trace) << "Proxy Request: " << client_request.uri_;

    
    // Reconstruct request string with Connection header set to close
    std::string raw_req = convert_to_rawbody(client_request, false);
    std::string root = root_;
    size_t loop_count = 0;

    // Get the local server host name
    std::string server_addr;
    for (auto it = client_request.headers_.begin(); it != client_request.headers_.end(); ++it) {
        if (it->first == "Host") {
            server_addr = it->second;
            size_t temp_pos = server_addr.find(":");
            if (temp_pos != std::string::npos)
                server_addr = server_addr.substr(0, temp_pos);
        }
    }

    try {
        // looping limit is set to 5 for now (to prevent inf. loop)
        while (loop_count < 5) {

            // Establish connection to the host with TCP socket
            boost::asio::io_service io_service;
            tcp::resolver resolver(io_service);
            tcp::resolver::query query(root.substr(0,root.length()-1), std::to_string(port_));
            tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            tcp::socket socket(io_service);
            boost::asio::connect(socket, endpoint_iterator);


            // Convert the request string to asio::streambuf and send the request
            boost::asio::streambuf request;
            std::ostream request_stream(&request);
            request_stream << raw_req;
            boost::asio::write(socket, request);


            // Read the response status line. The response streambuf will automatically
            // grow to accommodate the entire line. The growth may be limited by passing
            // a maximum size to the streambuf constructor.
            boost::asio::streambuf response;
            boost::asio::read_until(socket, response, "\r\n");


            // Check that response is OK.
            std::istream status_line_stream(&response);
            std::string http_version;
            unsigned int status_code;
            std::string status_message;

            status_line_stream >> http_version;
            status_line_stream >> status_code;
            std::getline(status_line_stream, status_message);
            return_response.code_ = static_cast<Response::status_code>(status_code);
            if (!status_line_stream || http_version.substr(0, 5) != "HTTP/") {
                socket.close();
                return ResponseGenerator::stock_response(Response::internal_server_error);
            }
            else if (status_code != 200 && status_code != 301 && status_code != 302) {
                socket.close();
                return ResponseGenerator::stock_response(return_response.code_);
            }

            // Read the response headers, which are terminated by a blank line.
            // and parse them to stored in response's header map
            std::string header;

            boost::asio::read_until(socket, response, "\r\n\r\n");
            std::istream header_stream(&response);

            while (std::getline(header_stream, header) && header != "\r") {
                bool isHeader = parse_header(header, return_response);
                if (isHeader == false) {
                    // header is always expected. but in case we miss them,
                    // handle it as internal server error
                    return ResponseGenerator::stock_response(Response::internal_server_error);
                }
            }
            std::cout << "HEADER:\n";
            for (auto it = return_response.headers_.begin(); it != return_response.headers_.end(); ++it)
                std::cout << it->first << ": " << it->second << std::endl;

            // checks for redirection. if true, reassign the root and repeat
            if (status_code == 301 || status_code == 302) {
                std::string new_root = return_response.headers_["Location"];
                if (new_root.find(server_addr) != std::string::npos) {
                    size_t pos = new_root.find("//");
                    if (pos != std::string::npos) {
                        new_root = new_root.substr(pos+2);
                    }
                    raw_req = redirect_request(new_root, client_request);
                    ++loop_count;
                    continue;
                }
            }

            // Convert and store the remaining contents into response's body
            if (response.size() > 0) {
                std::string ss( (std::istreambuf_iterator<char>(&response)), 
                                std::istreambuf_iterator<char>() );
                return_response.body_ = ss;
            }


            // Read in the rest of data from socket
            // Convert and store the contents into the body
            boost::system::error_code error;
            while (boost::asio::read(socket, response,
                boost::asio::transfer_at_least(1), error)) {
                std::string ss( (std::istreambuf_iterator<char>(&response)), 
                                std::istreambuf_iterator<char>() );
                return_response.body_ += ss;
            }


            // if the content is html, append the path_ to all alsolute paths
            if (return_response.headers_["Content-Type"].find("html") != std::string::npos) {
                return_response.body_ = clean_html(return_response.body_);
            }

            return_response.headers_["Content-Length"] = std::to_string(return_response.body_.length());

            if (error != boost::asio::error::eof)
                throw boost::system::system_error(error);
            break;
        }
    }
    catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return return_response;   
}


bool ProxyRequestHandler::parse_header(const std::string &header, Response &response)
{
    // header line expects "HEADER: VALUE" format
    if (header.find(":") == std::string::npos) {
        return false;
    }

    // Parse the header line and store them to response's headers_
    size_t pos = header.find(":");
    std::string head  = header.substr(0,pos);
    std::string value = header.substr(pos+2);
    value.pop_back();
    response.headers_[head] = value;
    return true;
}

std::string ProxyRequestHandler::clean_html(std::string &body)
{
    // variables to used within the scope
    size_t append_count = 0;
    std::string cleaned_body = "";
    size_t href_i, src_i, j;
    bool clean_href;

    while (true) {
        // define which index to parse
        href_i = body.find("href");
        src_i  = body.find("src");
        if (href_i == std::string::npos && src_i == std::string::npos) break;
        else if (href_i == std::string::npos) clean_href = false;
        else if (src_i == std::string::npos)  clean_href = true;
        else clean_href = href_i < src_i;

        if (clean_href) j = href_i + 4;
        else j = src_i + 3;


        // find the index of the url/path. if it starts with slash,
        // it is absolute path and append the path_
        while (j < body.size() && body[j] != '\"') ++j;
        if (j + 1 >= body.size()) {
            cleaned_body += body;
            break;
        }
        ++j;
        if (body[j] != '/') {
            cleaned_body += body.substr(0, j);
            body = body.substr(j);
        }
        else {
            cleaned_body += body.substr(0, j);
            cleaned_body += path_.substr(0,path_.length()-1);
            ++append_count;
            body = body.substr(j);
        }
    }
    cleaned_body += body;
    
    return cleaned_body;
}


std::string ProxyRequestHandler::redirect_request(std::string &new_root,  const Request &client_request)
{
    std::cout << "new_root: " << new_root << std::endl;
    if (new_root.find("\r") != std::string::npos)
        new_root.pop_back();
    size_t pos = 0;
    while (pos < new_root.size() && new_root[pos] != '/')
        ++pos;
    Request new_request = client_request;
    new_request.uri_ = new_root.substr(pos);

    return convert_to_rawbody(new_request, true);
}