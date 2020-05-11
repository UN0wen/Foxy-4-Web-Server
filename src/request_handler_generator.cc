#include "request_handler_generator.h"
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "static_request_handler.h"
#include "echo_request_handler.h"

RequestHandlerGenerator::RequestHandlerGenerator() {}

bool is_quoted(std::string* s)
{
	if(!boost::starts_with(*s, "\"") ||
	   !boost::ends_with(*s, "\""))
	{
		return false;
	}
	s->erase(0,1);
	s->erase(s->size()-1);
	return true;
}

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
			if(&x != NULL && !x.empty()) v1.push_back(x);
			x = "";
			continue;
		}
		x.push_back(s1[i]);
	}
    if(&x != NULL && !x.empty()){
        v1.push_back(x); 
    }
    x = "";
    for (int i = 0; i < strlen(s2); i++)
    {
        if (s2[i] == '/')
        {
            if(&x != NULL && !x.empty()) v2.push_back(x);
            x = "";
            continue;
        }
        x.push_back(s2[i]);
    }
    if(&x != NULL && !x.empty()){
      v2.push_back(x);
     }
    std::vector<std::string>::iterator it_1 = v1.begin();
    std::vector<std::string>::iterator it_2 = v2.begin();
    int counter = 0;
    while (it_2 != v2.end() && it_1 != v1.end())
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
        if(current_length == 0) continue;
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
    //TODO: if longest PATH is 0, then status handler will be taken place (Assign to whoever work on status handler)
    if(longest_length == 0){
        longest_path = "/echo/";
    } 
	BOOST_LOG_TRIVIAL(warning) << "Request Parser generator find longest_path: " << longest_path;  
	return map_[longest_path];
}

bool RequestHandlerGenerator::invalid_config(const auto &statement)
{
    if (statement->tokens_.size() != 3 &&
        statement->child_block_ != nullptr)
        return true;
    return false;
}

bool RequestHandlerGenerator::get_map(NginxConfig* config)
{
	for (const auto &statement : config->statements_)
	{
		std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
															statement->tokens_.end(),
															"location");
		if (find == statement->tokens_.begin())
		{
			std::string path, root, method = "";
			if(invalid_config(statement))
			{
				return false;
			}
    		//assign values if keywords are found
			method = *(find + 2);
			path = *(find + 1);

			if(!is_quoted(&path))
				return false;

			if (method == "StaticHandler" &&
				!path.empty())
			{
				for (const auto &s : statement->child_block_->statements_)
				{
					std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
																			 s->tokens_.end(),
																			 "root");
					if(find_root != s->tokens_.end())
					{
						root = *(find_root + 1);
					}
				}
				if(!is_quoted(&root))
				return false;
				map_.insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new StaticRequestHandler(root, path)));
			}
			else if (method == "EchoHandler" &&
				!path.empty())
			{
				map_.insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new EchoRequestHandler()));
			}
			else 
			{
				return false;
			}
		}
	}

	return true;
}

std::map<std::string, std::shared_ptr<RequestHandler>> RequestHandlerGenerator::mapping()
{
	return map_;
}
