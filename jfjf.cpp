#include <iostream>

int main () {
	int host = 0;
	host = 127 << 8;
	host = (host + 0) << 8;
	host = (host + 0) << 8;
	host += 15;
	std::cout << host << std::endl;
	return 0;
}