#include "Request.hpp"
#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

extern cmalt::ConstantsParametrs	params;

//================================================================================
namespace cmalt {
//================================================================================

Request::Request() {}

Request::Request(const Request &copy) {
	*this = copy;
}

Request::~Request() {
	this->clear();
}

Request	&Request::operator=(const Request &copy) {
	this->newLine = copy.newLine;
	this->pos = copy.pos;
	this->method = copy.method;
	this->path = copy.path;
	this->fullPath = copy.fullPath;
	this->query = copy.query;
	this->version = copy.version;
	this->headers = copy.headers;
	this->body = copy.body;
	this->code = copy.code;
	this->status = copy.status;
	this->conf = copy.conf;
	this->location = copy.location;
	this->locPath = copy.locPath;
	return *this;
}

void																Request::clear() {
	this->newLine.clear();
	this->pos = 0;
	this->method.clear();
	this->path.clear();
	this->fullPath.clear();
	this->query.clear();
	this->version.clear();
	this->headers.clear();
	this->body.clear();
	this->status.clear();
	this->code = 0;
}

void																Request::setConfig(kyoko::ConfigServer &conf) {
	this->conf = conf;
}

void																Request::parse(std::string &str) {
	this->clear();
	try {
		this->setHead(str);
		this->setHeaders(str);
		this->setBody(str);
		this->findSecretsHeaders();
		throw BaseException("OK", 200);
	}
	catch (BaseException &e) {
		this->code = e.getErrorNumber();
		this->status = std::string(e.what());
	}
	// std::cout << "Method: " << this->method << "|" << std::endl;
	// std::cout << "Path: " << this->path << "|" << std::endl;
	// std::cout << "FullPath: " << this->fullPath << "|" << std::endl;
	// std::cout << "Query: " << this->query << "|" << std::endl;
	// std::cout << "Version: " << this->version << "|" << std::endl;
	// std::cout << "Status: " << this->status << "|" << this->code << "|" << std::endl;
	// int	i = 0;
	// for (std::map<std::string, std::list<std::pair<std::string, double> > >::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
	// 	std::cout << "Header" << ++i << ": " << it->first << "=";
	// 	for (std::list<std::pair<std::string, double> >::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1) {
	// 		std::cout << it1->first << ":q=" << it1->second << ";";
	// 	}
	// 	std::cout << std::endl;
	// }
	// std::cout << "Body:\n|=====================================|\n" << this->body << "\n|==============================================|" << std::endl;
}

void									Request::getNewLine(std::string &str) {
	size_t	pos = str.find("\r\n", this->pos);
	this->newLine = (pos != std::string::npos) ? str.substr(this->pos, pos - this->pos) : std::string("");
	this->pos = (pos != std::string::npos) ? pos + 2 : pos;
	std::string::iterator it = this->newLine.begin();
	for (; it != this->newLine.end(); ++it) {
		if ((it == this->newLine.begin() && *it == ' ') || (*it != ' ' && std::isspace(*it)))
			throw BaseException("Bad Request2", 400);
	}
	if (it != this->newLine.end())
		throw BaseException("Bad Request1", 400);
}

void																Request::setHead(std::string &str) {
	size_t	pos = 0;
	this->getNewLine(str);
	this->setMethod(pos);
	this->setPath(pos);
	this->setFullPath();
	this->setVersion(pos);
}

void																Request::setMethod(size_t &pos) {
	for (std::vector<std::string>::iterator it = ::params.getMethods().begin(); it != ::params.getMethods().end(); ++it) {
		pos = this->newLine.find(*it);
		if (pos == 0) {
			this->method = this->newLine.substr(0, it->size() - 1);
			pos += it->size() + skipspace(this->newLine, it->size());
			return;
		}
	}
	throw BaseException("Method Not Allowed", 405);
}

void																Request::setPath(size_t &pos) {
	size_t	len = this->newLine.find_first_of('?', pos);
	size_t	pos2 = 0;
	if (len == std::string::npos)
		len = this->newLine.find_first_of(' ', pos);
	else
		pos2 = this->setQuery(len);
	this->path = this->newLine.substr(pos, len - pos);
	pos = len + pos2;
}

void																Request::setFullPath() {
	size_t	lenBegin = 0;
	size_t	lenEnd = 0;
	this->fullPath = "None";
	for (std::map<std::string, kyoko::ConfigLocation>::iterator it = this->conf.getLocations().begin(); it != this->conf.getLocations().end(); ++it) {
		std::string		begin;
		std::string		end;
		size_t			beginSize;
		size_t			endSize;
		size_t	pos = it->first.find_first_of('*');
		if (pos != std::string::npos) {
			begin = it->first.substr(0, pos);
			end = it->first.substr(pos + 1);
		}
		else
			begin = it->first;
		beginSize = begin.size();
		endSize = end.size();
		if (this->path.find(begin) == 0 && (this->path.find(end) == this->path.size() - endSize || pos == std::string::npos) && lenBegin <= beginSize && (lenEnd <= endSize || endSize < beginSize)) {
			this->fullPath.clear();
			this->fullPath = it->second.getPath() + this->path.substr(beginSize);
			lenEnd = endSize;
			lenBegin = beginSize;
			this->locPath = it->first;
			this->location = it->second;
		}
	}
}

size_t																Request::setQuery(size_t &pos) {
	size_t	len = this->newLine.find_first_of(' ', pos + 1) - pos - 1;
	this->query = this->newLine.substr(pos + 1, len);
	return 1 + len;
}

void																Request::setVersion(size_t &pos) {
	pos += skipspace(this->newLine, pos);
	size_t	len = this->newLine.find_first_of(' ', pos) - pos;
	this->version = this->newLine.substr(pos, len);
	if (this->version != "HTTP/1.1")
		throw BaseException("HTTP Version Not Supported", 505);
}

void																Request::setHeaders(std::string &str) {
	std::string	end("");
	std::string	nameHeader;
	this->getNewLine(str);
	while (this->newLine != end) {
		size_t	pos = 0;
		nameHeader = this->getHeaderName(pos);
		this->headers[nameHeader] = this->getHeaderValue(pos);
		this->getNewLine(str);
	}
}

std::string															Request::getHeaderName(size_t &pos) {
	std::string	nameHeader;
	pos = this->newLine.find_first_of(':');
	if (pos == std::string::npos)
		throw BaseException("Bad Request3", 400);
	nameHeader = this->newLine.substr(0, pos);
	pos += 1;
	return nameHeader;
}

void										Request::sort(std::list<std::pair<std::string, double> >::iterator first, std::list<std::pair<std::string, double> >::iterator last) {
	std::list<std::pair<std::string, double> >::iterator next;
	std::string	copyValue;
	double		copyWeight;
	for (; first != last; ++first) {
		next = first;
		++next;
		for (; next != last; ++next) {
			if (first->second < next->second) {
				copyValue = first->first;
				copyWeight = first->second;
				first->first = next->first;
				first->second = next->second;
				next->first = copyValue;
				next->second = copyWeight;
			}	
		}
	}
}

std::list<std::pair<std::string, double> >							Request::getHeaderValue(size_t &pos1) {
	std::list<std::pair<std::string, double> >	valuesHeader;
	bool										check = true;
	for (size_t pos2 = this->newLine.find_first_of(',', pos1); check; pos2 = this->newLine.find_first_of(',', pos1)) {
		double	weight = 1.0;
		std::string	value;
		size_t	pos3 = this->newLine.find_first_of(';', pos1);
		if (pos3 < pos2) {
			weight = static_cast<double>(atof(this->newLine.substr(pos3 + 3).c_str()));
			pos1 += skipspace(this->newLine, pos1);
			value = this->newLine.substr(pos1, pos3 - pos1);
		}
		else {
			pos1 += skipspace(this->newLine, pos1);
			pos3 = pos2 - reverseskipspace(this->newLine, pos2);
			if (pos2 == std::string::npos)
				pos3 = this->newLine.size() - reverseskipspace(this->newLine, pos2);
			value = this->newLine.substr(pos1, pos3 - pos1);
		}
		if (weight > 1.0 || weight < 0.0)
			throw BaseException("Bad Request4", 400);
		valuesHeader.push_back(std::make_pair(value, weight));
		if (pos2 != std::string::npos)
			pos1 = pos2 + 1;
		if (pos2 == std::string::npos)
			check = false;
	}
	this->sort(valuesHeader.begin(), valuesHeader.end());
	return valuesHeader;
}

void																Request::setBody(std::string &str) {
	std::map<std::string, std::list<std::pair<std::string, double> > >::iterator	it = this->headers.find("Transfer-Encoding");
	if (it != this->headers.end() && it->second.begin()->first == "chunked")
		this->chunkedBody(str);
	else {
		it = this->headers.find("Content-Length");
		if (it != this->headers.end()) {
			size_t	len = atoi(it->second.begin()->first.c_str());
			this->body = str.substr(this->pos, len);
		}
	}
}

void																Request::chunkedBody(std::string &str) {
	long	i = this->pos;
	long	len = strtol(str.substr(i).c_str(), NULL, 16);
	while (len != 0) {
		i = str.find("\r\n", i) + 2;
		this->body += str.substr(i, len);
		i += len + 2;
		len = strtol(&str.c_str()[i], NULL, 16);
	}
}

void																Request::findSecretsHeaders() {
	std::map<std::string, std::list<std::pair<std::string, double> > >::iterator	it = this->headers.begin();
	for (; it != this->headers.end(); ++it) {
		if (it->first.find("Secret") != std::string::npos) {
			std::string	name = this->getEnvName(it->first);
			this->rzhanoiHleb[name] = it->second.begin()->first;
		}
	}
}

std::string															&Request::getMethod() {
	return this->method;
}

std::string															&Request::getPath() {
	return this->path;
}

std::string															&Request::getFullPath() {
	return this->fullPath;
}

std::string															&Request::getQuery() {
	return this->query;
}

std::string															&Request::getVersion() {
	return this->version;
}

std::map<std::string, std::list<std::pair<std::string, double> > >	&Request::getHeaders() {
	return this->headers;
}

std::string															&Request::getBody() {
	return this->body;
}

int																	&Request::getCode() {
	return this->code;
}

std::string															&Request::getStatus() {
	return this->status;
}

kyoko::ConfigServer													&Request::getConfig() {
	return this->conf;
}

kyoko::ConfigLocation												&Request::getLocation() {
	return this->location;
}

std::string															&Request::getLocPath() {
	return this->locPath;
}

void																Request::setStatus(std::string newStatus) {
	this->status = newStatus;
}

void																Request::setCode(int newCode) {
	this->code = newCode;
}

//================================================================================
//end namespace cmalt
}
//================================================================================