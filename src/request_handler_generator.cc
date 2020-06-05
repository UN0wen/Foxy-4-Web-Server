#include <boost/log/trivial.hpp>
#include "request_handler_generator.h"
#include "static_request_handler.h"
#include "echo_request_handler.h"
#include "status_request_handler.h"
#include "not_found_request_handler.h"
#include "proxy_request_handler.h"
#include "health_request_handler.h"
#include "login_request_handler.h"
#include "utility.h"

RequestHandlerGenerator::RequestHandlerGenerator() {}

int RequestHandlerGenerator::common_prefix_length(std::string a, std::string b)
{
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
			if (&x != NULL && !x.empty())
				v1.push_back(x);
			x = "";
			continue;
		}
		x.push_back(s1[i]);
	}
	if (&x != NULL && !x.empty())
	{
		v1.push_back(x);
	}
	x = "";
	for (int i = 0; i < strlen(s2); i++)
	{
		if (s2[i] == '/')
		{
			if (&x != NULL && !x.empty())
				v2.push_back(x);
			x = "";
			continue;
		}
		x.push_back(s2[i]);
	}
	if (&x != NULL && !x.empty())
	{
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
		if (current_length == 0)
			continue;
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
	if (longest_length == 0)
	{
		return map_["/"];
	}
	BOOST_LOG_TRIVIAL(trace) << "[RequestParserGenerator] Found longest path: " << longest_path;
	return map_[longest_path];
}

bool RequestHandlerGenerator::invalid_config(const auto &statement)
{
	if (statement->tokens_.size() != 3 &&
		statement->child_block_ != nullptr)
		return true;
	return false;
}

RequestHandler *RequestHandlerGenerator::createHandler(std::string path, std::string method, NginxConfig &config)
{
	if (method == "StaticHandler")
	{
		return StaticRequestHandler::Init(path, config);
	}
	else if (method == "EchoHandler")
	{
		return EchoRequestHandler::Init(path, config);
	}
	else if (method == "StatusHandler")
	{
		return StatusRequestHandler::Init(path, config);
	}
	else if (method == "NotFoundHandler")
	{
		return NotFoundRequestHandler::Init(path, config);
	}
	else if (method == "ProxyHandler")
	{
		return ProxyRequestHandler::Init(path, config);
	}
	else if (method == "HealthHandler")
	{
		return HealthRequestHandler::Init(path, config);
	}
	else if(method == "LoginHandler")
	{
		return LoginRequestHandler::Init(path, config);
	}
	else
	{
		return nullptr;
	}
}

bool RequestHandlerGenerator::get_map(NginxConfig *config)
{
	for (const auto &statement : config->statements_)
	{
		std::vector<std::string>::iterator find = std::find(statement->tokens_.begin(),
															statement->tokens_.end(),
															"location");
		if (find == statement->tokens_.begin())
		{
			std::string path, root, method = "";
			if (invalid_config(statement))
			{
				return false;
			}
			//assign values if keywords are found
			method = *(find + 2);
			path = *(find + 1);

			if (!utility::is_quoted(&path))
				return false;

			RequestHandler *rh = createHandler(path, method, *(statement->child_block_));
			if (rh == nullptr)
			{
				return false;
			}
			else
			{
				map_.insert(std::pair<std::string, std::shared_ptr<RequestHandler>>(path, rh));
			}
		}
	}

	return true;
}

std::map<std::string, std::shared_ptr<RequestHandler>> RequestHandlerGenerator::mapping()
{
	return map_;
}
