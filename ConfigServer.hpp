#include "ConfigLocation.hpp"

class	ConfigServer {
public:
	ConfigServer();
	~ConfigServer() {}
	int		&getHost();
	int		&getPort();
	std::map<std::string, ConfigLocation>	&getLocations();
private:
	int		host;
	int		port;
	std::string	servName;
	std::map<std::string, ConfigLocation>	locations;
};
