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

#include <map>
#include <list>
#include <utility>
#include <vector>
#include <stack> 


class	ConfigServer{};

//================================================================================================================================================
//================================================================================================================================================
//================================================================================================================================================

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

BaseError::BaseError(std::string &message, int &error) : std::exception(), numError(error), message(message) {}

BaseError::~BaseError() throw () {}

const char	*BaseError::what() const throw() {
	return this->message.c_str();
}

const int	&BaseError::getErrorNumber() const throw() {
	return this->numError;
}

//================================================================================================================================================
//================================================================================================================================================
//================================================================================================================================================

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


std::vector<std::string>	&ConstatsParametrs::getMethods() {
	return this->methods;
}

void	ConstatsParametrs::setMethods(std::string method) {
	this->methods.push_back(method);
}

//================================================================================================================================================
//================================================================================================================================================
//================================================================================================================================================



class	Request {
public:
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

	std::string	getMethod();
	std::string	getPath();
	std::string	getQuery();
	std::string	getVersion();
	std::map<std::string, std::list<std::pair<std::string, double> > >	getHeaders();
	std::string	getBody();
	std::string	getStatus();
	int			getStatusCode();
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

Request::Request(ConstatsParametrs &params) : params(&params) {}

Request::Request(Request &copy) {
	*this = copy;
}

Request::~Request() {}

Request	&Request::operator=(Request &copy) {
	this->method = copy.method;
	this->path = copy.path;
	this->version = copy.version;
	this->headers = copy.headers;
	// this->cgiEnv = copy.cgiEnv;
	this->body = copy.body;
	this->query = copy.query;
	this->code = copy.code;
	this->status = copy.status;
	this->params = copy.params;
	return *this;
}

void	Request::parse(std::string &reading) {
	try {
		this->setMethod(this->getNewLine(reading));
		this->setHeaders(reading);
		this->setBody(reading);
		throw Request::RequestError("OK", 200);
	}
	catch (BaseError &e) {
		this->code = e.getErrorNumber();
		this->status = std::string(e.what());
	}
	// this->setCGIEnv();
	std::cout << this->method << std::endl;
	std::cout << this->path << std::endl;
	std::cout << this->query << std::endl;
	std::cout << this->version << std::endl;
	// std::cout << this->headers.begin()->first << std::endl;
	// std::cout << this->headers.begin()->second.begin()->first << std::endl;
	std::cout << this->body << std::endl;
	std::cout << this->status << std::endl;
	std::cout << this->code << std::endl;
}

std::string	Request::getNewLine(std::string &str) {
	size_t	pos = str.find("\r\n");
	std::string	line = str.substr(0, pos);
	str = str.substr(pos + 2);
	this->validator(line);
	return line;
}

void	Request::validator(std::string &str) {
	std::string::iterator it = str.begin();
	for (; it != str.end(); it++) {
		if ((it == str.begin() && *it == ' ') || (*it != ' ' && std::isspace(*it)))
			throw Request::RequestError("Bad Request", 400);
	}
	if (it != str.end())
		throw Request::RequestError("Bad Request", 400);
}

void	Request::setMethod(std::string str) {
	for (std::vector<std::string>::iterator	it = this->params->getMethods().begin(); it != this->params->getMethods().end(); it++) {
		size_t	pos = str.find(*it);
		if (pos == 0) {
			this->method = str.substr(0, (*it).size() - 1);
			str = str.substr((*it).size());
			this->setPath(str);
			return ;
		}
	}
	throw Request::RequestError("Method Not Allowed", 405);
}

size_t	skipspace(std::string &str) {
	std::string::iterator it = str.begin();
	for (; *it == ' '; it++) {}
	std::string		str1(it, str.end());
	return it - str.begin();
}

size_t	reverseskipspace(std::string &str) {
	std::string::reverse_iterator it = str.rbegin();
	for (; *it == ' '; it++) {}
	std::string		str1(it, str.rend());
	return it - str.rbegin();
}

size_t	skiptospace(std::string &str, size_t &zero) {
	std::string::iterator it = str.begin() + zero;
	for (; *it != ' '; it++) {}
	return it - str.begin() - zero;
}

void	Request::setPath(std::string &str) {
	size_t	pos = skipspace(str);
	size_t	len = str.find_first_of('?', pos);
	if (len == std::string::npos)
		len = str.find_first_of(' ', pos);
	len -= pos;
	this->path = str.substr(pos, len);
	str = str.substr(pos + len);
	this->setQuery(str);
	this->setVersion(str);
}

void		Request::setQuery(std::string &str) {
	if (str[0] != '?')
		return;
	size_t	len = str.find_first_of(' ', 1) - 1;
	this->query = str.substr(1, len);
	str = str.substr(len + 1);
}

void	Request::setVersion(std::string &str) {
	size_t	pos = skipspace(str);
	size_t	len = str.find_first_of(' ', pos) - pos;
	this->version = str.substr(pos, len);
	if (this->version != "HTTP/1.1")
		throw Request::RequestError("HTTP Version Not Supported", 505);
}

void	Request::setHeaders(std::string &str) {
	for (std::string str1 = this->getNewLine(str); str1 != ""; str1 = this->getNewLine(str)) {
		std::string	methodName = this->getMethod(str1);
		this->headers[methodName] = this->getValue(str1);
	}
}

std::string Request::getMethod(std::string &str) {
	size_t pos = str.find_first_of(':');
	std::string	methodName = str.substr(0, pos);
	str = str.substr(pos + 1);
	return methodName;
}

std::list<std::pair<std::string, double> >	Request::getValue(std::string &str) {
	std::list<std::pair<std::string, double> >	value;
	int											i = 0;

	for (size_t pos = str.find_first_of(','); i == 0; pos = str.find_first_of(',')) {
		double	weight = 0.0;
		std::string	val = str.substr(0, pos);
		val.erase(0, skipspace(val));
		val.erase(val.size() - reverseskipspace(val));
		size_t	pos1;
		if ((pos1 = val.find_first_of(';')) != std::string::npos) {
			weight = static_cast<double>(atof(val.substr(pos1 + 3).c_str()));
			val = val.substr(0, pos1);
		}
		value.push_back(std::make_pair(val, weight));
		if (pos != std::string::npos)
			str = str.substr(pos + 1);
		if (pos == std::string::npos)
			i++;
	}
	return value;
}

void	Request::setBody(std::string &str) {
	std::map<std::string, std::list<std::pair<std::string, double> > >::iterator	it = this->headers.find("Transfer-Encoding");
	if (it != this->headers.end() && it->second.begin()->first == "chunked")
		this->chunkedBody(str);
	else {
		it = this->headers.find("Content-Length");
		if (it != this->headers.end()) {
			size_t	len = atoi(it->second.begin()->first.c_str());
			this->body = str.substr(0, len);
		}
	}
}

void	Request::chunkedBody(std::string &str) {
	long	i = 0;
	long	len = strtol(str.c_str(), NULL, 16);
	while (len != 0) {
		i = str.find("\r\n", i) + 2;
		this->body += str.substr(i, len);
		i += len + 2;
		len = strtol(&str.c_str()[i], NULL, 16);
	}
}

std::string	Request::getMethod() {
	return this->method;
}

std::string	Request::getPath() {
	return this->path;
}

std::string	Request::getQuery() {
	return this->query;
}

std::string	Request::getVersion() {
	return this->version;
}

std::map<std::string, std::list<std::pair<std::string, double> > >	Request::getHeaders() {
	return this->headers;
}

std::string	Request::getBody() {
	return this->body;
}

std::string	Request::getStatus() {
	return this->status;
}

int	Request::getStatusCode() {
	return this->code;
}

// std::map<std::string, std::string>	Request::getCGIEnv() {
// 	return this->cgiEnv;
// }

Request::RequestError::RequestError(std::string status, int numerr) : BaseError(status, numerr) {}

int		main() {
	ConstatsParametrs	par;
	Request				req(par);

	par.setMethods("GET ");
	par.setMethods("PUT ");
	par.setMethods("POST ");


	std::string	str = "POST         /?fjhf HTTP/1.1\r\nHost: httpbin.org;q=1.8, kkiu;q=10        \r\nPort: 8080\r\nTransfer-Encoding: chunked\r\n\r\n5\r\nhyujk\r\n3\r\nyty\r\n0\r\n\r\n";
	req.parse(str);
	return 0;
}
