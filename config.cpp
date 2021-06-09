#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#define CLOSE_BRACKET 0
#define OPEN_BRACKET 1

struct location
{
	std::string path;
	bool bracket;
	std::map<std::string, std::string> params;
};


struct server
{
	std::string server_name;
	std::vector<location> locations;
	std::map<std::string, std::string> params;
	bool bracket;
};

void trim(std::string& src)
{
    src.erase(std::find_if_not(src.rbegin(), src.rend(), ::isspace).base(), src.end()); 
    src.erase(src.begin(), std::find_if_not(src.begin(), src.end(), ::isspace));
}

template<typename T>
void write_param(std::string& str, T conf) {
	std::cout << str << std::endl;
}

location parse_location(std::ifstream& file, std::string& str) {
	location loc;
	size_t pos;

	if ((pos = str.find(" ")) != std::string::npos) {
		loc.path.insert(0, str, 0, pos);
		str = str.substr(pos);
		trim(str);
		if (str.find("{") != std::string::npos) {
			loc.bracket = OPEN_BRACKET;
		}
		else {
			getline (file,str);
			trim(str);
			if (str.find("{", 0) != std::string::npos) {
				loc.bracket = OPEN_BRACKET;
			}
			else {
				throw std::logic_error("error config");
			}
		}
	}
	else {
		loc.path.insert(0, str);
		getline (file,str);
		trim(str);
		if (str.find("{", 0) != std::string::npos) {
			loc.bracket = OPEN_BRACKET;
		}
		else {
			throw std::logic_error("error config");
		}
	}
	while (loc.bracket == OPEN_BRACKET) {
		getline (file,str);
		trim(str);
		if (str.find("}") != std::string::npos) {
			loc.bracket = CLOSE_BRACKET;
		}
		else {
			write_param(str, loc);
		}
	}
	return loc;
}

server parse_server(std::ifstream& file, std::string& str) {
	server serv;
	str = str.substr(6);
	trim(str);
	if (str.find("{") != std::string::npos) {
		serv.bracket = OPEN_BRACKET;
	}
	else {
		getline (file,str);
		trim(str);
		if (str.find("{", 0) != std::string::npos) {
			serv.bracket = OPEN_BRACKET;
		}
		else {
			throw ;
		}
	}
	while (serv.bracket == OPEN_BRACKET) {
		getline (file,str);
		trim(str);
		if (str.find("location", 0, 8) != std::string::npos) {
			str = str.substr(8);
			trim(str);
			serv.locations.push_back(parse_location(file, str));
		}
		if (str.find("}") != std::string::npos) {
			serv.bracket = CLOSE_BRACKET;
		}
		else {
			write_param(str, serv);
		}
	}
	return serv;
}

int main() {
	std::string str;
	std::ifstream file("example.conf");
	std::vector<server> config;

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
				catch(const std::exception& e) {
					std::cerr << e.what() << std::endl;
				}
			}
			// std::cout << str << std::endl;
			
		}
		file.close();
	}
	// std::cout << config.size() << std::endl;
	return 0;
}