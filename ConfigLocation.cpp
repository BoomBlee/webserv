#include "ConfigLocation.hpp"

ConfigLocation::ConfigLocation() {
	this->path = "./errors/";
}

ConfigLocation::~ConfigLocation() {
}

std::string		&ConfigLocation::getPath() {
	return this->path;
}
