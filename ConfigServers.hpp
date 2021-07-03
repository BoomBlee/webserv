#pragma once
#include "ConfigLocation.hpp"
#include "ConfigServer.hpp"
#include <iostream>
#include <fstream>
#include "color.hpp"

void trim(std::string& src);

class ConfigServers
{
private:
	std::vector<ConfigServer> _serv;
	void _write_methods(std::string& str, std::vector<std::string>& methods);
	void _write_params_loc(std::string& str, ConfigLocation& loc);
	int _hostToInt(std::string& str);
	void _write_params_server(std::string& str, ConfigServer& server);
	ConfigLocation _parse_location(std::ifstream& file, std::string& str);
	ConfigServer _parse_server(std::ifstream& file, std::string& str);

public:
	ConfigServers();
	ConfigServers(std::string fileName);
	void add(std::string fileName);
	~ConfigServers();
};
