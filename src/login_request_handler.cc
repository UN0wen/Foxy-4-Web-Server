#include "login_request_handler.h"
#include "response_generator.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "data_collector.h"
#include "utility.h"
#include <regex>
#include <iterator>


LoginRequestHandler::LoginRequestHandler(const std::string &root, const std::string &path)
    : root_(root), path_(path)
{
}

RequestHandler *LoginRequestHandler::Init(const std::string &location_path, const NginxConfig &config)
{
    
    DataCollector::get_instance()->add_handler(location_path, "Login Request Handler");
    std::string root = "";
    for (const auto &s : config.statements_)
    {
        std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
                                                                 s->tokens_.end(),
                                                                 "root");
        if (find_root != s->tokens_.end())
        {
            root = *(find_root + 1);
        }
    }
    if (!utility::is_quoted(&root))
        return nullptr;
    LoginRequestHandler *login_request_handler = new LoginRequestHandler(root, location_path);
    BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Set up with root: " + root;
    return login_request_handler;
}

std::string LoginRequestHandler::get_extension(const std::string &request_path)
{
    // Determine the file extension.
    std::string extension = "";
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");

    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
        extension = request_path.substr(last_dot_pos + 1);
    }

    return extension;
}

Response LoginRequestHandler::perpare_html_response(const std::string file)
{
    //(JOSH): the reason that I am pasting the whole html here is because my browser does not even work with static request handler, which always return 404
    std::string login_form = "<!DOCTYPE html><html><h2>Foxy-4 Login Page</h2><form method=\"POST\">  <label for=\"fname\">User Name:</label><br>  <input type=\"text\" id=\"fname\" name=\"fname\"><br>  <label for=\"lname\">Password:</label><br>  <input type=\"text\" id=\"lname\" name=\"lname\"><br><br>  <input type=\"submit\" value=\"Submit\"></form> </html>";
    std::string login_success ="<!DOCTYPE html><html><body> <h2> Login Successfully </h2></body></html>   ";
    Response response = Response();
    std::string request_path;
    request_path = root_ + file;
    std::string full_path = get_extension(request_path);
    BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] preparing full path: " + full_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    // if (!is)
    // {
    //     response = ResponseGenerator::stock_response(Response::not_found);
    //     return response;
    // }
    response.code_ = Response::ok;
    char buf[512];
    // while (is.read(buf, sizeof(buf)).gcount() > 0)
    //     response.body_.append(buf, is.gcount());
    response.body_ = file=="index.html" ? login_form : login_success; 
    std::string content_length = std::to_string(response.body_.size());
    response.headers_["Content-Length"] = content_length;
    response.headers_["Content-Type"] = "text/html";
    return response;
}

//reference http://www.cplusplus.com/forum/general/182654/
std::set<std::string> LoginRequestHandler::extract_username(const std::string text){
    static const std::regex username_regex( "fname=(.*?)&", std::regex_constants::icase );
    return { std::sregex_token_iterator( text.begin(), text.end(), username_regex, 1 ),
             std::sregex_token_iterator{} } ;
}

std::string LoginRequestHandler::extract_password(const std::string text, int username_length){
    
    return text.substr(username_length+13);
}

Response LoginRequestHandler::handle_request(const Request &request)
{

    //first, check what is the request to see whether it is a GET or POST
    //second, replace the path with root and then add index.html if it is GET request
    //third, if it is a GET request, return the html. Othewise, perform a SQL query.
     BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] currently handling request";
    Response response = Response();
    if (request.method_ == Request::MethodEnum::GET)
    {
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] handing GET request";
        response = perpare_html_response("index.html");
    }
    else if (request.method_ == Request::MethodEnum::POST)
    {
        //TODO(DUY): Need to perform a password hashing
        //TODO(DAVID): Need to perform a SQL query
        //TODO(JOSH): check what is returning back to the server
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] handing POST request";
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] body : " + request.body_;
        std::string username = *extract_username(request.body_).begin();
        std::string password = extract_password(request.body_, username.length());
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] username : " + username;
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] password : " + password;
        bool is_valid_password = true;
        if (is_valid_password)
        {
            response = perpare_html_response("login_success.html");
        }
        else
        {
            response = perpare_html_response("login_failure.html");
        }
    }
    else
    {
        response = ResponseGenerator::stock_response(Response::bad_request);
    }

    return response;
}
