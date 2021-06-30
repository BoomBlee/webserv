#include "ConfigServer.hpp"

int	ConfigServer::getHost() {
	std::cout << this->host << std::endl;
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
