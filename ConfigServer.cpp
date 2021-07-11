#include "ConfigServer.hpp"

namespace kyoko {
ConfigServer::ConfigServer() {}
ConfigServer::~ConfigServer() {};
std::map<std::string, ConfigLocation>& ConfigServer::getLocations() {return _locations;};
int& ConfigServer::getHost() {return _host;};
int& ConfigServer::getPort() {return _port;};
std::string& ConfigServer::getServerName() {return _serverName;};
ConfigServer& ConfigServer::operator=(const ConfigServer &copy) {
	this->_host = copy._host;
	this->_port = copy._port;
	this->_serverName = copy._serverName;
	this->_locations = copy._locations;
	return *this;
}
}