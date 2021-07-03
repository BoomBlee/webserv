#pragma once
#include <string>
#include <vector>

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
	ConfigLocation();
	~ConfigLocation();
	bool& getAutoindex();
	std::string& getPath();
	std::string& getUploadPath();
	std::string& getCgiPath();
	std::string& getType();
	std::string& getIndex();
	std::vector<std::string>& getMethods();
};