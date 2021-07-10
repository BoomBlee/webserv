#include "ConstantParametrs.hpp"
#include <cstddef>

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
	//ДОБАВИТЬ ОБРАБОТКУ ЭТОГО
	size_t	_body_size;

public:
	ConfigLocation() : _autoindex(true), _body_size(0) {};
	ConfigLocation(const ConfigLocation &copy) {*this = copy;};
	~ConfigLocation() {};
	ConfigLocation	&operator=(const ConfigLocation &copy) {
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
	bool& getAutoindex() {return _autoindex;}
	std::string& getPath() {return _path;};
	std::string& getUploadPath() {return _upload_path;};
	std::string& getCgiPath() {return _cgiPath;};
	std::string& getType() {return _type;};
	std::string& getIndex() {return _index;};
	size_t&	getBodySize() {return _body_size;};
	std::vector<std::string>& getMethods() {return _Methods;};
};

//================================================================================
//end namespace kyoko
}
//================================================================================