#include "request_handler_generator.h"
#include <boost/log/trivial.hpp>

RequestHandlerGenerator::RequestHandlerGenerator(std::map<std::string, std::shared_ptr<RequestHandler>> map) : map_(map) {}

int RequestHandlerGenerator::common_prefix_length(std::string a, std::string b)
{
    BOOST_LOG_TRIVIAL(warning) << "Request Parser generator is finding common prefix";  
    std::vector<std::string> v1;
    std::vector<std::string> v2;
    char s1[a.size() + 1];
    char s2[b.size() + 1];
    strcpy(s1, a.c_str());
    strcpy(s2, b.c_str());
    std::string x = "";
    for (int i = 0; i < strlen(s1); i++)
    {
        if (s1[i] == '/')
        {
            v1.push_back(x);
            x = "";
            continue;
        }
        x.push_back(s1[i]);
    }

    v1.push_back(x);
    x = "";
    for (int i = 0; i < strlen(s2); i++)
    {
        if (s2[i] == '/')
        {
            v2.push_back(x);
            x = "";
            continue;
        }
        x.push_back(s2[i]);
    }
    v2.push_back(x);
    std::vector<std::string>::iterator it_1 = v1.begin();
    std::vector<std::string>::iterator it_2 = v2.begin();
    int counter = 0;
    while (it_2 != v2.end())
    {
        if (*it_2 == *it_1)
        {
            counter++;
        }
        it_2++;
        it_1++;
    }
    return counter;
}

std::shared_ptr<RequestHandler> RequestHandlerGenerator::dispatch_handler(std::string uri)
{

    int longest_length = 0;
    std::string longest_path;
    std::string method;
    for (auto it = map_.begin(); it != map_.end(); ++it)
    {
        std::string path = it->first;
        int current_length = common_prefix_length(uri, path);
        if (longest_length < current_length)
        {
            longest_length = current_length;
            longest_path = path;
        }
        if (longest_length == current_length && path.length() < longest_path.length())
        {
            longest_path = path;
            longest_length = current_length;
        }
    }
    BOOST_LOG_TRIVIAL(warning) << "Request Parser generator find logest_path: " << longest_path;     
    return map_[longest_path];
}
