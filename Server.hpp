#include "Response.hpp"

class	Server {
public:
	Server(const Server &);
	Server(ConfigServer &);
	~Server() {};
	Server	&operator=(const Server &);
	// long		getListenSock();
	// long		getAcceptSock();
	// struct	sockaddr_in		getSockaddr();
	long		accept();
	void		recv();
	void		send();
	int			chunked_detect(size_t &, std::string &);

	void		parseRequest();

	// void		send();
	class	ServerException : public BaseException {
	public:
		ServerException(std::string, int);
		ServerException(std::string, int, long &);
		virtual ~ServerException() throw ();
	};
private:
	struct	sockaddr_in		addr;
	ConfigServer			*config;
	ConstatsParametrs		params;
	long					listen_socket;
	long					accept_socket;
	Request					request;//для запросов на этот сервер
	std::string				reading;
	Response				response;//для ответов с этого сервера
}; // класс содержащий информацию об одном хосте
