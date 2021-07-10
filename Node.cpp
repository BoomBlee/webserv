#include "Node.hpp"
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <string>
#include <sys/poll.h>
#include <unistd.h>

//================================================================================
namespace third {

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Конструкторы, Деструктор и перегрузка операторов
	*/

	/*
		Стандартный конструктор
	*/
	Node::Node() {}

	/*
		Конструктор с конфигом в качастве параметра
	*/
	Node::Node(kyoko::ConfigServers& config) : _config(config) {}

	/*
		Конструктор с конфигом в качастве параметра
	*/
	Node::Node(std::string& path_to_config) {
		this->_config.add(path_to_config);
	}

	/*
		Конструктор копирования
	*/
	Node::Node(const Node& src_node) {
		*this = src_node;
	}

	/*
		Деструктор
	*/
	Node::~Node() {}

	/*
		Перегрузка оператора присванивание
	*/
	Node&	Node::operator=(const Node& src_node) {
		this->_config = src_node._config;
		this->_listen_servers = src_node._listen_servers;
		this->_accept_servers = src_node._accept_servers;
		this->_recv_servers = src_node._recv_servers;
		return *this;
	}

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Геттеры
	*/

	/*
		Гетер для _listen_servers
	*/
	std::map<long, Server>&	Node::get_listen_servers() {
		return this->_listen_servers;
	}

	Server&	Node::get_listen_servers(long socket) {
		return this->_listen_servers[socket];
	}

	/*
		Гетер для _accept_servers
	*/
	std::map<long, Server*>&	Node::get_accept_servers() {
		return this->_accept_servers;
	}

	Server&	Node::get_accept_servers(long socket) {
		return *(this->_accept_servers[socket]);
	}

	/*
		Гетер для _recv_servers
	*/
	std::map<long, Server*>&	Node::get_recv_servers() {
		return this->_recv_servers;
	}

	Server&	Node::get_recv_servers(long socket) {
		return *(this->_recv_servers[socket]);
	}

	/*
		Гетер для _config
	*/
	kyoko::ConfigServers&	Node::get_config() {
		return this->_config;
	}

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Сеттеры
	*/

	/*
		Сеттер для _listen_servers
	*/
	void	Node::set_listen_servers(std::map<long, Server>& servers) {
		this->_listen_servers = servers;
	}

	void	Node::set_listen_server(Server& server, long socket) {
		this->_listen_servers[socket] = server;
	}

	/*
		Сеттер для _accept_servers
	*/
	void	Node::set_accept_servers(std::map<long, Server*>& servers) {
		this->_accept_servers = servers;
	}

	void	Node::set_accept_server(Server& server, long socket) {
		this->_accept_servers[socket] = &server;
	}

	/*
		Сеттер для _recv_servers
	*/
	void	Node::set_recv_servers(std::map<long, Server*>& servers) {
		this->_recv_servers = servers;
	}

	void	Node::set_recv_server(Server& server, long socket) {
		this->_recv_servers[socket] = &server;
	}

	/*
		Сеттер для _config
	*/
	void	Node::set_config(kyoko::ConfigServers& config) {
		this->_config = config;
	}
	
	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Методы работы ноды (кластера)
	*/

	void	Node::start_node(std::string& path_to_config) {
		this->_config.add(path_to_config);
		std::vector<kyoko::ConfigServer>	vector_servers = this->_config.getConfigsServer();
		for (std::vector<kyoko::ConfigServer>::iterator	iter = vector_servers.begin(); iter != vector_servers.end(); ++iter) {
			try {
				this->new_server(*iter);
			}
			catch (cmalt::BaseException &e) {
				std::cerr << " " << e.what() << std::endl;
			}
		}
	}

	void	Node::start_node() {
		std::vector<kyoko::ConfigServer>	vector_servers = this->_config.getConfigsServer();
		for (std::vector<kyoko::ConfigServer>::iterator	iter = vector_servers.begin(); iter != vector_servers.end(); ++iter) {
			try {
				this->new_server(*iter);
			}
			catch (cmalt::BaseException &e) {
				std::cerr << " " << e.what() << std::endl;
			}
		}
	}

	void	Node::run_node() {
		int		sockets_size = this->_listen_servers.size();
		struct	pollfd	poll_fds[sockets_size * 2];
		std::map<long, int>	num_fds;
		int	count = 0;
		for (std::map<long, Server>::iterator iter = this->_listen_servers.begin();iter != this->_listen_servers.end(); ++iter) {
			poll_fds[count].fd = iter->first;
			poll_fds[count].events = POLLIN;
			num_fds[iter->first] = count;
			++count;
		}
		while (true) {
			bool	pool = true;
			while (pool) {
				std::cout << "\rWaiting on a connection" << std::flush;
				int ret = poll(poll_fds, sockets_size, 10000000);
				if (ret < 0)
					this->poll_error(sockets_size, poll_fds, num_fds);
				if (ret > 0)
					pool = false;
			}

			for (std::map<long, Server*>::iterator iter = this->_recv_servers.begin(); iter != this->_recv_servers.end() && !pool; ++iter) {
				int	num = num_fds[iter->first];
				if (poll_fds[num].revents & POLLOUT) {
					long	fd = iter->first;
					poll_fds[num].revents = 0;
					pool = true;
					try {
						iter->second->send(fd);
					}
					catch (cmalt::BaseException &e) {
						if (e.getErrorNumber() == 0) {
							std::cerr << e.what() << std::endl;
							this->_recv_servers.erase(fd);
							this->_accept_servers.erase(fd);
							if (fd > 0)
								close(fd);
						}
						else
							this->_recv_servers.erase(iter);
					}
				}
			}

			for (std::map<long, Server*>::iterator iter = this->_accept_servers.begin(); !pool && iter != this->_accept_servers.end(); ++iter) {
				int num = num_fds[iter->first];
				if (poll_fds[num].revents & POLLIN) {
					long	fd = iter->first;
					poll_fds[num].revents = 0;
					pool = true;
					try {
						iter->second->recv(fd);
						if (iter->second->get_request_is_full(fd)) {
							this->_recv_servers[iter->first] = iter->second;
							iter->second->set_request_is_full(false, fd);
						}
					}
					catch (cmalt::BaseException &e) {
						std::cerr << e.what() << std::endl;
						this->_accept_servers.erase(fd);
						if (fd > 0)
							close(fd);
					}
				}
			}

			for (std::map<long, Server>::iterator it = this->_listen_servers.begin(); pool == false && it != this->_listen_servers.end(); ++it) {
				int num = num_fds[it->first];
				if (poll_fds[num].revents & POLLIN) {
					poll_fds[num].revents = 0;
					pool = true;
					try {
						long fd = it->second.accept();
						poll_fds[sockets_size].fd = fd;
						poll_fds[sockets_size].events = POLLIN | POLLOUT;
						num_fds[fd] = sockets_size;
						sockets_size++;
						this->_accept_servers[fd] = &it->second;
					}
					catch (cmalt::BaseException &e) {
						std::cerr << e.what() << std::endl;
					}
				}
			}
			pool = true;
		}
	}

	/*
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Вспомогательные методы
	*/
	
	void	Node::new_server(kyoko::ConfigServer& config) {
		Server	server(config);
		long	socket = server.get_listen_socket();
		this->_listen_servers[socket] = server;
	}

	void	Node::poll_error(int& sockets_size, struct pollfd* poll_fds, std::map<long, int>& num_fds) {
		std::cerr << "Poll error" << std::endl;
		for (std::map<long, Server*>::iterator iter = this->_accept_servers.begin(); iter != this->_accept_servers.end(); ++iter) {
			long	fd = iter->first;
			int	num = num_fds[fd];
			if (fd > 0)
				close(fd);
			poll_fds[num].fd = 0;
			poll_fds[num].events = 0;
			poll_fds[num].revents = 0;
			num_fds.erase(fd);
		}
		sockets_size -= this->_accept_servers.size();
		this->_accept_servers.clear();
		this->_recv_servers.clear();
	}
	
}