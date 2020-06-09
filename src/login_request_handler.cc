#include "login_request_handler.h"
#include "response_generator.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "data_collector.h"
#include "utility.h"
#include "password.h"
#include "database.h"
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

Response LoginRequestHandler::perpare_html_response(const std::string file)
{
    Response response;
    std::string request_path;
    request_path = root_ + file;
    BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] preparing full path: " + request_path;
    std::ifstream is(request_path.c_str(), std::ios::in | std::ios::binary);
    if (!is)
    {
        response = ResponseGenerator::stock_response(Response::not_found);
        return response;
    }
    response.code_ = Response::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0)
        response.body_.append(buf, is.gcount());
    std::string content_length = std::to_string(response.body_.size());
    response.headers_["Content-Length"] = content_length;
    response.headers_["Content-Type"] = "text/html";
    return response;
}

//reference http://www.cplusplus.com/forum/general/182654/
std::set<std::string> LoginRequestHandler::extract_username(const std::string text)
{
    static const std::regex username_regex("fname=(.*?)&", std::regex_constants::icase);
    return {std::sregex_token_iterator(text.begin(), text.end(), username_regex, 1),
            std::sregex_token_iterator{}};
}

bool LoginRequestHandler::compare_password(std::string username, std::string password)
{
    std::string hashed_password = database::db_select_pass(username);
    return Password::check_password(password, hashed_password);
}

std::string LoginRequestHandler::extract_password(const std::string text, int username_length)
{

    return text.substr(username_length + 13);
}

Response LoginRequestHandler::handle_request(const Request &request)
{
    //first, check what is the request to see whether it is a GET or POST
    //second, replace the path with root and then add index.html if it is GET request
    //third, if it is a GET request, return the html. Othewise, perform a SQL query.
    BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Currently handling request";
    Response response;
    if (request.method_ == Request::MethodEnum::GET)
    {
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Handling GET request";
        if (request.uri_.find("signup") != std::string::npos)
        {
            response = perpare_html_response("signup/index.html");
        }
        else
        {
            response = perpare_html_response("index.html");
        }
    }
    else if (request.method_ == Request::MethodEnum::POST)
    {
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Handling POST request";
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Request body: " + request.body_;
        std::string username = "";
        std::string password = "";
        if (!extract_username(request.body_).empty())
        {
            username = *extract_username(request.body_).begin();
            password = extract_password(request.body_, username.length());
        }
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Username: " + username;
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Password: " + password;
        //Here is checking signup
        if (request.uri_.find("signup") != std::string::npos)
        {   //TODO: CHECK DUPLICATES IN SQL
            bool is_valid_username = true;
            if(username != "" && password != "" && is_valid_username){
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] Everything passed, preparing success response.";
                response = perpare_html_response("signup/signup_success.html");
            }
            else{
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] Something failed, preparing failure response.";
                response = perpare_html_response("signup/signup_failure.html");
            }
        }
        else
        {   //Here is checking login
            bool is_valid_password = false;
            if (username != "")
                is_valid_password = compare_password(username, password);
            if (is_valid_password)
            {
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] Password matched for log in request, preparing success response.";
                // Add the cookie here
                response = perpare_html_response("login_success.html");
            }
            else
            {
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] Password or username did not match for login request, preparing failure response.";
                response = perpare_html_response("login_failure.html");
            }
        }
    }
    else
    {
        response = ResponseGenerator::stock_response(Response::bad_request);
    }

    return response;
}
