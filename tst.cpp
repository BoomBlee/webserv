#include "headers.hpp"

int main() {
	long saveCin = dup(STDIN_FILENO);
	long saveCout = dup(STDOUT_FILENO);
	FILE *fileIn = tmpfile();
	FILE *fileOut = tmpfile();
	long fin = fileno(fileIn);
	long fout = fileno(fileOut);
	write(fin, "", 0);
	lseek(fin, 0, SEEK_SET);
	pid_t pid = fork();
	if (pid == -1)
		exit(0);
	if (pid == 0) {
		char * const * null = NULL;
		dup2(fin, STDIN_FILENO);
		dup2(fout, STDOUT_FILENO);
		char	**arg = new char*[3];
		arg[0] = new char[23];
		memcpy(arg[0], "/usr/local/bin/python3", 23);
		arg[1] = new char[21];
		arg[2] = nullptr;
		memcpy(arg[1], "./scriptCGI/hello.py", 21);
			// char	*arg[2] = {const_cast<char *>("/usr/local/bin/python3"), const_cast<char *>("./scriptCGI/hello.py")};
			execve(arg[0], &arg[0], NULL);
			std::cerr << arg[0] << "|" << arg[1] << std::endl;
	}
	else {
		char	buffer[TCP_SIZE];
		int		status;
		waitpid(-1, &status, 0);
		if (WEXITSTATUS(status) == 500)
			return 0;
		lseek(fout, 0, SEEK_SET);
		int i = 1;
		std::string ret;
		while (i > 0)
		{
			memset(buffer, 0, TCP_SIZE);
			i = read(fout, buffer, TCP_SIZE - 1);
			ret += std::string(buffer);
		}
	}
}