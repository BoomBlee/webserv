#include "Node.hpp"
#include <csignal>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

cmalt::ConstantsParametrs	params;
cmalt_kyoko::CGI			cgi;
bool	work;

void sigint_handler(int n) {
	if (n == 0)
		work = false;
	work = false;
}

int		main(int numArgs, char **args) {
	work = true;
	signal(SIGINT, sigint_handler);
	if (numArgs != 2)
		return 0;
	try {
		std::string	path = std::string(args[1]);
		mterresa::Node	node(path);
		node.start_node();
		node.run_node();
	}
	catch (cmalt::BaseException &e) {
		std::cerr << RED  << e.what() <<RESET << std::endl;
	}
	catch (std::exception& e) {
		std::cerr << RED  << RED << e.what() << RESET <<RESET << std::endl;
	}
	return 0;
}
