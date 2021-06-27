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
CONFIG
================================================================================*/

// class	ConfigLocation {};

class	ConfigServer {
public:
	ConfigServer() {
		// this->host = 2130706440; 10.0.3.1
		this->host = 0;
		this->host = 10 << 8;
		this->host = (this->host + 0) << 8;
		this->host = (this->host + 3) << 8;
		this->host += 1;
		this->port = 80;
	}
	~ConfigServer() {}
	int		getHost();
	int		getPort();
private:
	int		host;
	int		port;
	// std::vector<ConfigLocation>	locations;
};

int	ConfigServer::getHost() {
	return this->host;
}

int	ConfigServer::getPort() {
	return this->port;
}

class	Config {
public:
	Config() {};
	~Config() {};
	Config	&operator=(const Config &) {return *this;};
	void	parse(char *);
	std::vector<ConfigServer>	&getServers();
private:
	std::vector<ConfigServer>	servers;
}; // класс отвечающий за получение информации из конфига

/*================================================================================
REQUEST
================================================================================*/

class	Request {
public:
	Request();
	Request(Request &);
	Request	&operator=(Request &);
	~Request();
	void	parse(std::string &);
private:
	std::string															method;
	std::string															path;
	std::string															query;
	std::string															version;
	std::map<std::string, std::list<std::pair<std::string, double> > >	headers;//1 параметр мапы название хедера, 2 параметр мапы его значения и их вес
	std::string															body;
	int																	code;//код ошибки или успеха
	std::map<std::string, std::string>									cgiEnv;
}; // класс обрабатывающий запросы клиента

Request::Request() {}

Request::Request(Request &copy) {
	*this = copy;
}

Request::~Request() {}

Request	&Request::operator=(Request &copy) {
	this->method = copy.method;
	this->path = copy.path;
	this->version = copy.version;
	this->headers = copy.headers;
	this->cgiEnv = copy.cgiEnv;
	this->body = copy.body;
	this->query = copy.query;
	this->code = copy.code;
	return *this;
}

void	Request::parse(std::string &reading) {}

/*================================================================================
RESPONSE
================================================================================*/

/* Раскомитить

class	Response {}; // класс формирующий ответы клиенту

*/

/*================================================================================
SERVER
================================================================================*/

class	Server {
public:
	Server(const Server &);
	Server(ConfigServer &);
	~Server() {};
	Server	&operator=(const Server &);
	// long		getListenSock();
	// long		getAcceptSock();
	// struct	sockaddr_in		getSockaddr();
	long		accept();
	void		recv();
	int			chunked_detect(size_t &, std::string &);
	// void		send();
	class	ServerException : public BaseException {
	public:
		ServerException(std::string, int);
		ServerException(std::string, int, long &);
		virtual ~ServerException() throw ();
	};
private:
	struct	sockaddr_in		addr;
	ConfigServer			*config;
	long					listen_socket;
	long					accept_socket;
	Request					request;//для запросов на этот сервер
	std::string				reading;
	// Response				response;//для ответов с этого сервера
}; // класс содержащий информацию об одном хосте

Server::Server(ConfigServer &serv) {
	this->config = &serv;
	memset(reinterpret_cast<char*>(&this->addr), 0, sizeof(this->addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(this->config->getHost());
	addr.sin_port = htons(this->config->getPort());
	this->listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->listen_socket == -1)
		throw Server::ServerException("socket", 1);
	if (bind(this->listen_socket, reinterpret_cast<struct sockaddr *>(&this->addr), sizeof(this->addr)) == -1)
		throw Server::ServerException("bind", 2, this->listen_socket);
	if (listen(this->listen_socket, 1000) == -1)
		throw Server::ServerException("listen", 3, this->listen_socket);
}

long	Server::accept() {
	socklen_t	size = static_cast<socklen_t>(sizeof(this->addr));
	this->accept_socket = ::accept(this->listen_socket, reinterpret_cast<struct sockaddr *>(&this->addr), &size);
	if (this->accept_socket == -1)
		throw Server::ServerException("accept", 4, this->listen_socket);
	fcntl(this->accept_socket, F_SETFL, O_NONBLOCK);
	return this->accept_socket;
}

void	Server::recv() {
	char	buf[TCP_SIZE];
	memset(buf, '\0', TCP_SIZE);
	int		size = ::recv(this->accept_socket, buf, TCP_SIZE - 1, 0);
	if (size == 0)
		throw Server::ServerException("Connection was closed by client.", 0);
	if (size == -1)
		throw Server::ServerException("Read error, closing connection.", 0);
	this->reading += std::string(buf);
	std::cout << buf << std::endl;
	size_t	len = this->reading.find("\r\n\r\n");
	if (len != std::string::npos) {
		std::cout << len << std::endl;
		if (this->reading.find("Content-Length:") == std::string::npos) {
			size_t	pos = this->reading.find("Transfer-Encoding:");
			if (pos != std::string::npos && chunked_detect(pos, this->reading)) {
				if (this->reading.find("0\r\n\r\n") + 5 == this->reading.size())
					throw Server::ServerException("Finish chunk", 1);
			}
		}
		else if (this->reading.size() >= len + 4 + std::atoi(this->reading.substr(this->reading.find("Content-Length:") + 15, 10).c_str()))
			throw Server::ServerException("Full body", 1);
	}
}

int		Server::chunked_detect(size_t &pos, std::string &reading) {
	pos += 18;
	std::string str = reading.substr(pos);
	pos = skipspace(str);
	str = str.substr(pos, 7);
	std::cout << "|" << str << "|" <<  std::endl;
	if (str == "chunked") {
		return 1;
	}
	return 0;
}

// long	Server::getAcceptSock() {
// 	return this->accept_socket;
// }

// long	Server::getListenSock() {
// 	return this->listen_socket;
// }

// struct	sockaddr_in	Server::getSockaddr() {
// 	return this->addr;
// }

Server::Server(const Server &copy) {
	*this = copy;
}

Server	&Server::operator=(const Server &copy) {
	this->addr = copy.addr;
	this->config = copy.config;
	this->accept_socket = copy.accept_socket;
	this->listen_socket = copy.listen_socket;
	return *this;
}

Server::ServerException::ServerException(std::string mes, int err) : BaseException(mes, err) {}

Server::ServerException::ServerException(std::string mes, int err, long &sock) : BaseException(mes, err) {
	close(sock);
}

Server::ServerException::~ServerException() throw () {}

/*================================================================================
WEBSERV
================================================================================*/

/* Раскомитить

class	Webserv {
public:
	Webserv();
	~Webserv();
	Webserv	&operator=(const Webserv &);
	void	getConfig(char *);
	void	startServers();
	void	runServers();
private:
	Config						config;
	std::map<long, Server>		servers;
	std::map<long, Server *>	recvSockets;
	std::map<long, Server *>	respSockets;
}; // основной класс содержащий методы старта и очистки

void	Webserv::getConfig(char *pathConf) {
	this->config.parse(pathConf);
}

void	Webserv::startServers() {
	for (std::vector<ConfigServer>::iterator it = this->config.getServers().begin(); it != this->config.getServers().end(); it++) {
		Server	serv(*it);
		
		

		serv.start();
		
	}
}

*/

/*================================================================================
CGI
================================================================================*/

/* Раскомитить

class	CGI {}; // класс отвечающий за CGI

*/

int		main() {
	struct pollfd fds[1];
	char	buff[1000];


	try {
		ConfigServer	conf;
		Server	serv(conf);
		long fd = serv.accept();
		fds[0].fd = fd;
		fds[0].events = POLLIN;
		// fds[1].fd = fd;
		// fds[1].events = POLLOUT;
		std::cout << fds[0].events << std::endl;
		int		u = 0;
		while (1) {
			int ret = poll(fds, 1, 10000);
			if (ret > 0)
			{
				if (fds[0].revents == POLLIN) {
					fds[0].revents = 0;
					std::cout << "2" << std::endl;
					int retur = recv(fds[0].fd, buff, TCP_SIZE, 0);
					std::cout << std::string(buff) << std::endl;
					std::cout << "recv: " << retur << std::endl;
					fds[0].events = POLLOUT;
				}
				else if (fds[0].revents == POLLOUT) {
					u = 1;
					fds[0].revents = 0;
					// std::cerr << "3" << std::endl;
					int retur = send(fds[0].fd, "HTTP/1.1 200 OK\n\rContent-Length: 30\n\rContent-Location: /index.html\n\rContent-Type: text/html\n\rDate: Wed, 16 Jun 2021 18:47:31 GMT\n\rLast-Modified: Sat, 22 May 2021 11:12:27 GMT\n\rServer: Weebserv/1.0.0 (Unix)\n\rTransfer-Encoding: identity\n\r\n\rThis is the default index yo!\n\r", 269, 0);
					std::cout << "send: " << retur << std::endl;
					return 0;
				}
			}
			else if (ret == 0) {
				std::cout << "timeout" << std::endl;
			}
			else {
				break;
			}
			// recv(int __fd, void *__buf, size_t __n, int __flags)
		}
		// int retur;
		// if (fds[0].revents == POLLIN) {
		// 	retur = recv(fds[0].fd, buff, 65536, 0);
		// 	std::cout << buff << std::endl;
		// 	std::cout << retur << std::endl;
		// }
		// if (fds[1].revents == POLLOUT) {
		// 	retur = send(fds[1].fd, "HTTP/1.1 200 OK\n\rContent-Length: 30\n\rContent-Location: /index.html\n\rContent-Type: text/html\n\rDate: Wed, 16 Jun 2021 18:47:31 GMT\n\rLast-Modified: Sat, 22 May 2021 11:12:27 GMT\n\rServer: Weebserv/1.0.0 (Unix)\n\rTransfer-Encoding: identity\n\r\n\rThis!\n\r", 1000, 0);
		// 	std::cout << retur << std::endl;
		// }
	}
	catch (BaseException &e) {
		std::cerr << e.what() << " ; " << e.getErrorNumber() << std::endl;
	}
	return 0;
}

// int		main() {
// 	int		n = 0;
// 	int		i = 0;
// 	std::cout << "fjf" << std::endl;
// 	while (1) {
// 		std::string	dot[8] = {".       ", "..      ", "...     ", "....    ", ".....   ", "......  ", "....... ", "........"};
// 		if (n == 8 && i == 1)
// 			break;
// 		if (n == 8) {
// 			i++;
// 			n = 0;
// 		}
// 		std::cout << "\rfjfj" << dot[n++] << std::flush;
// 		usleep(1000000);
// 	}
// 	std::cout << "\rEND                  " << std::endl;
// 	return 0;
// }