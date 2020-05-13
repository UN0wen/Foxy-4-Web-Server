# Foxy-4 Web Server

## Source Code Files

```
server_main.cc --- Main executable. Reads config and starts server based on config.
config.cc --- NginxConfig class and extensions
config_parser.cc --- Config parser class from Assignment 1
config_statement.cc --- Config statement class from Assignment 1
mime_types.cc --- containes mapping of media types to extensions
request_handler_generator.cc --- generates request handlers at startup
request_parser.cc ---
response.cc ---
server.cc ---
session.cc ---
echo_request_handler.cc --- *Inherits from request_handler.h
static_request_handler.cc --- *Inherits from request_handler.h
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
make test
```
### Running

There are currently two ways to run the server locally. You can either use `./run_server.sh` in the Foxy-4 root directory, or run the server directly from the build binary from `/build`:

```
./bin/server ../config/nginx_relative.conf
```

### Config file

In addition to the format agreed upon in class, our config format allows for an optional field `dir <relative path>`.
This allows for changing the current working directory of the executable in order to handle static files.

## How to add a request handler

### Example

