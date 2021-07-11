#include "ConfigLocation.hpp"

namespace kyoko {
ConfigLocation::ConfigLocation() : _autoindex(false), _body_size(0) {};
ConfigLocation::ConfigLocation(const ConfigLocation &copy) {*this = copy;};
ConfigLocation::~ConfigLocation() {};
bool& ConfigLocation::getAutoindex() {return _autoindex;}
std::string& ConfigLocation::getPath() {return _path;};
std::string& ConfigLocation::getUploadPath() {return _upload_path;};
std::string& ConfigLocation::getCgiPath() {return _cgiPath;};
std::string& ConfigLocation::getType() {return _type;};
std::string& ConfigLocation::getIndex() {return _index;};
std::vector<std::string>& ConfigLocation::getMethods() {return _Methods;};
ConfigLocation&	ConfigLocation::operator=(const ConfigLocation &copy) {
	this->_autoindex = copy._autoindex;
	this->_path = copy._path;
	this->_upload_path = copy._upload_path;
	this->_cgiPath = copy._cgiPath;
	this->_type = copy._type;
	this->_index = copy._index;
	this->_Methods = copy._Methods;
	this->_body_size = copy._body_size;
	return *this;
}
size_t&	ConfigLocation::getBodySize() {return _body_size;};
}