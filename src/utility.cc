#include "utility.h"
#include <boost/algorithm/string/predicate.hpp>

namespace utility
{
    bool is_quoted(std::string *s)
    {
        if (!boost::starts_with(*s, "\"") ||
            !boost::ends_with(*s, "\""))
        {
            return false;
        }
        s->erase(0, 1);
        s->erase(s->size() - 1);
        if (!boost::ends_with(*s, "/"))
            *s += "/";
        return true;
    }

    int get_content_length(Request &request)
    {
        int content_length = 0;
        std::string content_length_str = request.headers_["Content-Length"];
        if (content_length_str != "")
        {
            content_length = std::stoi(content_length_str);
        }
        return content_length;
    }
} // namespace utility