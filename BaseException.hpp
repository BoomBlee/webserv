#pragma once
#include "headers.hpp"

class	BaseException : public std::exception {
public:
	BaseException(std::string &message, int &error);
	virtual ~BaseException() throw ();
	virtual	const char *what() const throw ();
	const int			&getErrorNumber() const throw ();
private:
	int			numError;
	std::string	message;
}; // базовый класс ошибок