#include "Server.hpp"

int main () {
	try {
		ConfigServer	conf;
		Server	serv(conf);

		long fd = serv.accept();
		int n = 0;
		struct pollfd fds[2];
		fds[0].fd = fd;
		fds[0].events = POLLIN;
		fds[1].fd = fd;
		fds[1].events = POLLOUT;
		while (1) {
			n = 0;
			while (n >= 0) {
				// std::cout << "kkk" << std::endl;
				poll(fds, 2, 1000);
				if (fds[0].revents == POLLIN && n == 0) {
					fds[0].revents = 0;
					n = 1;
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
				}
				if (fds[1].revents == POLLOUT && n == 1) {
					fds[1].revents = 0;
					n = 0;
					serv.send();
				}
			}
			std::cout << "kkk1" << std::endl;
		}
		// std::string	str("HTTP/1.1 200 OK\r\nContent-Length: 122\r\nContent-Type: text/html\r\n\r\n<html>\n<head><title>400 Bad Request</title></head>\n<body>\n<center><h1>400 Bad Request</h1></center>\n</body>\n</html>\r\n");
		// ::send(fd, str.c_str(), str.size(), 0);
	}
	catch (BaseException &e) {
		std::cout << e.what() << std::endl;
	}
	// std::string	str("/hello/myserv.html");
	// std::cout << str.find("/hello") << std::endl;
	return 0;
}
