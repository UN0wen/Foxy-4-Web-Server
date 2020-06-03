#include <password.h>

std::string Password::generate_hash(std::string password)
{
    return BCrypt::generateHash(password, WORKLOAD);
}

bool Password::check_password(std::string password, std::string hash)
{
    if (BCrypt::validatePassword(password, hash))
        return true;

    return false;
}