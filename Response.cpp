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







/*================================================================================
================================================================================*/

