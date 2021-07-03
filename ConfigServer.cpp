#include "ConfigServer.hpp"

ConfigServer::ConfigServer() {}
ConfigServer::~ConfigServer() {};
std::map<std::string, ConfigLocation>& ConfigServer::getLocations() {return _locations;};
int& ConfigServer::getHost() {return _host;};
int& ConfigServer::getPort() {return _port;};
std::string& ConfigServer::getServerName() {return _serverName;};