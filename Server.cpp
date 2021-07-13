#include "Server.hpp"
#include "color.hpp"
#include "headers.hpp"
// #include <bits/types/time_t.h>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>

//================================================================================
namespace third {

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Конструкторы, Деструктор и перегрузка операторов
	*/

	/*
		Стандартный конструктор
	*/
	Server::Server() : _listen_socket(-1) {}

	/*
		Конструктор с конфигом в качастве параметра
	*/
	Server::Server(kyoko::ConfigServer& config) : _listen_socket(-1), _config(config) {
		this->start_listening_host();
	}

	/*
		Конструктор копирования
	*/
	Server::Server(const Server& src_server) : _listen_socket(-1) {
		*this = src_server;
		if (_listen_socket < 0)
			this->start_listening_host();
	}

	/*
		Деструктор
	*/
	Server::~Server() {}

	/*
		Перегрузка оператора присванивание
	*/
	Server&	Server::operator=(const Server&	src_server) {
		this->_addr = src_server._addr;
		this->_config = src_server._config;
		this->_listen_socket = src_server._listen_socket;
		this->_request = src_server._request;
		this->_read_buf = src_server._read_buf;
		this->_response = src_server._response;
		this->_request_is_full = src_server._request_is_full;
		return *this;
	}

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Геттеры
	*/

	/*
		Гетер для _addr
	*/
	struct	sockaddr_in&	Server::get_sock_addr() {
		return this->_addr;
	};

	/*
		Гетер для _config
	*/
	kyoko::ConfigServer&	Server::get_config() {
		return this->_config;
	};

	/*
		Гетер для _listen_socket
	*/
	long&	Server::get_listen_socket() {
		return this->_listen_socket;
	};

	/*
		Гетер для _request
	*/
	cmalt::Request&	Server::get_request(long accept_socket) {
		return this->_request[accept_socket];
	};

	std::map<long, cmalt::Request>&	Server::get_request() {
		return this->_request;
	};

	/*
		Гетер для _read_buf
	*/
	std::string&	Server::get_read_buf(long accept_socket) {
		return this->_read_buf[accept_socket];
	};

	std::map<long, std::string>&	Server::get_read_buf() {
		return this->_read_buf;
	};

	/*
		Гетер для _response
	*/
	cmalt::Response&	Server::get_response(long accept_socket) {
		return this->_response[accept_socket];
	};

	std::map<long, cmalt::Response>&	Server::get_response() {
		return this->_response;
	};

	/*
		Гетер для _request_is_full
	*/
	bool&	Server::get_request_is_full(long accept_socket) {
		return this->_request_is_full[accept_socket];
	};

	std::map<long, bool>&	Server::get_request_is_full() {
		return this->_request_is_full;
	};

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Сеттеры
	*/

	/*
		Сетер для _addr
	*/
	void	Server::set_sock_addr(struct sockaddr_in&	addr) {
		this->_addr = addr;
	};

	/*
		Сетер для _config
	*/
	void	Server::set_config(kyoko::ConfigServer&	config) {
		this->_config = config;
	};

	/*
		Сетер для _listen_socket
	*/
	void	Server::set_listen_socket(long&	listen_socket) {
		this->_listen_socket = listen_socket;
	};

	/*
		Сетер для _request
	*/
	void	Server::set_request(cmalt::Request&	request, long accept_socket) {
		this->_request[accept_socket] = request;
	};

	void	Server::set_request(std::map<long, cmalt::Request>&	request) {
		this->_request = request;
	};

	/*
		Сетер для _read_buf
	*/
	void	Server::set_read_buf(std::string&	read_buf, long accept_socket) {
		this->_read_buf[accept_socket] = read_buf;
	};

	void	Server::set_read_buf(std::map<long, std::string>&	read_buf) {
		this->_read_buf = read_buf;
	};

	/*
		Сетер для _response
	*/
	void	Server::set_response(cmalt::Response&	response, long accept_socket) {
		this->_response[accept_socket] = response;
	};

	void	Server::set_response(std::map<long, cmalt::Response>&	response) {
		this->_response = response;
	};

	/*
		Сетер для _request_is_full
	*/
	void	Server::set_request_is_full(bool	request_is_full, long accept_socket) {
		this->_request_is_full[accept_socket] = request_is_full;
	};

	void	Server::set_request_is_full(std::map<long, bool>&	request_is_full) {
		this->_request_is_full = request_is_full;
	};

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Методы работы сервера
	*/

	/*
		Начало работы
	*/
	void	Server::start_listening_host() {
		if (this->_listen_socket < 0) {
			/*
				Структура _addr полностью зануляется, а после в нее записывается:
					ip преобразованный в сетевой порядок байт (htonl)
					порт преобразованный в сетевой порядок байт (htons)
					указывается, что сокет работает в семействе аддресов AF_INET, т.е. с адресами интернет протокола
			*/
			memset(reinterpret_cast<char*>(&this->_addr), 0, sizeof(this->_addr));
			this->_addr.sin_addr.s_addr = htonl(static_cast<uint32_t>(this->_config.getHost()));
			this->_addr.sin_port = htons(static_cast<uint16_t>(this->_config.getPort()));
			this->_addr.sin_family = AF_INET;
			/*
				Создается сокет способный работать с адресами интернет протокола(AF_INET) и работающий на протоколе TCP(SOCK_STREAM)
				В случае ошибки бросается исключение с кодом 1
			*/
			this->_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (this->_listen_socket == -1)
				throw cmalt::BaseException("Failed to create socket", 1);
			/*
				Созданный сокет привязывается к нашей структуре, а точнее к данным внесенным в нее, т.е. к ip и порту
				В случае ошибки бросается исключение с кодом 2, а созданный сокет закрывается
			*/
			if (bind(this->_listen_socket, reinterpret_cast<struct sockaddr *>(&this->_addr), static_cast<socklen_t>(sizeof(this->_addr))) == -1)
				throw cmalt::BaseException("Failed to bind socket to " + std::to_string(this->_config.getHost()) + ":" + std::to_string(this->_config.getPort()), 2, this->_listen_socket);
			/*
				Привязанный сокет запускает прослушивание, т.е. он начинает принимать запросы на соединие
				В случае ошибки бросается исключение с кодом 3, а созданный сокет закрывается
			*/
			if (listen(this->_listen_socket, 1000) == -1)
				throw cmalt::BaseException("Failed to listen socket to " + std::to_string(this->_config.getHost()) + ":" + std::to_string(this->_config.getPort()), 3, this->_listen_socket);
		}
	}
	
	/*
		Соединение с клиентом
	*/
	long	Server::accept() {
		long	accept_socket;
		/*
			На основе сокета принимающего запросы на соединение мы создаем сокет уже соединенный с клиентом
			Данный сокет не хранится в классе, т.к. у одного сервера может быть множество соединений одновременно
			В случае ошибки бросается исключение с кодом 4
		*/
		socklen_t	size = static_cast<socklen_t>(sizeof(this->_addr));
		accept_socket = ::accept(this->_listen_socket, reinterpret_cast<struct sockaddr *>(&this->_addr), &size);
		if (accept_socket == -1)
			throw cmalt::BaseException("Failed to accept socket to " + std::to_string(this->_config.getHost()) + ":" + std::to_string(this->_config.getPort()), 4);
		fcntl(accept_socket, F_SETFL, O_NONBLOCK);
		/*
			Для данного сокета создаются объекты классов Request и Response
		*/
		cmalt::Request	request;
		request.setConfig(this->_config);
		cmalt::Response	response;
		this->_request[accept_socket] = request;
		this->_response[accept_socket] = response;
		this->_request_is_full[accept_socket] = false;
		this->_send_pos[accept_socket] = 0;
		this->_request_params[accept_socket];
		return accept_socket;
	}

	/*
		Получение запроса
	*/

	void	Server::recv(long& accept_socket) {
		struct timeval	start1, start2, end1, end2;
		if (this->_request_is_full[accept_socket] == false) {
			char	buf[TCP_SIZE];
			errno = 0;
			int		size = ::recv(accept_socket, buf, TCP_SIZE - 1, 0);
			if (size == 0) {
				this->_read_buf.erase(accept_socket);
				throw cmalt::BaseException("\rConnection was closed by client", 0);
			}
			if (size == -1) {
				this->_read_buf.erase(accept_socket);
				throw cmalt::BaseException("\rRead error, closing connection", 0);
			}
			memset(&buf[size], '\0', TCP_SIZE - size);
			this->_read_buf[accept_socket] += std::string(buf);
			if (!this->_request_params[accept_socket]._if_length) {
				this->_request_params[accept_socket]._length = this->_read_buf[accept_socket].find("\r\n\r\n");
				if (this->_request_params[accept_socket]._length != std::string::npos)
					this->_request_params[accept_socket]._if_length = true;
			}
			if (this->_request_params[accept_socket]._if_length) {
				if (!this->_request_params[accept_socket]._if_type) {
					this->_request_params[accept_socket]._pos = this->_read_buf[accept_socket].find("Content-Length:");
					if (this->_request_params[accept_socket]._pos == std::string::npos) {
						this->_request_params[accept_socket]._pos = this->_read_buf[accept_socket].find("Transfer-Encoding:");
						if (this->_request_params[accept_socket]._pos != std::string::npos && this->chunked_detect(this->_request_params[accept_socket]._pos, accept_socket)) {
						 	this->_request_params[accept_socket]._type = 1;//transfer-encoding
							if (this->_read_buf[accept_socket].find("0\r\n\r\n", this->_request_params[accept_socket]._length) + 5 == this->_read_buf[accept_socket].size())
								this->_request_is_full[accept_socket] = true;
							this->_request_params[accept_socket]._pos = this->_request_params[accept_socket]._length;
						}
						else
							this->_request_is_full[accept_socket] = true;
					}
					else {
						this->_request_params[accept_socket]._type = 2;//content-length
						this->_request_params[accept_socket]._pos = this->request_content_length_start_pos(this->_request_params[accept_socket]._pos, accept_socket);
						this->_request_params[accept_socket]._size = this->_request_params[accept_socket]._length + 4 + std::atoi(this->_read_buf[accept_socket].substr(this->_request_params[accept_socket]._pos, this->request_content_length_end_pos(this->_request_params[accept_socket]._pos, accept_socket)).c_str());
					}
					this->_request_params[accept_socket]._if_type = true;
				}
				else {
					if (this->_request_params[accept_socket]._type == 1) {
						if (this->_read_buf[accept_socket].find("0\r\n\r\n", this->_request_params[accept_socket]._pos) + 5 == this->_read_buf[accept_socket].size())
							this->_request_is_full[accept_socket] = true;
						std::cout << this->_read_buf[accept_socket].size() << std::endl;
						if (this->_read_buf[accept_socket].find_last_of("0") != std::string::npos)
							this->_request_params[accept_socket]._pos = this->_read_buf[accept_socket].find_last_of("0");
					}
					else if (this->_read_buf[accept_socket].size() >= this->_request_params[accept_socket]._size)
						this->_request_is_full[accept_socket] = true;
				}
			}
			if (this->_request_is_full[accept_socket] == true) {
				this->_request[accept_socket].parse(this->_read_buf[accept_socket]);
				this->_request_params[accept_socket]._if_length = false;
				this->_request_params[accept_socket]._length = 0;
				this->_request_params[accept_socket]._if_type = false;
				this->_request_params[accept_socket]._size = 0;
				this->_request_params[accept_socket]._type = 0;
				this->_request_params[accept_socket]._pos = 0;
				this->_read_buf.erase(accept_socket);
			}
		}
	}
	
	/*
		Отправка ответа
	*/
	void	Server::send(long& accept_socket) {
		static long s = 0;
		long size = 0;
		if (this->_send_pos[accept_socket] == 0)
			this->_response[accept_socket].initialisation(this->_request[accept_socket]);
		size = this->_response[accept_socket].getAsk().size() - this->_send_pos[accept_socket];
		if (size > TCP_SIZE)
			size = TCP_SIZE;
		size_t	ret = ::send(accept_socket, &this->_response[accept_socket].getAsk().c_str()[this->_send_pos[accept_socket]], size, 0);
		this->_send_pos[accept_socket] += ret;
		if (ret < 0)
			throw cmalt::BaseException("\rSend error, closing connection", 0);
		if (!this->_request[accept_socket].getConnection())
			throw cmalt::BaseException("\rConnection closed", 0);
		if (this->_send_pos[accept_socket] >= this->_response[accept_socket].getAsk().size()) {
			this->_send_pos[accept_socket] = 0;
			throw cmalt::BaseException("\rSend full ask", 1);
		}
	}

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Вспомогательные методы
	*/

	/*
		Проверка значение хедера Transfer-Encoding
	*/
	bool	Server::chunked_detect(size_t& pos, long& accept_socket) {
		/*
			Пропускаются все пробелы после "Transfer-Encoding:" и если там "chunked", то возвращается true
		*/
		pos += 18 + cmalt::skipspace(this->_read_buf[accept_socket], pos + 18);
		std::string transfer = this->_read_buf[accept_socket].substr(pos, 7);
		return transfer == "chunked";
	}

	/*
		Получение позиции начала числового значения длины тела запроса. Если есть хедер "Content-Length"
	*/
	size_t	Server::request_content_length_start_pos(size_t& pos, long& accept_socket) {
		pos = cmalt::skipspace(this->_read_buf[accept_socket], pos + 15);
		return pos;
	}

	/*
		Получение позиции конца числового значения длины тела запроса. Если есть хедер "Content-Length"
	*/
	size_t	Server::request_content_length_end_pos(size_t& pos, long& accept_socket) {
		size_t	i;
		for (i = pos; std::isdigit(this->_read_buf[accept_socket][i]); ++i) {}
		return i;
	}

}
//================================================================================
