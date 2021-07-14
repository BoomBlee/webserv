#include "Response.hpp"
#include "color.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <sys/stat.h>
#include <vector>

extern	cmalt::ConstantsParametrs	params;
extern	cmalt_kyoko::CGI			cgi;

//================================================================================
namespace cmalt {
//================================================================================

#define ERROR_PATH_400 "./YoupiBanane/errors/error_404.htm"
#define ERROR_PATH_404 "./YoupiBanane/errors/error_404.htm"
#define ERROR_PATH_405 "./YoupiBanane/errors/error_404.htm"
#define ERROR_PATH_500 "./YoupiBanane/errors/error_500.htm"
#define ERROR_PATH_505 "./YoupiBanane/errors/error_505.htm"

Response::Response() {
	this->setMethodFunctions();
	this->setErrorsPath();
}

Response::Response(const Response &copy) {
	*this = copy;
}

Response::~Response() {}

Response	&Response::operator=(const Response &copy) {
	this->ask = copy.ask;
	this->path = copy.path;
	this->fullPath = copy.fullPath;
	this->type = copy.type;
	this->conf = copy.conf;
	this->headers = copy.headers;
	this->req = copy.req;
	this->errorsPath = copy.errorsPath;
	this->methodFunctions = copy.methodFunctions;
	return *this;
}

void								Response::initialisation(Request &request) {
	this->clear();
	this->setRequest(request);
	this->setConfig(this->req.getLocation());
	this->setPath(this->req.getPath());
	this->setType(this->path);
	try {
		if (this->req.getCode() != 200)
			throw BaseException(this->req.getStatus(), this->req.getCode());
		if (this->conf.getRedirect() != "")
			throw BaseException("Moved Permanently", 301);
		vecFind(this->conf.getMethods().begin(), this->conf.getMethods().end(), this->req.getMethod(), 405, "Method Not Allowed");
		if (this->conf.getBodySize() != 0 && this->conf.getBodySize() < this->req.getBody().size())
			throw BaseException("Payload Too Large", 413);
		(this->*Response::methodFunctions[this->req.getMethod()])();
	}
	catch (BaseException &e) {
		this->req.setCode(e.getErrorNumber());
		this->req.setStatus(e.what());
		if (this->errorsPath[e.getErrorNumber()] != "")
			this->setPath(this->errorsPath[e.getErrorNumber()].substr(13));
		else
		 	this->setPath("/");
		this->setType(this->path);
		this->getErrorHtml();
		this->setHeaders();
		this->ask = this->askStatus() + this->askHeaders() + std::string("\r\n") + this->ask;
	}
	this->closeFile();
}

void								Response::clear() {
	this->ask.clear();
	this->type.clear();
	this->headers.clear();
	this->path.clear();
	this->closeFile();
}

void								Response::setConfig(kyoko::ConfigLocation &location) {
	this->conf = location;
}

void								Response::setPath(std::string path) {
	this->path = path;
	this->fullPath = this->req.getFullPath();
	if (this->stat(this->fullPath.c_str()) == 1 && this->conf.getIndex() != "")
		this->fullPath += "/" + this->conf.getIndex();
}

void								Response::setType(std::string &path) {
	size_t	pos1 = path.find_last_of('/');
	size_t	pos2 = path.find_last_of('.');
	if (pos1 == std::string::npos)
		pos1 = 0;
	if (pos2 > pos1)
		this->type = path.substr(pos2 + 1);
	if (::params.getTypes()[this->type] != "")
		this->type = ::params.getTypes()[this->type];
	else if (this->conf.getAutoindex())
		this->type = "text/html";
	else
		this->type = "text/plain";
}

void								Response::setHeaders() {
	if (this->req.getCode() == 405)
		this->setAllow();
	this->headers["Content-Length"] = std::to_string(this->ask.size());
	this->headers["Content-Type"] = this->type;
	this->headers["Content-Location"] = this->path;
	if (this->conf.getRedirect() != "")
		this->headers["Location"] = this->conf.getRedirect();
	else
		this->headers["Location"] = this->path;
	this->headers["Server"] = std::string("WebServ/1.1");
	this->headers["Date"] = this->getDate();
}

void								Response::setAllow() {
	std::vector<std::string>::iterator it = this->conf.getMethods().begin();
	while (it != this->conf.getMethods().end()) {
		this->headers["Allow"] += *(it++);
		if (it != this->conf.getMethods().end())
			this->headers["Allow"] += ", ";
	}
}

std::string							Response::getDate() {
	std::string		date;
	char			buf[100];
	struct timeval	tv;
	struct tm		*tm;

	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	strftime(buf, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
	date = std::string(buf);
	return date;
}

void								Response::setRequest(Request &request) {
	this->req = request;
	this->deleteDoubleSlash(this->req.getFullPath());
	this->deleteDoubleSlash(this->req.getLocPath());
	this->deleteDoubleSlash(this->req.getPath());
}

void								Response::openFile(std::string &path, std::ios_base::openmode mode) {
	if (mode != (std::fstream::out | std::fstream::trunc) && this->stat(path.c_str()) != 0)
		throw BaseException("Not Found", 404);
	this->file.open(path, mode);
	if (!this->file.is_open())
		throw BaseException("Forbidden", 403);
}

int									Response::stat(const char *path) {
	struct stat	st;
	if (::stat(path, &st) == 0 && st.st_mode & S_IFDIR)
		return 1;
	if (::stat(path, &st) == 0 && st.st_mode & S_IFREG)
		return 0;
	return 2;
}

void								Response::closeFile() {
	if (this->file.is_open())
		this->file.close();
}

void								Response::readFile() {
	while (!this->file.eof()) {
		std::string	buf;
		getline(this->file, buf);
		this->ask += buf + "\n";
	}
}

void								Response::writeFile(std::string &body) {
	this->file << body;
}

void								Response::setMethodFunctions() {
	this->methodFunctions["GET"] = &Response::methodGET;
	this->methodFunctions["PUT"] = &Response::methodPUT;
	this->methodFunctions["POST"] = &Response::methodPOST;
	this->methodFunctions["DELETE"] = &Response::methodDELETE;
}

void								Response::setErrorsPath() {
	this->errorsPath[400] = std::string(ERROR_PATH_400);
	this->errorsPath[404] = std::string(ERROR_PATH_404);
	this->errorsPath[405] = std::string(ERROR_PATH_405);
	this->errorsPath[500] = std::string(ERROR_PATH_500);
	this->errorsPath[505] = std::string(ERROR_PATH_505);
}

void								Response::methodGET() {
	if (this->conf.getCgiPath() != "") {
		this->cgi();
	}
	else {
		try {
			this->openFile(this->fullPath, std::fstream::in);
			this->readFile();
		}
		catch (BaseException &e) {
			if (this->conf.getAutoindex() && this->stat(this->fullPath.c_str()) == 1)
				this->ask = this->generateAutoindex(this->fullPath.c_str());
			else
				throw e;
		}
	}
	this->setHeaders();
	this->ask = this->askStatus() + this->askHeaders() + "\r\n" + this->ask;
}

void								Response::methodPUT() {
	if (this->stat(this->fullPath.c_str()) == 0) {
		this->openFile(this->fullPath, std::fstream::out);
		this->req.setCode(204);
		this->req.setStatus("No Content");
	}
	else {
		this->openFile(this->fullPath, std::fstream::out | std::fstream::trunc);
		this->req.setCode(201);
		this->req.setStatus("Created");
	}
	this->writeFile(this->req.getBody());
	this->setHeaders();
	this->ask = this->askStatus() + this->askHeaders() + "\r\n";
}

void								Response::methodPOST() {
	if (this->conf.getCgiPath() != "") {
		this->cgi();
	}
	else {
		this->req.setCode(204);
		this->req.setStatus("No Content");
	}
	this->setHeaders();
	this->ask = this->askStatus() + this->askHeaders() + "\r\n" + this->ask;
}

void								Response::methodDELETE() {
	if (this->stat(this->fullPath.c_str()) == 0) {
		if (remove(this->fullPath.c_str()) == 0) {
			this->req.setCode(204);
			this->req.setStatus("No Content");
		}
		else
			throw BaseException("Forbidden", 403);
	}
	else
		throw BaseException("Not Found", 404);
	this->setHeaders();
	this->ask = this->askStatus() + this->askHeaders() + "\r\n";
}

void								Response::cgi() {
	this->ask = ::cgi.execCGI(this->req);
	size_t	pos = 0;
	size_t	end = this->ask.size();
	while (this->ask.find("\r\n\r\n", pos) != std::string::npos || this->ask.find("\r\n", pos) == pos) {
		if (this->ask.find("Status: ", pos) == pos) {
			this->req.setCode(std::atoi(this->ask.substr(pos + 8, 3).c_str()));
			this->req.setStatus(this->ask.substr(pos + 12, this->ask.find("\r\n", pos) - (pos + 12)));
			if (this->req.getCode() == 500)
				throw BaseException("Internal Server Error", 500);
		}
		else if (this->ask.find("Content-Type: ", pos) == pos) {
			this->type = this->ask.substr(pos + 14, this->ask.find("\r\n", pos) - (pos + 14));
		}
		pos += this->ask.find("\r\n", pos) + 2 - pos;
	}
	while (this->ask.find("\r\n", end) == end)
		end -= 2;
	this->ask = this->ask.substr(pos, end - pos);
}

void								Response::deleteDoubleSlash(std::string &str) {
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		if (*it == '/' && *(it + 1) == '/')
			str.erase(it + 1);
	}
}

std::string							Response::generateAutoindex(const char *path) {
	std::string dirName(path);
    DIR *dir = opendir(path);
	std::vector<std::string>	files;
    std::string page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + dirName + "</title>\n\
    </head>\n\
    <body>\n\
    <h1>INDEX</h1>\n\
    <p>\n";

    if (dir == NULL) {
        std::cerr << RED  << RED << "Error: could not open [" << path << "]" << RESET <<RESET << std::endl;
        return "";
    }
    if (dirName[0] != '/')
        dirName = "/" + dirName;
    for (struct dirent *dirEntry = readdir(dir); dirEntry; dirEntry = readdir(dir)) {
		files.push_back(std::string(dirEntry->d_name));
    }
	size_t pos = dirName.find(this->conf.getPath());
	if (pos != std::string::npos)
		pos += this->conf.getPath().size();
	else
	 	pos = dirName.size();
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		if (*it == "." || *it == "..")
			page += "\t\t<p><a href=\"http://" + std::to_string(this->req.getConfig().getHost()) + ":" + std::to_string(this->req.getConfig().getPort()) + "/" + dirName.substr(pos) + "/" + *it + "\">" + *it + "</a></p>\n";
	}
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
		if (*it != "." && *it != "..")
			page += "\t\t<p><a href=\"http://" + std::to_string(this->req.getConfig().getHost()) + ":" + std::to_string(this->req.getConfig().getPort()) + "/" + dirName.substr(pos) + "/" + *it + "\">" + *it + "</a></p>\n";
	}
    page +="\
    </p>\n\
    </body>\n\
    </html>\n";
    closedir(dir);
    return page;
}

std::string							Response::askStatus() {
	std::string		status;

	status = "HTTP/1.1 " + std::to_string(this->req.getCode()) + " " + this->req.getStatus() + "\r\n";
	return status;
}

std::string							Response::askHeaders() {
	std::string		headers;
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it)
		headers += it->first + ": " + it->second + "\r\n";
	return headers;
}

void							Response::getErrorHtml() {
	this->closeFile();
	try {
		this->openFile(this->errorsPath[this->req.getCode()], std::fstream::in);
		this->readFile();
	}
	catch (BaseException &e) {}
}

kyoko::ConfigLocation				&Response::getConfig() {
	return this->conf;
}

std::string							&Response::getType() {
	return this->type;
}

std::string							&Response::getAsk() {
	return this->ask;
}

Request								&Response::getRequest() {
	return this->req;
}

std::map<std::string, std::string>	&Response::getHeaders() {
	return this->headers;
}

void								Response::setAsk(std::string str) {
	this->ask = str;
}

//================================================================================
//end namespace cmalt
}
//================================================================================
