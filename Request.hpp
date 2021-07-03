#pragma once
#include "ConfigServer.hpp"
#include "ConstatsParametrs.hpp"

class	Request {
public:
	Request() {};
	Request(ConstatsParametrs &);
	Request(Request &);
	Request	&operator=(Request &);
	~Request();
	void	setParams(ConstatsParametrs &);
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
	std::string	&getFullPath();
	std::string	&getQuery();
	std::string	&getVersion();
	std::map<std::string, std::list<std::pair<std::string, double> > >	&getHeaders();
	std::string	&getBody();
	std::string	&getStatus();
	int			&getStatusCode();
	ConfigServer	&getConf();
	// std::map<std::string, std::string>	getCGIEnv();

	class	RequestError : public BaseException {
	public:
		RequestError(std::string, int);
		virtual ~RequestError() throw () {};
	};
private:
	std::string															method;
	std::string															path;
	std::string															fullPath;
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
