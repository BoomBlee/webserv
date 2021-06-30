#include "ConfigServer.hpp"

ConfigServer::ConfigServer () {
	// this->host = 2130706440; 10.0.3.1
	this->host = 0;
	this->host = 127 << 8;
	this->host = (this->host + 0) << 8;
	this->host = (this->host + 0) << 8;
	this->host += 1;
	this->port = 9090;
	this->servName = "localhost";
	ConfigLocation	loc;
	this->locations["/"] = loc;
}

std::map<std::string, ConfigLocation>	&ConfigServer::getLocations() {
	return this->locations;
}

int	&ConfigServer::getHost() {
	std::cout << this->host << std::endl;
	return this->host;
}

int	&ConfigServer::getPort() {
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
