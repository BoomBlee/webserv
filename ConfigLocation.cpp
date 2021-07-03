#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() : _autoindex(true) {};
ConfigLocation::~ConfigLocation() {};
bool& ConfigLocation::getAutoindex() {return _autoindex;}
std::string& ConfigLocation::getPath() {return _path;};
std::string& ConfigLocation::getUploadPath() {return _upload_path;};
std::string& ConfigLocation::getCgiPath() {return _cgiPath;};
std::string& ConfigLocation::getType() {return _type;};
std::string& ConfigLocation::getIndex() {return _index;};
std::vector<std::string>& ConfigLocation::getMethods() {return _Methods;};
