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
	ConfigServer() {};
	ConfigServer(const ConfigServer &copy) {*this = copy;}
	~ConfigServer() {};
	ConfigServer	&operator=(const ConfigServer &copy) {
		this->_host = copy._host;
		this->_port = copy._port;
		this->_serverName = copy._serverName;
		this->_locations = copy._locations;
		return *this;
	}
	std::map<std::string, ConfigLocation>& getLocations() {return _locations;};
	int&	getHost() {return _host;};
	int&	getPort() {return _port;};
	std::string&	getServerName() {return _serverName;};
};

//================================================================================
//end namespace kyoko
}
//================================================================================