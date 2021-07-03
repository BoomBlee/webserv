#pragma once
#include "Request.hpp"

class Response
{
private:
	Request req;
	std::map<std::string, std::string> cgi_env;
	std::map<std::string, std::string> headers;
	std::string ask;
public:
	Response() {};
	~Response() {};
	void	initialization(Request &);
	void	setCGIEnv();
	void	setHeaders();
	void	executeGET();
	void	executePOST();
	void	executePUT();
	std::string	getDate();
	std::string	&getAsk();
	size_t		getSizeAsk();
	int		connectionAccept();
};
