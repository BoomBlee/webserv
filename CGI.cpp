#include "CGI.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <iterator>
#include <map>
#include <new>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

//================================================================================
namespace cmalt_kyoko {
//================================================================================

CGI::CGI() {
	this->cgiEnv["AUTH_TYPE"];
	this->cgiEnv["REDIRECT_STATUS"] = "200";
	this->cgiEnv["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->cgiEnv["SCRIPT_NAME"];
	this->cgiEnv["SCRIPT_FILENAME"];
	this->cgiEnv["REQUEST_METHOD"];
	this->cgiEnv["CONTENT_LENGTH"];
	this->cgiEnv["CONTENT_TYPE"];
	this->cgiEnv["PATH_INFO"];
	this->cgiEnv["PATH_TRANSLATED"];
	this->cgiEnv["QUERY_STRING"];
	this->cgiEnv["REMOTE_ADDR"];
	this->cgiEnv["REMOTE_IDENT"];
	this->cgiEnv["REMOTE_USER"];
	this->cgiEnv["REQUEST_URI"];
	this->cgiEnv["SERVER_NAME"];
	this->cgiEnv["SERVER_PORT"];
	this->cgiEnv["SERVER_PROTOCOL"];
	this->cgiEnv["SERVER_SOFTWARE"] = "Weebserv/1.0";
}

CGI::CGI(const CGI &copy) {
	*this = copy;
}

CGI::~CGI() {}

CGI &CGI::operator=(const CGI &copy) {
	this->cgiEnv = copy.cgiEnv;
	this->ret = copy.ret;
	this->saveCin = copy.saveCin;
	this->saveCout = copy.saveCout;
	this->fileIn = copy.fileIn;
	this->fileOut = copy.fileOut;
	this->fin = copy.fin;
	this->fout = copy.fout;
	return *this;
}

void					CGI::clear() {
	this->ret.clear();
	this->cgiEnv["AUTH_TYPE"].clear();
	this->cgiEnv["SCRIPT_NAME"].clear();
	this->cgiEnv["SCRIPT_FILENAME"].clear();
	this->cgiEnv["REQUEST_METHOD"].clear();
	this->cgiEnv["CONTENT_LENGTH"].clear();
	this->cgiEnv["CONTENT_TYPE"].clear();
	this->cgiEnv["PATH_INFO"].clear();
	this->cgiEnv["PATH_TRANSLATED"].clear();
	this->cgiEnv["QUERY_STRING"].clear();
	this->cgiEnv["REMOTE_ADDR"].clear();
	this->cgiEnv["REMOTE_IDENT"].clear();
	this->cgiEnv["REMOTE_USER"].clear();
	this->cgiEnv["REQUEST_URI"].clear();
	this->cgiEnv["SERVER_NAME"].clear();
	this->cgiEnv["SERVER_PORT"].clear();
	this->cgiEnv["SERVER_PROTOCOL"].clear();
}

void				CGI::setCGI(cmalt::Request &req) {
	this->ret = req.getBody();
	this->cgiEnv["SCRIPT_NAME"] = req.getLocation().getCgiPath();
	this->cgiEnv["SCRIPT_FILENAME"] = req.getLocation().getCgiPath();
	this->cgiEnv["REQUEST_METHOD"] = req.getMethod();
	this->cgiEnv["CONTENT_LENGTH"] = std::to_string(this->ret.size());
	if (req.getHeaders()["Content-Type"].begin() != req.getHeaders()["Content-Type"].end())
		this->cgiEnv["CONTENT_TYPE"] = req.getHeaders()["Content-Type"].begin()->first;
	this->cgiEnv["PATH_INFO"] = req.getPath();
	this->cgiEnv["PATH_TRANSLATED"] = req.getFullPath();
	this->cgiEnv["QUERY_STRING"] = req.getQuery();
	int	host = req.getConfig().getHost();
	this->cgiEnv["REMOTE_ADDR"] = std::to_string(host >> 24) + "." + std::to_string(host << 8 >> 24) + "." + std::to_string(host << 16 >> 24) + "." + std::to_string(host << 24 >> 24);
	this->cgiEnv["REQUEST_URI"] = req.getPath() + "?" + req.getQuery();
	this->cgiEnv["SERVER_NAME"] = req.getConfig().getServerName();
	this->cgiEnv["SERVER_PORT"] = std::to_string(req.getConfig().getPort());
	this->cgiEnv["SERVER_PROTOCOL"] = req.getVersion();
}

std::string			CGI::execCGI(cmalt::Request &req) {
	char	**env = NULL;
	this->clear();
	this->setCGI(req);
	try {
		this->mapToChar(&env);
	}
	catch (std::bad_alloc &e) {
		std::cerr << e.what() << std::endl;
	}
	this->openTmpFile();
	pid_t	pid = fork();
	this->workWithFork(pid, env, req);
	this->closeTmpFile();
	this->deleteEnv(&env);
	if (pid == 0)
		exit(500);
	return this->ret;
}

void				CGI::mapToChar(char ***env) {
	*env = new char *[this->cgiEnv.size() + 1];
	int	i = 0;
	for (std::map<std::string, std::string>::iterator it = this->cgiEnv.begin(); it != this->cgiEnv.end(); ++it) {
		std::string	newEnv = it->first + "=" + it->second;
		(*env)[i] = new char[newEnv.size() + 1];
		std::strcpy((*env)[i], newEnv.c_str());
		i++;
	}
	(*env)[i] = NULL;
}

void				CGI::openTmpFile() {
	this->saveCin = dup(STDIN_FILENO);
	this->saveCout = dup(STDOUT_FILENO);
	this->fileIn = tmpfile();
	this->fileOut = tmpfile();
	this->fin = fileno(this->fileIn);
	this->fout = fileno(this->fileOut);
	write(this->fin, this->ret.c_str(), this->ret.size());
	lseek(this->fin, 0, SEEK_SET);
}

void				CGI::workWithFork(pid_t pid, char **env, cmalt::Request &req) {
	if (pid == -1)
		throw cmalt::BaseException("Internal Server Error", 500);
	if (pid == 0) {
		char * const * null = NULL;
		dup2(this->fin, STDIN_FILENO);
		dup2(this->fout, STDOUT_FILENO);
		execve(req.getLocation().getCgiPath().c_str(), null, env);
	}
	else {
		char	buffer[TCP_SIZE];
		int		status;
		waitpid(-1, &status, 0);
		if (WEXITSTATUS(status) == 500)
			throw cmalt::BaseException("Internal Server Error", 500);
		lseek(this->fout, 0, SEEK_SET);
		int i = 1;
		while (i > 0)
		{
			memset(buffer, 0, TCP_SIZE);
			i = read(this->fout, buffer, TCP_SIZE - 1);
			this->ret += std::string(buffer);
		}
	}
}

void				CGI::closeTmpFile() {
	dup2(STDIN_FILENO, this->saveCin);
	dup2(STDOUT_FILENO, this->saveCout);
	fclose(this->fileIn);
	fclose(this->fileOut);
	close(this->fin);
	close(this->fout);
	close(this->saveCin);
	close(this->saveCout);
}

void				CGI::deleteEnv(char ***env) {
	if (env && *env) {
		for (int i = 0; (*env)[i]; ++i)
			delete[] (*env)[i];
		delete[] (*env);
	}
}


//================================================================================
//end namespace cmalt_kyoko
}
//================================================================================