#include "Config.hpp"
#include <bits/c++config.h>
#include <cstddef>
#include <string>

//================================================================================
namespace cmalt {
//================================================================================

class	Request {
public:
	Request();
	Request(const Request &);
	~Request();
	Request																&operator=(const Request &);
	void																setConfig(kyoko::ConfigServer &);
	void																parse(std::string &);

	std::string															&getMethod();
	std::string															&getPath();
	std::string															&getFullPath();
	std::string															&getQuery();
	std::string															&getVersion();
	std::map<std::string, std::list<std::pair<std::string, double> > >	&getHeaders();
	std::string															&getBody();
	int																	&getCode();
	std::string															&getStatus();
	kyoko::ConfigServer													&getConfig();
	kyoko::ConfigLocation												&getLocation();
	std::string															&getLocPath();

	void																setStatus(std::string);
	void																setCode(int);
private:
	std::string															newLine;
	size_t																pos;
	std::string															method;
	std::string															path;
	std::string															fullPath;
	std::string															query;
	std::string															version;
	std::map<std::string, std::list<std::pair<std::string, double> > >	headers;//1 параметр мапы название хедера, 2 параметр мапы его значения и их вес
	std::string															body;
	int																	code;//код ошибки или успеха
	std::string															status;
	kyoko::ConfigServer													conf;
	kyoko::ConfigLocation												location;
	std::string															locPath;
private:
	void																clear();
	void																sort(std::list<std::pair<std::string, double> >::iterator, std::list<std::pair<std::string, double> >::iterator);
	void																getNewLine(std::string &);
	void																setHead(std::string &);
	void																setMethod(size_t &);
	void																setPath(size_t &);
	void																setFullPath();
	size_t																setQuery(size_t &);
	void																setVersion(size_t &);
	void																setHeaders(std::string &);
	std::string															getHeaderName(size_t &);
	std::list<std::pair<std::string, double> >							getHeaderValue(size_t &);
	void																setBody(std::string &);
	void																chunkedBody(std::string &);
};

//================================================================================
//end namespace cmalt
}
//================================================================================