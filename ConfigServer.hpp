#pragma once
#include "ConfigLocation.hpp"

//================================================================================
namespace kyoko {
//================================================================================

class ConfigServer
{
private:
	int	_host;
	int _port;
	std::string _serverName;
	std::map<std::string, ConfigLocation> _locations;
public:
	ConfigServer();
	ConfigServer(const ConfigServer &copy);
	~ConfigServer();
	std::map<std::string, ConfigLocation>& getLocations();
	int&	getHost();
	int&	getPort();
	std::string&	getServerName();
	ConfigServer& operator=(const ConfigServer &);
};

//================================================================================
//end namespace kyoko
}
//================================================================================
