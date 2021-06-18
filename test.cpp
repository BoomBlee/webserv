#include <algorithm>
#include <asm-generic/socket.h>
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
ERRORS
================================================================================*/

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

/*================================================================================
CONFIG
================================================================================*/

// class	ConfigLocation {};

class	ConfigServer {
public:
	ConfigServer() {this->host = 2130706440; this->port = 9090;};
	~ConfigServer() {};
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
	~Request();
	void	parse(std::string &);
private:
	std::string															method;
	std::string															path;
	std::string															version;
	std::map<std::string, std::list<std::pair<std::string, double>>>	headers;//1 параметр мапы название хедера, 1 параметр мапы его значения и их вес
	std::map<std::string, std::string>									cgiEnv;
	std::string															body;
	int																	code;//код ошибки или успеха
}; // класс обрабатывающий запросы клиента

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
	// void		send();
	class	ServerError : public BaseError {
	public:
		ServerError(std::string, int);
		ServerError(std::string, int, long &);
		virtual ~ServerError() throw ();
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
		throw Server::ServerError("socket", 1);
	if (bind(this->listen_socket, reinterpret_cast<struct sockaddr *>(&this->addr), sizeof(this->addr)) == -1)
		throw Server::ServerError("bind", 2, this->listen_socket);
	if (listen(this->listen_socket, 1000) == -1)
		throw Server::ServerError("listen", 3, this->listen_socket);
}

long	Server::accept() {
	socklen_t	size = static_cast<socklen_t>(sizeof(this->addr));
	this->accept_socket = ::accept(this->listen_socket, reinterpret_cast<struct sockaddr *>(&this->addr), &size);
	if (this->accept_socket == -1)
		throw Server::ServerError("accept", 4, this->listen_socket);
	fcntl(this->accept_socket, F_SETFL, O_NONBLOCK);
	return this->accept_socket;
}

void	Server::recv() {
	char	buf[TCP_SIZE];
	int		size = ::recv(this->accept_socket, buf, TCP_SIZE - 1, 0);
	std::string		str(buf);
	this->reading += str;
	if (this->reading.find("Content-Length: ") == std::string::npos) {
	}
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

Server::ServerError::ServerError(std::string mes, int err) : BaseError(mes, err) {}

Server::ServerError::ServerError(std::string mes, int err, long &sock) : BaseError(mes, err) {
	close(sock);
}

Server::ServerError::~ServerError() throw () {}

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
	struct pollfd fds[2];
	char	*buff;


	try {
		ConfigServer	conf;
		Server	serv(conf);
		long fd = serv.accept();
		fds[0].fd = fd;
		fds[0].events = POLLIN;
		// fds[1].fd = fd;
		// fds[1].events = POLLOUT;
		std::cout << fds[0].events << std::endl;
		while (1) {
			int ret = poll(fds, 1, 10000);
			if (ret > 0)
			{
				if (fds[0].revents == POLLIN) {
					fds[0].revents = 0;
					int retur = recv(fds[0].fd, buff, 65536, 0);
					std::cout << buff << std::endl;
					std::cout << "recv: " << retur << std::endl;
					fds[0].events = POLLOUT;
				}
				if (fds[0].revents == POLLOUT) {
					fds[0].revents = 0;
					int retur = send(fds[0].fd, "HTTP/1.1 200 OK\n\rContent-Length: 30\n\rContent-Location: /index.html\n\rContent-Type: text/html\n\rDate: Wed, 16 Jun 2021 18:47:31 GMT\n\rLast-Modified: Sat, 22 May 2021 11:12:27 GMT\n\rServer: Weebserv/1.0.0 (Unix)\n\rTransfer-Encoding: identity\n\r\n\rThis is the default index yo!\n\r", 269, 0);
					std::cout << "send: " << retur << std::endl;
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
	catch (BaseError &e) {
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