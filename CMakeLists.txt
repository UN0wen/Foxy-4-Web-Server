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
add_library(session src/session.cc)
add_library(config_statement src/config_statement.cc)
add_library(config src/config.cc)
add_library(config_parser src/config_parser.cc)
add_library(request_parser src/request_parser.cc)
add_library(echo_request_handler src/echo_request_handler.cc)
add_library(static_request_handler src/static_request_handler.cc)
add_library(status_request_handler src/status_request_handler.cc)
add_library(request_handler_generator src/request_handler_generator.cc)
add_library(data_collector src/data_collector.cc)
add_library(mime_types src/mime_types.cc)
add_library(response_generator src/response_generator.cc)
add_library(not_found_request_handler src/not_found_request_handler.cc)
add_library(utility src/utility.cc)
add_library(proxy_request_handler src/proxy_request_handler.cc)


target_link_libraries(not_found_request_handler config response_generator data_collector)
target_link_libraries(static_request_handler Boost::log mime_types config response_generator)
target_link_libraries(config config_statement echo_request_handler static_request_handler status_request_handler)
target_link_libraries(config_parser config)
target_link_libraries(request_handler_generator Boost::log not_found_request_handler proxy_request_handler)
target_link_libraries(static_request_handler data_collector utility)
target_link_libraries(echo_request_handler data_collector)
target_link_libraries(status_request_handler data_collector)
target_link_libraries(request_parser utility)
target_link_libraries(proxy_request_handler Boost::log mime_types config response_generator data_collector utility)

# joshua: Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server server_lib session config config_parser config_statement static_request_handler status_request_handler response_generator mime_types echo_request_handler not_found_request_handler proxy_request_handler utility data_collector Boost::system request_parser request_handler_generator Boost::log_setup Boost::log)

add_executable(config_test tests/config/config_test.cc)
add_executable(config_parser_test tests/config_parser/config_parser_test.cc)
add_executable(echo_request_handler_test tests/echo_request_handler/echo_request_handler_test.cc)
add_executable(static_request_handler_test tests/static_request_handler/static_request_handler_test.cc)
add_executable(not_found_request_handler_test tests/not_found_request_handler/not_found_request_handler_test.cc)
add_executable(request_parser_test tests/request_parser/request_parser_test.cc)
add_executable(response_generator_test tests/response_generator/response_generator_test.cc)
add_executable(request_handler_generator_test tests/request_handler_generator/request_handler_generator_test.cc)

target_link_libraries(config_test config_parser gtest_main Boost::system)
target_link_libraries(config_parser_test config config_parser config_statement gtest_main Boost::system)
target_link_libraries(echo_request_handler_test echo_request_handler gtest_main Boost::system)
target_link_libraries(static_request_handler_test static_request_handler  config config_parser mime_types  gtest_main Boost::system Boost::log)
target_link_libraries(request_parser_test request_parser gtest_main Boost::system)
target_link_libraries(response_generator_test response_generator gtest_main Boost::system)
target_link_libraries(request_handler_generator_test request_handler_generator echo_request_handler static_request_handler config config_parser gtest_main Boost::system Boost::log)
target_link_libraries(not_found_request_handler_test not_found_request_handler gtest_main Boost::system)

gtest_discover_tests(config_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/config)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/config_parser)
gtest_discover_tests(echo_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/echo_request_handler)
gtest_discover_tests(static_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/static_request_handler)
gtest_discover_tests(not_found_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/not_found_request_handler)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/request_parser)
gtest_discover_tests(response_generator_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/response_generator)
gtest_discover_tests(request_handler_generator_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/request_handler_generator)
add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/integration.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/server)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server server_lib session config config_parser config_statement utility response_generator static_request_handler echo_request_handler Boost::system request_parser status_request_handler not_found_request_handler
  TESTS config config_parser_test config_statement config_test config_parser_test request_handler_generator_test request_parser_test
  response_generator_test static_request_handler_test echo_request_handler_test not_found_request_handler_test)
