#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "color.hpp"

#define CLOSE_BRACKET 0
#define OPEN_BRACKET 1

class Location
{
private:
	bool _autoindex;
	std::string _path;
	std::string _upload_path;
	std::string _cgiPath;
	std::string _type;
	std::string _index;
	std::vector<std::string>	_Methods;
	// std::map<std::string, std::string> params;
public:
	Location() : _autoindex(true) {};
	~Location() {};
	bool& getAutoindex() {return _autoindex;}
	std::string& getPath() {return _path;};
	std::string& getUploadPath() {return _upload_path;};
	std::string& getCgiPath() {return _cgiPath;};
	std::string& getType() {return _type;};
	std::string& getIndex() {return _index;};
	std::vector<std::string>& getMethods() {return _Methods;};
	// std::map<std::string, std::string>& getParams() {return params;};
};


class Server
{
private:
	int	_host;
	int _port;
	std::string _serverName;
	std::vector<Location> _locations;
	// std::map<std::string, std::string> params;
public:
	Server() {};
	~Server() {};
	std::vector<Location>& getLocations() {return _locations;};
	int&	getHost() {return _host;};
	int&	getPort() {return _port;};
	std::string&	getServerName() {return _serverName;};
	// std::map<std::string, std::string>& getParams() {return params;};
};

// template<class InputIt, class T>
// InputIt find(InputIt first, InputIt last, const T& value)
// {
// 	for (; first != last; ++first) {
// 		if (*first == value) {
// 			return first;
// 		}
// 	}
// 	return last;
// }

// template<class InputIt, class UnaryPredicate>
// InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q)
// {
// 	for (; first != last; ++first) {
// 		if (!q(*first)) {
// 			return first;
// 		}
// 	}
// 	return last;
// }

void trim(std::string& src)
{
	src.erase(find_if_not(src.rbegin(), src.rend(), ::isspace).base(), src.end()); 
	src.erase(src.begin(), find_if_not(src.begin(), src.end(), ::isspace));

	src.erase(find(src.begin(), src.end(), '#'), src.end());
}

void write_methods(std::string& str, std::vector<std::string>& methods) {
	
}

void write_params_loc(std::string& str, Location& loc) {
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
		else if (first_str == "cgi_path") {
			loc.getCgiPath() = second_str;
		}
		else if (first_str == "type") {
			loc.getType() = second_str;
		}
		else if (first_str == "index") {
			loc.getIndex() = second_str;
		}
		else if (first_str == "method") {
			write_methods(second_str, loc.getMethods());
		}
		else {
			throw std::logic_error("error config: not found params \"" + str + "\"");//
		}
	}
	else if (!str.empty()) {
		throw std::logic_error("error config: write_params \"" + str + "\"");
	}
}

void write_params_server(std::string& str, Server& server) {
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
		if (first_str == "port") {
			server.getPort() = atoi(second_str.c_str());
		}
		else if (first_str == "host") {
			server.getHost() = atoi(second_str.c_str());
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

Location parse_location(std::ifstream& file, std::string& str) {
	Location loc;
	size_t pos;
	bool bracket = CLOSE_BRACKET;

	if ((pos = str.find(" ")) != std::string::npos) {
		loc.getPath().insert(0, str, 0, pos);
		str = str.substr(pos);
		trim(str);
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
	}
	else {
		loc.getPath().insert(0, str);
		getline (file,str);
		trim(str);
		if (str.find("{", 0) != std::string::npos) {
			bracket = OPEN_BRACKET;
		}
		else {
			throw std::logic_error("error config: no open bracket location");
		}
	}
	while (bracket == OPEN_BRACKET) {
		// if (file.eof())
		// 	throw std::logic_error("error config: bracket parse_loc");
		getline (file,str);
		trim(str);
		if (str.find("}") != std::string::npos) {
			bracket = CLOSE_BRACKET;
		}
		else if (str.find("{") != std::string::npos) {
			throw std::logic_error("error config: bracket parse_loc");
		}
		else {
			write_params_loc(str, loc);
		}
	}
	if (bracket == OPEN_BRACKET)
		throw std::logic_error("error config:bracket");
	return loc;
}

Server parse_server(std::ifstream& file, std::string& str) {
	Server serv;
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
			serv.getLocations().push_back(parse_location(file, str));
			// std::cout << "123 " << str << std::endl;
		}
		// else if (str.find("}") != std::string::npos) {
		else if (str == "}") {
			bracket = CLOSE_BRACKET;
		}
		else {
			//search server_name;
			write_params_server(str, serv);
		}
	}
	if (bracket == OPEN_BRACKET)
		throw std::logic_error("error config:bracket");
	return serv;
}

// void print_params(std::vector<Server> &config) {
// 	for (size_t i=0; i < config.size(); ++i) {
// 		std::cout << BLUE << i << RESET << std::endl;
// 		for (std::map<std::string, std::string>::iterator it=config[i].getParams().begin(); it != config[i].getParams().end(); ++it) {
// 			std::cout << it->first << "=\"" << it->second << "\"" << std::endl;
// 		}
// 		for (size_t k=0; k < config[i].getLocations().size(); ++k) {
// 			std::cout << YELLOW << "location " << RESET << config[i].getLocations()[k].getPath() << std::endl;
// 			for (std::map<std::string, std::string>::iterator it=config[i].getLocations()[k].getParams().begin(); it != config[i].getLocations()[k].getParams().end(); ++it) {
// 				std::cout << it->first << "=\"" << it->second << "\"" << std::endl;
// 			}
// 		}
// 	}
// }

int main() {
	std::string str;
	std::ifstream file("example.conf");
	std::vector<Server> config;

	if (file.is_open())
	{
		while (!file.eof())
		{
			getline (file,str);
			trim(str);
			if (str.find("server", 0, 6) != std::string::npos && (!str[6] || isspace(str[6]))) {
				try {
					config.push_back(parse_server(file, str));
				}
				catch (const std::exception& e) {
					std::cerr << RED << e.what() << RESET << std::endl;
				}
			}
		}
		file.close();
	}
	// print_params(config);
	return 0;
}
