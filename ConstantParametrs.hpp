#include "BaseException.hpp"
#include <string>

//================================================================================
namespace cmalt {
//================================================================================

class	ConstantsParametrs {
public:
	ConstantsParametrs();
	ConstantsParametrs(const ConstantsParametrs &);
	~ConstantsParametrs();
	ConstantsParametrs									&operator=(const ConstantsParametrs &);
	std::vector<std::string>							&getMethods();
	void												setMethod(std::string);
	std::string											&getVersion();
	void												setVersion(std::string);
	std::map<std::string, std::vector<std::string> >	&getHeaders();
	void												setType(std::string, std::string);
	std::map<std::string, std::string>					&getTypes();
private:
	std::vector<std::string>							methods;
	std::string											version;
	std::map<std::string, std::vector<std::string> >	headers;
	std::map<std::string, std::string>					types;
};


//================================================================================
//end namespace cmalt
}
//================================================================================