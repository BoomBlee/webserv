#include "BaseException.hpp"
#include <string>

//================================================================================
namespace cmalt {
//================================================================================

BaseException::BaseException() : std::exception(), message(std::string("BaseMessage")), numError(0) {}

BaseException::BaseException(std::string message, int numError) : std::exception(), message(message), numError(numError) {}

BaseException::BaseException(std::string message, int numError, long &sock) : std::exception(), message(message), numError(numError) {
	close(sock);
	sock = -1;
}

BaseException::BaseException(const BaseException &copy) : std::exception() {
	*this = copy;
}

BaseException::~BaseException() throw () {}

BaseException	&BaseException::operator=(const BaseException &copy) {
	this->message = copy.message;
	this->numError = copy.numError;
	return *this;
}

const char	*BaseException::what() const throw () {
	return this->message.c_str();
}

const int	&BaseException::getErrorNumber() const throw () {
	return this->numError;
}

//================================================================================
//end namespace cmalt
}
//================================================================================
