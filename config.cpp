#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define CLOSE_BRACKET 0
#define OPEN_BRACKET 1

struct location
{
	std::string path;
};


struct server
{
	std::string server_name;
	std::vector<location> locations;
	bool bracket;
};

void trim(std::string& src)
{
    src.erase(std::find_if_not(src.rbegin(), src.rend(), ::isspace).base(), src.end()); 
    src.erase(src.begin(), std::find_if_not(src.begin(), src.end(), ::isspace));
}

void write_param(std::string& str) {

}

location parse_location(std::ifstream& file, std::string& str) {
	location loc;
	size_t pos;

	if ((pos = str.find(" ")) != std::string::npos) {
		loc.path.insert(0, str, pos);
		str.substr(pos);
		trim(str);
	}


	return loc;
}

server parse_server(std::ifstream& file, std::string& str) {
	server serv;
	str = str.substr(6);
	trim(str);
	if (str.find("{") != std::string::npos) {
		serv.bracket = OPEN_BRACKET;
		std::cout << "if" << "\"" << str << "\"" << std::endl;
	}
	else {
		getline (file,str);
		std::cout << "else" << "\"" << str << "\"" << std::endl;
		trim(str);
		if (str.find("{", 0) != std::string::npos) {
			serv.bracket = OPEN_BRACKET;
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
			write_param(str);
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
				config.push_back(parse_server(file, str));
			}
			// std::cout << str << std::endl;
			
		}
		file.close();
	}
	std::cout << config.size() << std::endl;
	return 0;
}