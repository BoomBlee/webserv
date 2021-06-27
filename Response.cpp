#include "test.hpp"


/*================================================================================
TOOLS
================================================================================*/




/*================================================================================
RESPONSE
================================================================================*/

void	Response::initialization(Request &req) {
	this->req = req;
	this->setCGIEnv();
}

void	Response::setCGIEnv() {
	// this->cgi_env.insert(req.conf.cgiEnv.begin(), req.conf.cgiEnv.end());
	std::map<std::string, std::list<std::pair<std::string, double> > >::iterator it = this->req.getHeaders().begin();
	for (; it != this->req.getHeaders().end(); ++it) {
		if (it->first.find("Secret") != std::string::npos) {
			this->cgi_env[it->first] = it->second.begin()->first;
		}
	}
	for (std::map<std::string, std::string>::iterator it1 = this->cgi_env.begin(); it1 != this->cgi_env.end(); ++it1) {
		std::cout << it1->first << "=" << it1->second << std::endl;
	}
}







/*================================================================================
================================================================================*/

