#include "ConstatsParametrs.hpp"

class ConfigLocation {
private:
	std::string	path;
	std::string upload_path;
	bool		autoindex;
	std::string cgiPath;
	std::string	type;
	std::string	index;
	std::vector<std::string>	allowMethods;
public:
	ConfigLocation();
	~ConfigLocation();
	std::string	&getPath();
};
