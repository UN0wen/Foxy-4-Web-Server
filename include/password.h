#pragma once

#include <string>
#include <bcrypt/BCrypt.hpp>

class Password
{
public:
    // Create hash from password
    static std::string generate_hash(std::string password);

    // Compare password to generated hash
    static bool check_password(std::string password, std::string hash);

private:
    static const int WORKLOAD = 12;
};