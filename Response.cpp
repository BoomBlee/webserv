#include <algorithm>
// #include <asm-generic/socket.h>
#include <exception>
#include <iostream>
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
#include <vector>
#include <stack>

#define TCP_SIZE 65536//максимальный размер пакета в TCP

class Request {
};

/*================================================================================
TOOLS
================================================================================*/

size_t	skipspace(std::string &str) {
	std::string::iterator it = str.begin();
	for (; *it == ' '; it++) {}
	std::string		str1(it, str.end());
	return it - str.begin();
}

/*================================================================================
ERRORS
================================================================================*/

class	BaseException : public std::exception {
public:
	BaseException(std::string &message, int &error);
	virtual ~BaseException() throw ();
	virtual	const char *what() const throw ();
	const int			&getErrorNumber() const throw ();
private:
	int			numError;
	std::string	message;
}; // базовый класс ошибок

BaseException::BaseException(std::string &message, int &error) : std::exception(), numError(error), message(message) {}

BaseException::~BaseException() throw () {}

const char	*BaseException::what() const throw() {
	return this->message.c_str();
}

const int	&BaseException::getErrorNumber() const throw() {
	return this->numError;
}


/*================================================================================
RESPONSE
================================================================================*/

class Response
{
private:
	Request req;
	std::map<std::string, std::string> cgi_env;
	std::string ask;
public:
	Response();
	~Response();
	void	initialization(Request &);
	void	setCGIEnv();
};

Response::Response(/* args */)
{
}

Response::~Response()
{
}

void	Response::initialization(Request &req) {
	this->req = req;
	this->setCGIEnv();
}

void	Response::setCGIEnv() {
	this->cgi_env.insert(req.conf.cgiEnv.begin(), req.conf.cgiEnv.end());
	std::map<std::string, std::list<std::pair<std::string, double> > >::iterator it;
	for (it = this->req.headers.begin(); it != this->req.headers.end(); ++it) {
		if (it->first.find("Secret") != std::string::npos) {
			std::string	name = it->first.
		}
	}
}





/*================================================================================
================================================================================*/

int		main() {
	
}
