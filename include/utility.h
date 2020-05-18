#pragma once

#include <string>
#include "request.h"

namespace utility
{
    bool is_quoted(std::string *s);

    int get_content_length(Request &request);
} // namespace utility