#include "headers.hpp"

size_t	skipspace(std::string &str) {
	std::string::iterator it = str.begin();
	for (; *it == ' '; it++) {}
	std::string		str1(it, str.end());
	return it - str.begin();
}

size_t	reverseskipspace(std::string &str) {
	std::string::reverse_iterator it = str.rbegin();
	for (; *it == ' '; it++) {}
	std::string		str1(it, str.rend());
	return it - str.rbegin();
}

void trim(std::string& src)
{
	src.erase(find_if_not(src.rbegin(), src.rend(), ::isspace).base(), src.end()); 
	src.erase(src.begin(), find_if_not(src.begin(), src.end(), ::isspace));

	src.erase(find(src.begin(), src.end(), '#'), src.end());
}