#include "ConfigLocation.hpp"

class	ConfigServer {
public:
	ConfigServer() {
		// this->host = 2130706440; 10.0.3.1
		this->host = 0;
		this->host = 127 << 8;
		this->host = (this->host + 0) << 8;
		this->host = (this->host + 0) << 8;
		this->host += 15;
		this->port = 9090;
	}
	~ConfigServer() {}
	int		getHost();
	int		getPort();
private:
	int		host;
	int		port;
	std::vector<std::string>	servNames;
	std::map<std::string, ConfigLocation>	locations;
};
