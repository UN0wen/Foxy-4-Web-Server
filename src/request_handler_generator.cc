#include "request_handler_generator.h"
#include <boost/log/trivial.hpp>
#include "static_request_handler.h"
#include "echo_request_handler.h"

RequestHandlerGenerator::RequestHandlerGenerator() {}

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

bool RequestHandlerGenerator::invalid_config(std::vector<std::string>::iterator path,
                                 			 std::vector<std::string>::iterator root,
                                			 std::vector<std::string>::iterator method,
                                			 const auto &statement)
{
    if (statement->tokens_.size() != 2 ||
        path == std::prev(statement->tokens_.end()) ||
        root == std::prev(statement->tokens_.end()) ||
        method == std::prev(statement->tokens_.end()))
        return true;
    return false;
}

bool RequestHandlerGenerator::get_map(NginxConfig* config)
{
	for (const auto &statement : config->statements_)
	{
		std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
															statement->tokens_.end(),
															"server");
		if (find != statement->tokens_.end())
		{
			std::string path, root, method = "";
			for (const auto &s : statement->child_block_->statements_)
			{
				std::vector<std::string>::iterator find_path = std::find(s->tokens_.begin(),
																		 s->tokens_.end(),
																		 "path");
				std::vector<std::string>::iterator find_root = std::find(s->tokens_.begin(),
																		 s->tokens_.end(),
																		 "root");
				std::vector<std::string>::iterator find_method = std::find(s->tokens_.begin(),
																		   s->tokens_.end(),
																		   "method");

				if(invalid_config(find_path, find_root, find_method, s))
				{
					return false;
				}
        		//assign values if keywords are found
				if (find_path != s->tokens_.end())
				{
					path = *(find_path + 1);
				}

				if (find_root != s->tokens_.end())
				{
					root = *(find_root + 1);
				}
				if (find_method != s->tokens_.end())
				{
					method = *(find_method + 1);
				}
			}

			if (method == "static" &&
				!root.empty() && !path.empty())
			{
				map_.insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, new StaticRequestHandler(root, path)));
			}
			else if (method == "echo" &&
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
