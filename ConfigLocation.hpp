#pragma once
#include "ConstantParametrs.hpp"
// #include <cstddef>

//================================================================================
namespace kyoko {
//================================================================================

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
	size_t	_body_size;
	std::string _redirect;
public:
	ConfigLocation();
	ConfigLocation(const ConfigLocation &copy);
	~ConfigLocation();
	bool& getAutoindex();
	std::string& getPath();
	std::string& getUploadPath();
	std::string& getCgiPath();
	std::string& getType();
	std::string& getIndex();
	size_t&	getBodySize();
	std::vector<std::string>& getMethods();
	std::string& getRedirect();
	ConfigLocation	&operator=(const ConfigLocation &copy);
};

//================================================================================
//end namespace kyoko
}
//================================================================================
