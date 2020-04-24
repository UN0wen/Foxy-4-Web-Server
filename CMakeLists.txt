cmake_minimum_required(VERSION 3.10.0)
project(Foxy-4)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system log_setup log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Enable Bash 
# found at https://stackoverflow.com/questions/25627336/integrate-bash-test-scripts-in-cmake
find_program(BASH_PROGRAM bash)

# joshua: Update name and srcs
add_library(server_lib src/server.cc)
add_library(reply src/reply.cc)
add_library(session src/session.cc)
add_library(config_statement src/config_statement.cc)
add_library(config src/config.cc)
add_library(config_parser src/config_parser.cc)
add_library(request_parser src/request_parser.cc)
add_library(echo_request_handler src/echo_request_handler.cc)

target_link_libraries(config config_statement)
target_link_libraries(config_parser config)


# joshua: Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server server_lib session config config_parser config_statement reply echo_request_handler Boost::system request_parser Boost::log_setup Boost::log)

add_executable(config_test tests/config/config_test.cc)
add_executable(config_parser_test tests/config_parser/config_parser_test.cc)
add_executable(echo_request_handler_test tests/echo_request_handler/echo_request_handler_test.cc)
add_executable(request_parser_test tests/request_parser/request_parser_test.cc)
add_executable(reply_message_test tests/reply_message/reply_message_test.cc)
add_executable(request_test tests/request/request_test.cc)

target_link_libraries(request_test gtest_main)
target_link_libraries(config_test config_parser gtest_main)
target_link_libraries(config_parser_test config config_parser config_statement gtest_main)
target_link_libraries(echo_request_handler_test echo_request_handler gtest_main Boost::system)
target_link_libraries(request_parser_test request_parser gtest_main Boost::system)
target_link_libraries(reply_message_test reply gtest_main Boost::system)

gtest_discover_tests(config_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/config)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/config_parser)
gtest_discover_tests(echo_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/echo_request_handler)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/request_parser)
gtest_discover_tests(reply_message_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/reply_message)
gtest_discover_tests(request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/request)
add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/integration.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server server_lib session config config_parser config_statement reply echo_request_handler Boost::system request_parser
  TESTS config config_parser_test config_statement config_test config_parser_test request_parser_test reply_message_test echo_request_handler_test request_test)
