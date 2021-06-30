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
