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
	std::string path;
	std::map<std::string, std::string> params;
public:
	Location() {};
	~Location() {};
	std::string& getPath() {return path;};
	std::map<std::string, std::string>& getParams() {return params;};
};


class Server
{
private:
	std::vector<Location> locations;
	std::map<std::string, std::string> params;
public:
	Server() {};
	~Server() {};
	std::vector<Location>& getLocations() {return locations;};
	std::map<std::string, std::string>& getParams() {return params;};
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

void write_param(std::string& str, std::map<std::string, std::string>& params) {
	size_t pos;
	std::string second_str;
	size_t delete_pos;

	if ((pos = str.find(" ")) != std::string::npos && (delete_pos = str.find(";")) != std::string::npos) {
		str.erase(delete_pos);
		second_str = str.substr(pos);
		trim(second_str);
		params.insert(std::pair<std::string, std::string>(str.substr(0, pos), second_str));
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
			write_param(str, loc.getParams());
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
			write_param(str, serv.getParams());
		}
	}
	if (bracket == OPEN_BRACKET)
		throw std::logic_error("error config:bracket");
	return serv;
}

void print_params(std::vector<Server> &config) {
	for (size_t i=0; i < config.size(); ++i) {
		std::cout << BLUE << i << RESET << std::endl;
		for (std::map<std::string, std::string>::iterator it=config[i].getParams().begin(); it != config[i].getParams().end(); ++it) {
			std::cout << it->first << "=\"" << it->second << "\"" << std::endl;
		}
		for (size_t k=0; k < config[i].getLocations().size(); ++k) {
			std::cout << YELLOW << "location " << RESET << config[i].getLocations()[k].getPath() << std::endl;
			for (std::map<std::string, std::string>::iterator it=config[i].getLocations()[k].getParams().begin(); it != config[i].getLocations()[k].getParams().end(); ++it) {
				std::cout << it->first << "=\"" << it->second << "\"" << std::endl;
			}
		}
	}
}

void write_cgi_env(std::map<std::string, std::string>& cgi_env, std::vector<Server> &config) {
	
}

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
			// std::cout << str << std::endl;
			
		}
		file.close();
	}
	print_params(config);
	return 0;
}
