#include "BaseException.hpp"

BaseException::BaseException(std::string &message, int &error) : std::exception(), numError(error), message(message) {}

BaseException::~BaseException() throw () {}

const char	*BaseException::what() const throw() {
	return this->message.c_str();
}

const int	&BaseException::getErrorNumber() const throw() {
	return this->numError;
}
