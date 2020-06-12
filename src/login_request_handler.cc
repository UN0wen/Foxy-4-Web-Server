
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <sqlite/sqlite3.h>
#include <regex>
#include <iterator>

#include "login_request_handler.h"
#include "response_generator.h"
#include "data_collector.h"
#include "utility.h"
#include "password.h"
#include "database.h"
#include "cookie.h"

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

Response LoginRequestHandler::prepare_html_response(const std::string file)
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
std::string LoginRequestHandler::extract_username(const std::string text)
{
    static const std::regex username_regex("username=(.*?)&", std::regex_constants::icase);
    std::sregex_token_iterator it(text.begin(), text.end(), username_regex, 1);
    static const std::sregex_token_iterator end;
    return it == end ? "" : std::string(*it);
}

bool LoginRequestHandler::compare_password(std::string username, std::string password)
{
    std::string hashed_password = "";
    database::db_select_pass(username, hashed_password);
    return Password::check_password(password, hashed_password);
}

bool LoginRequestHandler::signup(std::string username, std::string password)
{
    int rc = database::db_insert(username, password);
    // Duplicate detected
    BOOST_LOG_TRIVIAL(error) << rc;
    return rc != SQLITE_CONSTRAINT;
}

std::string LoginRequestHandler::extract_password(const std::string text)
{
    static const std::regex password_regex("password=(.*?)(&|$)", std::regex_constants::icase);
    std::sregex_token_iterator it(text.begin(), text.end(), password_regex, 1);
    static const std::sregex_token_iterator end;
    return it == end ? "" : std::string(*it);
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
            response = prepare_html_response("signup/index.html");
        }
        else
        {
            response = prepare_html_response("index.html");
        }
    }
    else if (request.method_ == Request::MethodEnum::POST)
    {
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Handling POST request";
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Request body: " + request.body_;

        std::string username = extract_username(request.body_);
        std::string password = extract_password(request.body_);

        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Username: " + username;
        BOOST_LOG_TRIVIAL(info) << "[LoginRequestHandler] Password: " + password;
        //Here is checking signup
        if (request.uri_.find("signup") != std::string::npos)
        {
            bool valid_signup = signup(username, password);
            if (username != "" && password != "" && valid_signup)
            {
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] [Signup] Signup successful, preparing success response.";
                response = prepare_html_response("signup/signup_success.html");
            }
            else
            {
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] [Signup] Signup failed: Duplicate username detected, preparing failure response.";
                response = prepare_html_response("signup/signup_failure.html");
            }
        }
        else
        { //Here is checking login
            bool is_valid_password = false;
            if (username != "")
                is_valid_password = compare_password(username, password);
            if (is_valid_password)
            {
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] Password matched for log in request, preparing success response.";
                
                std::string cookie = Cookie::generate_cookie(username);
                
                response = prepare_html_response("login_success.html");
                response.headers_["Set-Cookie"] = "jwt=" + cookie + "; Path=/";
            }
            else
            {
                BOOST_LOG_TRIVIAL(trace) << "[LoginRequestHandler] Password or username did not match for login request, preparing failure response.";
                response = prepare_html_response("login_failure.html");
            }
        }
    }
    else
    {
        response = ResponseGenerator::stock_response(Response::bad_request);
    }

    return response;
}
