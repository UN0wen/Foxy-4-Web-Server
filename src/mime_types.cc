//
// mime_types.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "mime_types.h"

namespace mime_types
{

struct mapping
{
    const char *extension;
    const char *mime_type;
} mappings[] =
    {
        {"gif", "image/gif"},
        {"htm", "text/html"},
        {"html", "text/html"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"png", "image/png"},
        {"txt", "text/plain"},
        {"zip", "application/zip"},
        {"avi", "video/x-msvideo"},
        {"csv", "text/csv"},
        {"doc", "application/msword"},
        {"docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {"gz", "application/gzip"},
        {"json", "application/json"},
        {"rar", "application/vnd.rar"},
        {"tar", "application/x-tar"},
        {"7z", "application/x-7z-compressed"}};

std::string extension_to_type(const std::string &extension)
{
    for (mapping m : mappings)
    {
        if (m.extension == extension)
        {
            return m.mime_type;
        }
    }

    return "application/octet-stream";
}

} // namespace mime_types