#include "test.hpp"


/*================================================================================
TOOLS
================================================================================*/




/*================================================================================
RESPONSE
================================================================================*/

void	Response::initialization(Request &req) {
	this->req = req;
	this->setCGIEnv();
	this->setHeaders();
	if (this->req.getMethod() == "GET")
		this->executeGET();
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
	this->cgi_env["PATH_INFO"]; // config(maybe request)
	this->cgi_env["PATH_TRANSLATED"]; // full PATH_INFO
	this->cgi_env["QUERY_STRING"] = this->req.getQuery();
	this->cgi_env["REMOTE_ADDR"]; // config (ip host)
	this->cgi_env["REMOTE_IDENT"]; // NO
	this->cgi_env["REMOTE_USER"]; // NO
	this->cgi_env["REQUEST_METHOD"] = this->req.getMethod();
	this->cgi_env["REQUEST_URI"] = this->req.getPath();
	this->cgi_env["SCRIPT_NAME"]; //config
	this->cgi_env["SERVER_NAME"]; // config
	this->cgi_env["SERVER_PORT"]; // config
	this->cgi_env["SERVER_PROTOCOL"] = this->req.getVersion();
	this->cgi_env["SERVER_SOFTWARE"] = this->req.getVersion();
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

	std::string	date = std::string(ctime(&now));
	return date;
}

int		Response::connectionAccept() {
	if (this->req.getStatusCode() > 200)
		return 0;
	return 1;
}

void	Response::executeGET() {
	this->headers["Content-Type"];
	this->headers["Content-Length"];

}

void	Response::executePOST() {
	
}

void	Response::executePUT() {
	
}


// void	Response::setHeaders() {
// 	this->headers.clear();
// 	this->headers["Accept-Ranges"] = "None";
// 	this->headers["Content-Encoding"] = "None";
// 	this->headers["Content-Language"];
// 	this->headers["Authorization"] = "None";
// 	this->headers["Expect"] = "None";
// 	this->headers["From"] = "None";
// 	this->headers["Host"] = "None"; //config (host + port)
// 	this->headers["Expect"] = "None";
// 	this->headers["Expect"] = "None";
// 	this->headers["Expect"] = "None";
// 	this->headers["Expect"] = "None";
// 	this->headers["Expect"] = "None";
// }




/*================================================================================
================================================================================*/

