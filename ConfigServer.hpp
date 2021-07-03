#pragma once
#include <string>
#include <map>
#include "ConfigLocation.hpp"

class ConfigServer
{
private:
	int	_host;
	int _port;
	std::string _serverName;
	std::map<std::string, ConfigLocation> _locations;
public:
	ConfigServer();
	~ConfigServer();
	std::map<std::string, ConfigLocation>& getLocations();
	int&	getHost();
	int&	getPort();
	std::string&	getServerName();
};