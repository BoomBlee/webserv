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
	std::string	path = std::string(args[1]);
	try {
		third::Node	node(path);
		node.start_node();
		node.run_node();
	}
	catch (cmalt::BaseException &e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}