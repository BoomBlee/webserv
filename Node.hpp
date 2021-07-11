#pragma once
#include "Server.hpp"
#include <cstddef>
#include <string>


//переписать комментарии в интерфейсе класса, над каждой функцией написать что она делает отдельным коментарием

//================================================================================
namespace third {
	
	class	Node {
	private:
		/*
			Переменные отвечающие за сервера
				_listen_servers - мапа содержащая объект класса Server и в качестве ключа его _listen_socket
				_accept_servers - мапа содержащая указатель на объект класса Server и в качестве ключа сокет полученный после метода accept
				_recv_servers - мапа содержащая указатель на объект класса Server и в качестве ключа сокет полученный после метода recv
				_config - объект класса ConfigServers содержащий информацию о всех серверах которые были указаны в конфиге
		*/
		std::map<long, Server>	_listen_servers;
		std::map<long, Server*>	_accept_servers;
		std::map<long, Server*>	_recv_servers;
		kyoko::ConfigServers	_config;
	public:
		/*
			Конструкторы:
				стандартный
				с конфигом в качестве параметра
				со строкой содежащей путь к конигу в качестве параметра
				копирующий
			Деструктор
			Операторы:
				присваивания
		*/
		Node();
		Node(kyoko::ConfigServers&);
		Node(std::string&);
		Node(const Node&);
		~Node();
		Node&	operator=(const Node&);
		/*
			Геттеры
		*/
		std::map<long, Server>&	get_listen_servers();
		Server&	get_listen_servers(long);
		std::map<long, Server*>&	get_accept_servers();
		Server&	get_accept_servers(long);
		std::map<long, Server*>&	get_recv_servers();
		Server&	get_recv_servers(long);
		kyoko::ConfigServers&	get_config();
		/*
			Сеттеры
		*/
		void	set_listen_servers(std::map<long, Server>&);
		void	set_listen_server(Server&, long);
		void	set_accept_servers(std::map<long, Server*>&);
		void	set_accept_server(Server&, long);
		void	set_recv_servers(std::map<long, Server*>&);
		void	set_recv_server(Server&, long);
		void	set_config(kyoko::ConfigServers&);
		/*
			Методы работы ноды (кластера)
				start_node(строка содержащая путь к файлу .conf) - запускает парсер конфига и создает по информации из него сервера 
				run_node - запускает цикл принятия запросов и отправки ответов для всех серверов хранящихся в _listen_servers, _accept_servers и _recv_servers
		*/
		void	start_node(std::string&);
		void	start_node();
		void	run_node();
	private:
		/*
			Вспомогательные методы:
				new_server(объект класса ConfigServer) - метод создающий новый объект класса Server и добавляющий его в _listen_servers
		*/
		void	new_server(kyoko::ConfigServer&);
		void	poll_error(int&, struct pollfd*, std::map<long, int>&);
	};

}
//================================================================================