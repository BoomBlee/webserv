#include "headers.hpp"
#include <string>

//================================================================================
namespace cmalt {
//================================================================================

class	BaseException : public std::exception {
public:
	BaseException();
	BaseException(std::string, int);
	BaseException(std::string, int, long &);
	BaseException(const BaseException &);
	virtual ~BaseException() throw ();
	BaseException		&operator=(const BaseException &);
	virtual	const char	*what() const throw ();
	const int			&getErrorNumber() const throw ();
private:
	int					numError;
	std::string			message;
};

template<class Iter, class T>
void	vecFind(Iter first, Iter second, T &val, int code = 0, std::string status = "None") {
	for (;first != second; ++first) {
		if (*first == val)
			return;
	}
	throw BaseException(status, code);
}

template<class Iter, class T>
void	mapFind(Iter first, Iter second, T &val, int code = 0, std::string status = "None") {
	for (;first != second; ++first) {
		if (first.first == val)
			return;
	}
	throw BaseException(status, code);
}

//================================================================================
//end namespace cmalt
}
//================================================================================
