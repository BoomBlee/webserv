#include "Response.hpp"

void	Response::initialization(Request &req) {
	this->req = req;
	this->setCGIEnv();
	this->setHeaders();
	// if (this->req.getMethod() == "GET")
		this->executeGET();
	if (this->req.getMethod() == "PUT")
		this->executePUT();
	if (this->req.getMethod() == "POST")
		this->executePOST();
}

void	Response::setCGIEnv() {
	this->cgi_env.clear();
	this->cgi_env["AUTH_TYPE"];
	this->cgi_env["CONTENT_LENGTH"];
	if (this->req.getHeaders().find("Content-Length") != this->req.getHeaders().end()) {
		this->cgi_env["CONTENT_LENGTH"] = this->req.getHeaders().find("Content-Length")->second.begin()->first;
	}
	this->cgi_env["CONTENT_TYPE"];
	if (this->req.getHeaders().find("Content-Type") != this->req.getHeaders().end()) {
		this->cgi_env["CONTENT_TYPE"] = this->req.getHeaders().find("Content-Type")->second.begin()->first;
	}
	this->cgi_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->cgi_env["PATH_INFO"] = this->req.getPath(); // config(maybe request)
	this->cgi_env["PATH_TRANSLATED"] = this->req.getFullPath(); // full PATH_INFO
	this->cgi_env["QUERY_STRING"] = this->req.getQuery();
	int	host = this->req.getConf().getHost();
	this->cgi_env["REMOTE_ADDR"] = std::to_string(host >> 24) + "." + std::to_string(host << 8 >> 24) + "." + std::to_string(host << 16 >> 24) + "." + std::to_string(host << 24 >> 24); // config (ip host)
	this->cgi_env["REMOTE_IDENT"]; // NO
	this->cgi_env["REMOTE_USER"]; // NO
	this->cgi_env["REQUEST_METHOD"] = this->req.getMethod();
	this->cgi_env["REQUEST_URI"] = this->req.getPath();
	this->cgi_env["SCRIPT_NAME"]; //config
	this->cgi_env["SERVER_NAME"]; // config
	this->cgi_env["SERVER_PORT"]; // config
	this->cgi_env["SERVER_PROTOCOL"] = this->req.getVersion();
	this->cgi_env["SERVER_SOFTWARE"] = this->req.getVersion();
	std::cout << "CGI variables:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = this->cgi_env.begin(); it != this->cgi_env.end(); ++it) {
		std::cout << "\t" << it->first << "=" << it->second << std::endl;
	}
	std::cout << "END CGI" << std::endl;
}


void	Response::setHeaders() {
	this->headers.clear();
	this->headers["Connection"] = this->connectionAccept() ? "keep-alive" : "close";
	this->headers["Date"] = this->getDate();
	this->headers["Location"] = "None"; // full PATH
	this->headers["Server"] = "WebServ/1.1";
}

std::string	Response::getDate() {
	time_t	now = time(0);
	std::string	str = std::string(ctime(&now));

	std::string	date = str.substr(0, str.find("\n"));
	return date;
}

int		Response::connectionAccept() {
	if (this->req.getStatusCode() > 200)
		return 0;
	return 1;
}

void	Response::executeGET() {
	// std::cout 
	std::fstream	fd((this->req.getFullPath()).c_str() , std::fstream::in);
	std::cout << "|" << this->req.getFullPath() << "|" << std::endl;
	// std::fstream	fd((std::string("./errors/error_404.html")).c_str() , std::fstream::in);
	std::string		body;
	std::string str;

	while (fd.is_open() && !fd.eof()) {
		getline(fd, str);
		body += str + "\r\n";
	}
	this->headers["Content-Type"] = "text/html";
	this->headers["Content-Length"] = std::string(std::to_string(body.size()));
	this->ask.clear();
	this->ask += this->req.getVersion() + " " + std::string(std::to_string(this->req.getStatusCode())) + " " + this->req.getStatus() + "\r\n";
	// this->ask += std::string("HTTP/1.1") + " " + std::to_string(200) + " OK\r\n";
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
		this->ask += it->first + ": " + it->second + "\r\n";
	}
	this->ask += "\r\n" + body + "\r\n";
}

void	Response::executePOST() {
	std::fstream	fd((this->req.getPath() + "").c_str() , std::fstream::out);
	fd << this->req.getBody();
	this->ask.clear();
	this->ask += this->req.getVersion() + " " + std::string(std::to_string(this->req.getStatusCode())) + " " + this->req.getStatus() + "\r\n";
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
		this->ask += it->first + ": " + it->second + "\r\n";
	}
}

void	Response::executePUT() {
	std::fstream	fd((this->req.getPath() + "").c_str() , std::fstream::out | std::fstream::trunc);
	fd << this->req.getBody();
	this->ask.clear();
	this->ask += this->req.getVersion() + " " + std::string(std::to_string(this->req.getStatusCode())) + " " + this->req.getStatus() + "\r\n";
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
		this->ask += it->first + ": " + it->second + "\r\n";
	}
}

std::string	&Response::getAsk() {
	return this->ask;
}

size_t		Response::getSizeAsk() {
	return static_cast<size_t>(this->ask.size());
}
