#include "headers.hpp"

//================================================================================
namespace cmalt {
//================================================================================
size_t	skipspace(std::string &str, size_t pos) {
	std::string::iterator it = str.begin() + pos;
	for (; *it == ' '; ++it) {}
	std::string		str1(it, str.end());
	return it - str.begin() - pos;
}

size_t	reverseskipspace(std::string &str, size_t pos) {
	std::string::iterator it = str.begin() + pos;
	if (pos == std::string::npos)
		it = str.end() - 1;
	for (; *it == ' '; --it) {}
	std::string		str1(str.begin() + pos, it);
	if (pos == std::string::npos)
		return (str.end() - 1) - it;
	return pos - (it - str.begin());
}
//================================================================================
//end namespace cmalt
}
//================================================================================

//================================================================================
namespace kyoko {
//================================================================================
void trim(std::string& src)
{
	src.erase(find_if_not(src.rbegin(), src.rend(), ::isspace).base(), src.end()); 
	src.erase(src.begin(), find_if_not(src.begin(), src.end(), ::isspace));

	src.erase(find(src.begin(), src.end(), '#'), src.end());
}
//================================================================================
//end namespace kyoko
}
//================================================================================
