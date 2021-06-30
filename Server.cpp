#include "Server.hpp"

Server::Server(ConfigServer &serv) {
	this->config = &serv;
	this->params.setMethods("GET ");
	this->params.setMethods("PUT ");
	this->params.setMethods("POST ");
	this->request.setParams(this->params);
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
	throw Server::ServerException("Full body", 1);
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

void	Server::send() {
	this->response.initialization(this->request);
	std::cout << this->response.getSizeAsk() << std::endl;
	std::cout << this->response.getAsk() << std::endl;
	int		ret = ::send(this->accept_socket, this->response.getAsk().c_str(), this->response.getSizeAsk(), 0);
}

void	Server::parseRequest() {
	this->request.parse(this->reading);
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
