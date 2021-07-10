#pragma one

#include <algorithm>
// #include <asm-generic/socket.h>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <netinet/in.h>
#include <ostream>
#include <fstream>
#include <string>
#include <cstring>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>
#include <stdlib.h>
#include <map>
#include <list>
#include <utility>
#include <vector>
#include <stack>
#include <sys/time.h>
#include "color.hpp"
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#define TCP_SIZE 65536//максимальный размер пакета в TCP


//================================================================================
namespace cmalt {
//================================================================================
size_t	skipspace(std::string &str, size_t pos);
size_t	reverseskipspace(std::string &str, size_t pos);
//================================================================================
//end namespace cmalt
}
//================================================================================

//================================================================================
namespace kyoko {
//================================================================================
void	trim(std::string& src);
//================================================================================
//end namespace kyoko
}
//================================================================================
