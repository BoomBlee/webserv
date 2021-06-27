#pragma once

#include <algorithm>
// #include <asm-generic/socket.h>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <fstream>
#include <string>
#include <cstring>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdlib.h>

#include <map>
#include <list>
#include <utility>
#include <vector>
#include <stack> 

#define TCP_SIZE 65536//максимальный размер пакета в TCP

class	ConfigServer{};

size_t	skipspace(std::string &str);

class	BaseError : public std::exception {
public:
	BaseError(std::string &message, int &error);
	virtual ~BaseError() throw ();
	virtual	const char *what() const throw ();
	const int			&getErrorNumber() const throw ();
private:
	int			numError;
	std::string	message;
}; // базовый класс ошибок


class	ConstatsParametrs {
public:
	ConstatsParametrs() {}
	~ConstatsParametrs() {}
	std::vector<std::string>	&getMethods();
	void		setMethods(std::string);
private:
	std::vector<std::string>							methods;
	std::string											version;
	std::map<std::string, std::vector<std::string> >		headers;
};



class	Request {
public:
	Request() {};
	Request(ConstatsParametrs &);
	Request(Request &);
	Request	&operator=(Request &);
	~Request();
	void	parse(std::string &);
	std::string	getNewLine(std::string &);
	void	validator(std::string &);
	void	setMethod(std::string);
	void	setPath(std::string &);
	void	setQuery(std::string &);
	void	setVersion(std::string &);
	void	setHeaders(std::string &);
	void	setBody(std::string &);
	void	chunkedBody(std::string &);
	std::string	getMethod(std::string &);
	std::list<std::pair<std::string, double> >	getValue(std::string &);

	std::string	&getMethod();
	std::string	&getPath();
	std::string	&getQuery();
	std::string	&getVersion();
	std::map<std::string, std::list<std::pair<std::string, double> > >	&getHeaders();
	std::string	&getBody();
	std::string	&getStatus();
	int			&getStatusCode();
	// std::map<std::string, std::string>	getCGIEnv();

	class	RequestError : public BaseError {
	public:
		RequestError(std::string, int);
		virtual ~RequestError() throw () {};
	};
private:
	std::string															method;
	std::string															path;
	std::string															query;
	std::string															version;
	std::map<std::string, std::list<std::pair<std::string, double> > >	headers;//1 параметр мапы название хедера, 2 параметр мапы его значения и их вес
	std::string															body;
	int																	code;//код ошибки или успеха
	std::string															status;
	// std::map<std::string, std::string>									cgiEnv;
	ConstatsParametrs													*params;
	ConfigServer														conf;
}; // класс обрабатывающий запросы клиента



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
	int		connectionAccept();
};
