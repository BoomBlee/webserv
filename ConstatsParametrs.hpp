#pragma once
#include "BaseException.hpp"

class	ConstatsParametrs {
public:
	ConstatsParametrs() {}
	~ConstatsParametrs() {}
	std::vector<std::string>	&getMethods();
	void		setMethods(std::string);
private:
	std::vector<std::string>							methods;
	std::string											version;
	std::map<std::string, std::vector<std::string> >		headers;
};
