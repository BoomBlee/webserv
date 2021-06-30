#include "Request.hpp"

Request::Request(ConstatsParametrs &params) : params(&params) {}

Request::Request(Request &copy) {
	*this = copy;
}

Request::~Request() {}

void	Request::setParams(ConstatsParametrs &par) {
	this->params = &par;
}

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
	std::cout << reading << std::endl;
	try {
		this->setMethod(this->getNewLine(reading));
		this->setHeaders(reading);
		this->setBody(reading);
		throw Request::RequestError("OK", 200);
	}
	catch (BaseException &e) {
		this->code = e.getErrorNumber();
		this->status = std::string(e.what());
	}
	// this->setCGIEnv();
	std::cout << "Method: " << this->method << "|" << std::endl;
	std::cout << "Path: " << this->path  << "|" << std::endl;
	std::cout << "Query: " << this->query  << "|" << std::endl;
	std::cout << "Version: " << this->version  << "|" << std::endl;
	std::cout << "Headers:" << std::endl;
	for (std::map<std::string, std::list<std::pair<std::string, double> > >::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
		std::cout << it->first << "=";
		for (std::list<std::pair<std::string, double> >::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1) {
			std::cout << it1->first << ";";
		}
		std::cout << std::endl;
	}
	std::cout << "\n|||||" << std::endl;
	std::cout << "Body:\n" << this->body  << "\n||||" << std::endl;
	std::cout << "Status: " << this->status  << "|" << std::endl;
	std::cout << "Code: " << this->code  << "|" << std::endl;
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

void	Request::setPath(std::string &str) {
	size_t	pos = skipspace(str);
	size_t	len = str.find_first_of('?', pos);
	if (len == std::string::npos)
		len = str.find_first_of(' ', pos);
	len -= pos;
	this->path = str.substr(pos, len);
	for (std::map<std::string, ConfigLocation>::iterator it = this->conf.getLocations().begin(); it != this->conf.getLocations().end(); ++it) {
		if (this->path.find(it->first) == 0)
			this->path = it->second.getPath() + this->path.substr(it->first.size());
	}
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
	std::string str1;
	this->headers.clear();
	for (str1 = this->getNewLine(str); str1 != ""; str1 = this->getNewLine(str)) {
		std::string	methodName = this->getMethod(str1);
		std::list<std::pair<std::string, double> >	value = this->getValue(str1);
		this->headers[methodName] = value;
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

std::string	&Request::getMethod() {
	return this->method;
}

std::string	&Request::getPath() {
	return this->path;
}

std::string	&Request::getQuery() {
	return this->query;
}

std::string	&Request::getVersion() {
	return this->version;
}

std::map<std::string, std::list<std::pair<std::string, double> > >	&Request::getHeaders() {
	return this->headers;
}

std::string	&Request::getBody() {
	return this->body;
}

std::string	&Request::getStatus() {
	return this->status;
}

int	&Request::getStatusCode() {
	return this->code;
}

Request::RequestError::RequestError(std::string status, int numerr) : BaseException(status, numerr) {}
