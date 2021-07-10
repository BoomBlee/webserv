#include "Request.hpp"
#include <map>
#include <string>

//================================================================================
namespace cmalt_kyoko {
//================================================================================
class	CGI {
public:
	CGI();
	CGI(const CGI &);
	~CGI();
	CGI									&operator=(const CGI &);
	std::string							execCGI(cmalt::Request &);
private:
	std::map<std::string, std::string>	cgiEnv;
	std::string							ret;
	long								saveCout;
	long								saveCin;
	FILE								*fileIn;
	FILE								*fileOut;
	long								fin;
	long								fout;
private:
	void								clear();
	void								mapToChar(char ***);
	void								openTmpFile();
	void								workWithFork(pid_t, char **, cmalt::Request &);
	void								closeTmpFile();
	void								deleteEnv(char ***, cmalt::Request &);
	void								setCGI(cmalt::Request &);
};
//================================================================================
//end namespace cmalt_kyoko
}
//================================================================================