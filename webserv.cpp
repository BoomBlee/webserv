#include "Node.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>

cmalt::ConstantsParametrs	params;
cmalt_kyoko::CGI			cgi;

int		main(int numArgs, char **args) {
	if (numArgs != 2)
		return 0;
	try {
		std::string	path = std::string(args[1]);
		third::Node	node(path);
		node.start_node();
		node.run_node();
	}
	catch (cmalt::BaseException &e) {
		std::cerr << e.what() << std::endl;
	}
	catch (std::exception& e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}
	return 0;
}
