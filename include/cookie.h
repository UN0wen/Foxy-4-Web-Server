#pragma once

#include <string>
#include <jwt-cpp/jwt.h>

class Cookie
{
public:
    // Create cookie based on username and current time
    static std::string generate_cookie(std::string username);

    // Verify cookie
    static bool verify_cookie(std::string cookie);

private:
    static const int TIME_TO_LIVE;
    static const std::string SECRET;
    static jwt::verifier<jwt::default_clock> verifier;
};