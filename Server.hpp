#pragma once
#include "Response.hpp"
#include <cstddef>
#include <map>
#include <string>


//переписать комментарии в интерфейсе класса, над каждой функцией написать что она делает отдельным коментарием


//================================================================================
namespace third {

	class	Server {
	private:
		/*
			Переменные отвечающие за сервер:
				_addr - стандартная структура содержащая информацию для сокетов
				_config - объект класс ConfigServer содержащий информацию из конфига, такую как host, port и т.д.
				_listen_socket - слушающий сокет, способен принимать сигналы чтобы понять о появлении клиента желающего получить подключение
				_request - мапа объектов класса Request, которые отвечают за обработку запроса своего сокета (сокет ключ)
				_read_buf - мапа строк в которые записываются запросы их сокетов (сокет ключ)
				_response - мапа объектов класса Response, оторые отвечают за написание ответов для своего сокета (сокет ключ)
				_request_is_full - мапа переменных отвечающих за то, что запрос их сокета получен полностью (сокет ключ)
		*/
		struct	sockaddr_in	_addr;
		kyoko::ConfigServer	_config;
		long	_listen_socket;
		std::map<long, cmalt::Request>	_request;
		std::map<long, std::string>	_read_buf;
		std::map<long, cmalt::Response>	_response;
		std::map<long, bool>	_request_is_full;
		std::map<long, size_t>	_size_send;
	public:
		/*
			Конструкторы:
				стандартный
				с конфигом
				копирующий
			Деструктор
			Операторы:
				присваивания
		*/
		Server();
		Server(kyoko::ConfigServer&);
		Server(const Server&);
		~Server();
		Server&	operator=(const Server&);
		/*
			Геттеры
		*/
		struct	sockaddr_in&	get_sock_addr();
		kyoko::ConfigServer&	get_config();
		long&	get_listen_socket();
		cmalt::Request&	get_request(long);
		std::map<long, cmalt::Request>&	get_request();
		std::string&	get_read_buf(long);
		std::map<long, std::string>&	get_read_buf();
		cmalt::Response&	get_response(long);
		std::map<long, cmalt::Response>&	get_response();
		bool&	get_request_is_full(long);
		std::map<long, bool>&	get_request_is_full();
		/*
			Сеттеры
		*/
		void	set_sock_addr(struct	sockaddr_in&);
		void	set_config(kyoko::ConfigServer&);
		void	set_listen_socket(long&);
		void	set_request(cmalt::Request&, long);
		void	set_request(std::map<long, cmalt::Request>&);
		void	set_read_buf(std::string&, long);
		void	set_read_buf(std::map<long, std::string>&);
		void	set_response(cmalt::Response&, long);
		void	set_response(std::map<long, cmalt::Response>&);
		void	set_request_is_full(bool, long);
		void	set_request_is_full(std::map<long, bool>&);
		/*
			Методы работы сервера
				accept - производит соединение сокета
				recv(сокет принимающий запросы) - принимает запрос и отправляет его в _request на обработку
				send(сокет отправляющий ответы) - получает ответ из _response и отправляет его клиенту
				start_listening_host - создает слушающий сокет, который ожидает соединение или бросает исключение в случае ошибки
		*/
		long	accept();
		void	recv(long&);
		void	send(long&);
		void	start_listening_host();
	private:
		/*
			Вспомогательные методы:
				chunked_detect(позиция в строке запроса) - метод помогающий определить используется ли чанки в данном запросе
				request_content_length_start_pos - метод помогающий найти позицию начала числового значения длины тела запроса
				request_content_length_end_pos - метод помогающий найти позицию конца числового значения длины тела запроса
		*/
		bool	chunked_detect(size_t&, long&);
		size_t	request_content_length_start_pos(size_t&, long&);
		size_t	request_content_length_end_pos(size_t&, long&);
	};

}
//================================================================================
