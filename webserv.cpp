#include "Server.hpp"

int main () {
	try {
		ConfigServer	conf;
		Server	serv(conf);

		serv.accept();
		int n = 0;
		// while (n == 0) {
		// 	struct pf
		// }
		try {
			serv.recv();
		}
		catch (BaseException &e) {
			if (e.getErrorNumber() == 1) {
				serv.parseRequest();
				std::cout << "ff" << std::endl;
			}
			else
				std::cout << "NO" << std::endl;
		}
		// serv.send(); 
		// std::string	str("HTTP/1.1 200 OK\r\nContent-Length: 122\r\nContent-Type: text/html\r\n\r\n<html>\n<head><title>400 Bad Request</title></head>\n<body>\n<center><h1>400 Bad Request</h1></center>\n</body>\n</html>\r\n");
		// ::send(fd, str.c_str(), str.size(), 0);
	}
	catch (BaseException &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
