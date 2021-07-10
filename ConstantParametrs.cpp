#include "ConstantParametrs.hpp"
#include <string>

//================================================================================
namespace cmalt {
//================================================================================

ConstantsParametrs::ConstantsParametrs() {
	this->setVersion("HTTP/1.1");
	this->setMethod("GET ");
	this->setMethod("PUT ");
	this->setMethod("POST ");
	this->setMethod("DELETE ");
	this->setType("html", "text/html");
	this->setType("htm", "text/html");
	this->setType("css", "text/css");
	this->setType("js", "text/javascript");
	this->setType("xml", "text/xml");
	this->setType("gif", "image/gif");
	this->setType("jpg", "image/jpeg");
	this->setType("jpeg", "image/jpeg");
	this->setType("png", "image/png");
	this->setType("bmp", "image/bmp");
	this->setType("ico", "image/x-icon");
}

ConstantsParametrs::ConstantsParametrs(const ConstantsParametrs &copy) {
	*this = copy;
}

ConstantsParametrs::~ConstantsParametrs() {}

ConstantsParametrs	&ConstantsParametrs::operator=(const ConstantsParametrs &copy) {
	this->headers = copy.headers;
	this->methods = copy.methods;
	this->version = copy.version;
	this->types = copy.types;
	return *this;
}

std::vector<std::string>							&ConstantsParametrs::getMethods() {
	return this->methods;
}

void												ConstantsParametrs::setMethod(std::string method) {
	this->methods.push_back(method);
}

std::string											&ConstantsParametrs::getVersion() {
	return this->version;
}

void												ConstantsParametrs::setVersion(std::string version) {
	this->version = version;
}

std::map<std::string, std::vector<std::string> >	&ConstantsParametrs::getHeaders() {
	return this->headers;
}

void												ConstantsParametrs::setType(std::string type, std::string standart) {
	this->types[type] = standart;
}

std::map<std::string, std::string>					&ConstantsParametrs::getTypes() {
	return this->types;
}

//================================================================================
//end namespace cmalt
}
//================================================================================