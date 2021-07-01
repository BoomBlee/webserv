#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "color.hpp"

#define CLOSE_BRACKET 0
#define OPEN_BRACKET 1

void trim(std::string& src)
{
	src.erase(find_if_not(src.rbegin(), src.rend(), ::isspace).base(), src.end()); 
	src.erase(src.begin(), find_if_not(src.begin(), src.end(), ::isspace));

	src.erase(find(src.begin(), src.end(), '#'), src.end());
}

class ConfigLocation
{
private:
	bool _autoindex;
	std::string _path;
	std::string _upload_path;
	std::string _cgiPath;
	std::string _type;
	std::string _index;
	std::vector<std::string>	_Methods;
public:
	ConfigLocation() : _autoindex(true) {};
	~ConfigLocation() {};
	bool& getAutoindex() {return _autoindex;}
	std::string& getPath() {return _path;};
	std::string& getUploadPath() {return _upload_path;};
	std::string& getCgiPath() {return _cgiPath;};
	std::string& getType() {return _type;};
	std::string& getIndex() {return _index;};
	std::vector<std::string>& getMethods() {return _Methods;};
};

class ConfigServer
{
private:
	int	_host;
	int _port;
	std::string _serverName;
	std::map<std::string, ConfigLocation> _locations;
public:
	ConfigServer() {
	};
	~ConfigServer() {};
	std::map<std::string, ConfigLocation>& getLocations() {return _locations;};
	int&	getHost() {return _host;};
	int&	getPort() {return _port;};
	std::string&	getServerName() {return _serverName;};
};

class ConfigServers
{
private:
	std::vector<ConfigServer> _serv;

	void _write_methods(std::string& str, std::vector<std::string>& methods) {
		size_t pos;

		while ((pos = str.find(" ")) != std::string::npos) {
			methods.push_back(str.substr(0, pos));
			str.erase(0, pos + 1);
		}
		methods.push_back(str.substr(0));
	}

	void _write_params_loc(std::string& str, ConfigLocation& loc) {
		size_t pos;
		std::string second_str;
		std::string first_str;
		size_t delete_pos;

		if ((pos = str.find(" ")) != std::string::npos && (delete_pos = str.find(";")) != std::string::npos) {
			str.erase(delete_pos);
			first_str = str.substr(0, pos);
			second_str = str.substr(pos);
			trim(second_str);
			// params.insert(std::pair<std::string, std::string>(str.substr(0, pos), second_str));
			if (first_str == "autoindex") {
				if (second_str == "on")
					loc.getAutoindex() = true;
				else if (second_str == "off")
					loc.getAutoindex() = false;
				else
					throw std::logic_error("error config: invalid params \"" + str + "\"");
			}
			else if (first_str == "upload_path") {
				loc.getUploadPath() = second_str;
			}
			else if (first_str == "root") {
				loc.getPath() = second_str;
			}
			else if (first_str == "cgi_path") {
				loc.getCgiPath() = second_str;
			}
			else if (first_str == "type") {
				loc.getType() = second_str;
			}
			else if (first_str == "index") {
				loc.getIndex() = second_str;
			}
			else if (first_str == "cgi_extension") {
				loc.getType() = second_str;
			}
			else if (first_str == "method") {
				if (second_str.empty())
					throw std::logic_error("error config: no params method");
				else
					_write_methods(second_str, loc.getMethods());
			}
			else {
				throw std::logic_error("error config: not found params \"" + str + "\"");//
			}
		}
		else if (!str.empty()) {
			throw std::logic_error("error config: write_params \"" + str + "\"");
		}
	}

	int _hostToInt(std::string& str) {
		int ip = 0;
		size_t pos;

		if ((pos = str.find(".")) != std::string::npos) {
			ip = atoi(str.substr(0, pos).c_str()) << 8;
			str.erase(0, pos + 1);
			if ((pos = str.find(".")) != std::string::npos) {
				ip = (ip + atoi(str.substr(0, pos).c_str())) << 8;
				str.erase(0, pos + 1);
				if ((pos = str.find(".")) != std::string::npos) {
					ip = (ip + atoi(str.substr(0, pos).c_str())) << 8;
					str.erase(0, pos + 1);
					if (!str.empty()) {
						ip = (ip + atoi(str.substr(0).c_str()));
						return ip;
					}
				}
			}
		}
		throw std::logic_error("error config: invalid host \"" + str + "\"");
	}

	void _write_params_server(std::string& str, ConfigServer& server) {
		size_t pos;
		std::string second_str;
		std::string first_str;
		size_t delete_pos;

		if ((pos = str.find(" ")) != std::string::npos && (delete_pos = str.find(";")) != std::string::npos) {
			str.erase(delete_pos);
			first_str = str.substr(0, pos);
			second_str = str.substr(pos);
			trim(second_str);
			if (first_str == "port") {
				server.getPort() = atoi(second_str.c_str());
			}
			else if (first_str == "host") {
				server.getHost() = _hostToInt(second_str);
			}
			else if (first_str == "server_name") {
				server.getServerName() = second_str;
			}
			else {
				throw std::logic_error("error config: not found params \"" + str + "\"");//
			}
		}
		else if (!str.empty()) {
			throw std::logic_error("error config: write_params \"" + str + "\"");
		}
	}

	ConfigLocation _parse_location(std::ifstream& file, std::string& str) {
		ConfigLocation loc;
		size_t pos;
		bool bracket = CLOSE_BRACKET;

		if (str.find("{") != std::string::npos) {
			bracket = OPEN_BRACKET;
		}
		else {
			getline (file,str);
			trim(str);
			if ((str.find("{", 0) != std::string::npos) && bracket == CLOSE_BRACKET) {
				bracket = OPEN_BRACKET;
			}
			else {
				throw std::logic_error("error config");
			}
		}
		while (bracket == OPEN_BRACKET) {
			getline (file,str);
			trim(str);
			if (str.find("}") != std::string::npos) {
				bracket = CLOSE_BRACKET;
			}
			else if (str.find("{") != std::string::npos) {
				throw std::logic_error("error config: bracket parse_loc");
			}
			else {
				_write_params_loc(str, loc);
			}
		}
		if (bracket == OPEN_BRACKET)
			throw std::logic_error("error config:bracket");
		return loc;
	}

	ConfigServer _parse_server(std::ifstream& file, std::string& str) {
		ConfigServer serv;
		bool bracket = CLOSE_BRACKET;

		str = str.substr(6);
		trim(str);
		if (str.find("{") != std::string::npos) {
			bracket = OPEN_BRACKET;
		}
		else {
			getline (file,str);
			trim(str);
			if (str.find("{", 0) != std::string::npos) {
				bracket = OPEN_BRACKET;
			}
			else {
				throw std::logic_error("error config: no open bracket server");;
			}
		}
		while (bracket == OPEN_BRACKET) {
			if (file.eof())
				throw std::logic_error("error config: bracket parse_serv");
			getline (file,str);
			trim(str);
			if (str.find("location", 0, 8) != std::string::npos) {
				str = str.substr(8);
				trim(str);

				std::string	path = str.substr(0, str.find(" "));
				if (str.find(" ") != std::string::npos) {
					str = str.substr(str.find(" "));
				} else {
					str = str.substr(0);
				}
				trim(str);
				serv.getLocations()[path] = _parse_location(file, str);
			}
			else if (str == "}") {
				bracket = CLOSE_BRACKET;
			}
			else {
				_write_params_server(str, serv);
			}
		}
		if (bracket == OPEN_BRACKET)
			throw std::logic_error("error config:bracket");
		return serv;
	}

public:
	ConfigServers() {}
	ConfigServers(std::string fileName) {add(fileName);}
	void	add(std::string fileName) {
		std::string str;
		std::ifstream file(fileName);

		if (file.is_open())
		{
			while (!file.eof())
			{
				getline (file,str);
				trim(str);
				if (str.find("server", 0, 6) != std::string::npos && (!str[6] || isspace(str[6]))) {
					try {
						this->_serv.push_back(_parse_server(file, str));
					}
					catch (const std::exception& e) {
						std::cerr << RED << e.what() << RESET << std::endl;
					}
				}
			}
			file.close();
		}
	}
	~ConfigServers() {};
};

int main() {
	try {
		ConfigServers servers("example.conf");
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}