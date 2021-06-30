#include "ConstatsParametrs.hpp"

class ConfigLocation {
private:
	std::string	path;
	std::string	type;
	std::vector<std::string>	allowMethods;
public:
	ConfigLocation();
	~ConfigLocation();
};
