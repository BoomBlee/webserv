#include "Server.hpp"

int main () {
	ConfigServer	conf;
	Server	serv(conf);

	serv.accept();
	// try {
	// 	serv.recv();
	// }
	// catch (BaseException &e) {
	// 	serv.parseRequest();
	// 	std::cout << "ff" << std::endl;
	// }
	serv.send();
	// std::string	str("HTTP/1.1 200 OK\r\nContent-Length: 122\r\nContent-Type: text/html\r\n\r\n<html>\n<head><title>400 Bad Request</title></head>\n<body>\n<center><h1>400 Bad Request</h1></center>\n</body>\n</html>\r\n");
	// ::send(fd, str.c_str(), str.size(), 0);

	return 0;
}
