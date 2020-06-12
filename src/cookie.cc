#include "cookie.h"
#include "boost/log/trivial.hpp"

const int Cookie::TIME_TO_LIVE = 2 * 60 * 60; // 2 hours
const std::string Cookie::SECRET = "1uXX4cW#00&xCZPY@IQ*Lf#cAYj#45q~";
jwt::verifier<jwt::default_clock> Cookie::verifier = jwt::verify()
                                                         .allow_algorithm(jwt::algorithm::hs256{SECRET})
                                                         .with_issuer("foxy-4");

std::string Cookie::generate_cookie(std::string username)
{
    std::string token = jwt::create()
                            .set_issuer("foxy-4")
                            .set_issued_at(std::chrono::system_clock::now())
                            .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{TIME_TO_LIVE})
                            .set_payload_claim("username", jwt::claim(username))
                            .sign(jwt::algorithm::hs256{SECRET});

    return token;
}

// Verify cookie
bool Cookie::verify_cookie(std::string cookie)
{
    try
    {
        auto decoded = jwt::decode(cookie);
        verifier.verify(decoded);
    }
    catch (...)
    {
        BOOST_LOG_TRIVIAL(info) << "[Cookie] Token verification failed";
        return false;
    }

    return true;
}
