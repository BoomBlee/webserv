#include "ConstatsParametrs.hpp"

std::vector<std::string>	&ConstatsParametrs::getMethods() {
	return this->methods;
}

void	ConstatsParametrs::setMethods(std::string method) {
	this->methods.push_back(method);
}
