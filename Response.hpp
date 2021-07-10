#include "CGI.hpp"
#include <map>
#include <string>


//================================================================================
namespace cmalt {
//================================================================================
class	Response {
public:
	Response();
	Response(const Response &);
	~Response();
	Response							&operator=(const Response &);
	void								initialisation(Request &);

	kyoko::ConfigLocation				&getConfig();
	std::string							&getType();
	std::string							&getAsk();
	Request								&getRequest();
	std::map<std::string, std::string>	&getHeaders();
private:
	kyoko::ConfigLocation				conf;
	std::string							path;
	std::string							fullPath;
	std::string							type;
	std::string							ask;
	Request 							req;
	std::map<std::string, std::string>	headers;
	std::fstream						file;
	std::map<int, std::string>			errorsPath;
	std::map<std::string, void (Response::*)()>	methodFunctions;
private:
	void								clear();
	void								setConfig(kyoko::ConfigLocation &);
	void								setPath(std::string &);
	void								setType(std::string &);//path
	void								setHeaders();
	void								setAllow();
	std::string							getDate();
	void								setRequest(Request &);
	void								openFile(std::string &, std::ios_base::openmode);
	int									stat(const char *);
	void								closeFile();
	void								readFile();
	void								writeFile(std::string &);

	void								setMethodFunctions();
	void								setErrorsPath();
	
	void								methodGET();
	void								methodPUT();
	void								methodPOST();
	void								methodDELETE();
	// void								methodHEAD();

	void								cgi();

	std::string							askStatus();
	std::string							askHeaders();
	void								getErrorHtml();

	std::string							generateAutoindex(const char *);
};
//================================================================================
//end namespace cmalt
}
//================================================================================