# Foxy-4 Web Server

## Table of Contents
- [Foxy-4 Web Server](#foxy-4-web-server)
  - [Table of Contents](#table-of-contents)
  - [Source Code Files](#source-code-files)
  - [Getting Started](#getting-started)
    - [Building](#building)
    - [Testing](#testing)
    - [Running](#running)
    - [Config file](#config-file)
  - [How to add a request handler](#how-to-add-a-request-handler)
    - [Request Handler Interface](#request-handler-interface)
    - [Detailed steps](#detailed-steps)
      - [Adding path to configuration file](#adding-path-to-configuration-file)
      - [Registering your handler with the dispatcher and data collector](#registering-your-handler-with-the-dispatcher-and-data-collector)
        - [Dispatcher](#dispatcher)
        - [Data Collector](#data-collector)
      - [Adding implementation and tests](#adding-implementation-and-tests)
      - [Naming conventions](#naming-conventions)
    - [Useful utility methods](#useful-utility-methods)
      - [mime_types](#mimetypes)
      - [ResponseGenerator](#responsegenerator)
      - [Logging](#logging)
      - [Utility namespace](#utility-namespace)
    - [Example Request Handler Implementation](#example-request-handler-implementation)
      - [Header](#header)
      - [Definition](#definition)

## Source Code Files

```
Foxy-4 
|   README.md 
|   ./run_server.sh --- Script to start the server
|   CMakeLists.txt
|
└── config
|   |   nginx.conf --- Config file used with ./run_server.sh
|   |   nginx_relative.conf --- Config file used when running server from build dir
|
└── congif_docker
|   |   nginx.conf --- Deployment config file
|
└── data --- Static data served by the server
|   |   ...
|
└── docker --- Docker configurations
|   |   ...
|
└── include --- Header files
|   |   ...
|
└── src
    |   server_main.cc --- Main executable. Reads config and starts server based on config.
    |   config.cc --- NginxConfig class and extensions
    |   config_parser.cc --- Config parser class from Assignment 1
    |   config_statement.cc --- Config statement class from Assignment 1
    |   mime_types.cc --- containes mapping of media types to extensions
    |   request_handler_generator.cc --- generates request handlers at startup
    |   request_parser.cc --- parses HTTP requests
    |   response.h --- class to hold an HTTP response
    |   response_generator.cc --- generates response objects
    |   server.cc --- class that listens for incoming connections
    |   session.cc --- class that recieves requests and builds the appropriate response
    |   echo_request_handler.cc --- sends back an echo of the request 
    |                             *Inherits from request_handler.h
    |   static_request_handler.cc --- serves static files 
    |                             *Inherits from request_handler.h
    |   status_request_handler.cc --- fetches server usage data 
    |                             *Inherits from request_handler.h
    |   data_collector.cc --- class dedicated to storing info for status requests
```

## Getting Started

### Building

In directory `Foxy-4`:

```
mkdir build
cd build 
cmake ..
make
```

### Testing

```
ctest --output-on-failure
```
### Running

There are currently two ways to run the server locally. You can either use `./run_server.sh` in the Foxy-4 root directory:

```
./run_server.sh
```

or run the server directly from the build binary from `/build`:

```
./bin/server ../config/nginx_relative.conf
```

### Config file

In addition to the format agreed upon in class, our config format allows for an optional field `dir <relative path>`.
This allows for changing the current working directory of the executable in order to handle static files.

## How to add a request handler

### Request Handler Interface

All request handlers must implement the following RequestHandler interface:

```c++
class RequestHandler
{
public:
    // Child classes' constructor can take in any number of arguments
    RequestHandler() {}

    // Instantiate the RequestHandler with the information provided in the config file
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);

    // Takes in a Request object, and return a Response object,
    // both of which are defined in the class API
    virtual Response handle_request(const Request &request) = 0;
};
```
### Detailed steps
#### Adding path to configuration file

To register your request handler in the configuration file, your config block must have the following format:

```
location <path> <HandlerName> {
*<args>;
}
```

+ `<path>` [String]: The route that this handler will serve, enclosed in double quotes
+ `<HandleName>` [String]: The name of the handler, used to register the handler with the server
+ `<args>`: Any number of argument lines that the request handler can parse. One such example is `root <root_path>;`.

In the configuration file, all paths are assumed to be directory paths. If they are not e.g. `/static` instead of `/static/`, they should be turned into one by using `utility::is_quoted()`.

#### Registering your handler with the dispatcher and data collector

##### Dispatcher
Our request handler dispatcher class is `RequestHandlerGenerator`. In order to register your handle with the dispatcher after adding it to the config file, follow these steps:

+ In `RequestHandlerGenerator::createHandler`, add an if else block with `method == <HandleName>`, where `<HandleName>` is the name you gave the handler in our config file earlier.
+ Call `<YourHandlerClass>::Init()` with the given `path` (route location) and scoped config block `NginxConfig& config`.
+ Return a `RequestHandler*`. The pointer will be converted to a smart pointer in our dispatcher class, and the route to handler mapping will be registered as well.

##### Data Collector
You will also want to register your handler with our data collector class `DataCollector`, which is used to collect route and handler data for the `/status` route.

In order to do this, add the following in your Init() function:

```c++
DataCollector::get_instance()->add_handler(location_path, "Your Handler Name Here";
```

where `<location_path>` is the same variable that was passed to `Init()` in [the previous section](#dispatcher), and the string assigned to the data collector is the name you want your handler to show up as in our status page.

Under the hood, `DataCollector` is implemented as a singleton class that you can get an instance of by calling `DataCollector::get_instance()`.

#### Adding implementation and tests
Your source code file should be in `/src/`, and your header file should be in `/include/`. When adding unit tests, add a folder in `/tests/` with your handler file name `/tests/<name>-request-handler`, and leave your unit tests in this directory. Finally add your class to `CMakeLists.txt`, following the same procedure as laid out in the class website. 

Additionally, our header files have an include guard `#pragma once` in order to prevent including multiple times.
#### Naming conventions
Class names are in CamelCase, and function names are in snake_case. 

### Useful utility methods 
#### mime_types

```c++
mime_types::extension_to_type(std::string &extension)
```
Converts a file extension string to the corresponding Content-Type header.

#### ResponseGenerator
```c++
Response ResponseGenerator::stock_response(Response::status_code status)
```
Generate a template Response object with the provided status code and a generic HTML body with the status code string. The returned Response object contains sufficient information to be sent to the client.

#### Logging
Logging is done by calling 
```
BOOST_LOG_TRIVIAL(<log_severity>) << <log_message>
```

+ `<log_severity>`: one of trace, debug, info, warning, error, fatal
+ `<log_message>`: your log message

#### Utility namespace
`utility.h` currently contains two utility functions:

```c++
bool is_quoted(std::string* s)
```
Check if the string s is contained in quotes, useful for checking string arguments in the config file. If s is enclosed in quotes, the quotes are then removed. If s is not a directory path, it is turned into one (by adding / to the end). 


```c++
int get_content_length(Request& request)
```
Returns the value of the `content-length` header in `request` if it exists, and returns 0 otherwise.
s
### Example Request Handler Implementation

Our `StaticRequestHandler` will be used as the example in this section.

#### Header
```c++
class StaticRequestHandler : public RequestHandler
{
public:
    // Constructor override to pass in more arguments 
    StaticRequestHandler(const std::string &root, const std::string &path);

    // Default constructor 
    StaticRequestHandler() {
        root_ = "";
        path_ = "";
    }

    // API compliant Init() and handle_request() functions
    static RequestHandler* Init(const std::string& location_path, const NginxConfig& config);
    
    Response handle_request(const Request &request);

private:
    // Functions needed for the handler
    ...

    // Arguments passed in by constructor
    std::string root_;
    std::string path_;
};
```
#### Definition

```c++
StaticRequestHandler::StaticRequestHandler(const std::string &root, const std::string &path)
    : root_(root), path_(path)
{
}
```
Each StaticRequestHandler has one defined path_ (location to listen to) and root_ (location to serve on disk).

```c++
RequestHandler* StaticRequestHandler::Init(const std::string& location_path, const NginxConfig& config)
{
    DataCollector::uri_request_handler[location_path] = "Static Request Handler";
```
Initialization method returning a RequestHandler pointer. First, we register our request handler for the data collector with `uri_request_handler`.

``` c++
    for (const auto &s : config.statements_)
    {
        std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(), s->tokens_.end(), "root");
        if (find_root != s->tokens_.end())
        {
            root = *(find_root + 1);
        }
    }
``` 
This code block is used to find the "root" token in the scoped config block

```c++
    std::string root = "";

    if(!utility::is_quoted(&root))
        return nullptr;

    // Allocate on heap with new, and return a pointer
    StaticRequestHandler* static_request_handler = new StaticRequestHandler(root, location_path);
    return static_request_handler;
}
```
The `is_quoted` function is used to force all paths in config to end with `/` as well as checking that all paths in config are surrounded by quotes. Then, a new handler is allocated on the heap and returned as a pointer.

```c++
Response StaticRequestHandler::handle_request(const Request &request)
{
    Response response = Response();
    std::string request_path;

    BOOST_LOG_TRIVIAL(info) << "Request URI " << request.uri_;
```
`handle_request()` contains the majority of the handler's functionality. Logging is demonstrated here using a severity level of info.

```c++
    if (!check_request_path(request.uri_, request_path))
    {
        BOOST_LOG_TRIVIAL(error) << "Request URI contains error(s)... returning 400 response";

        // Can be generated for most common HTTP error codes
        response = ResponseGenerator::stock_response(Response::bad_request);
        return response;
    }
```
Since request handlers can have private member functions, one such function is used here to check if the request path is correct and has no URL encoding error. When the check fails, a more severe logging level is used (error), and a stock response is generated and immediately returned.
```c++
    if (!replace_path(request_path))
    {
        ...
    }

    ...

    // Fill out the response to be sent to the client.
    response.code_ = Response::ok;

    ... // fill out response.body_


    // Set the two required headers for a response:
    response.headers_["Content-Length"] = std::to_string(response.body_.size());

    response.headers_["Content-Type"] = mime_types::extension_to_type(extension);

    return response;
}
```

At the end, some of the response headers are set using the headers_ map. Content-Length (required if response have a body) and Content-Type (always required) is filled in.