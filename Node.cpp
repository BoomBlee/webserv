#include "Node.hpp"
#include "color.hpp"
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <ostream>
#include <string>
#include <sys/poll.h>
#include <sys/select.h>
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
		std::vector<kyoko::ConfigServer>	vector_servers = this->_config.getConfigServer();
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
		std::vector<kyoko::ConfigServer>	vector_servers = this->_config.getConfigServer();
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
		std::map<long, struct pollfd>	all_fds;
		struct	pollfd	poll_fds[sockets_size * 1001];
		std::map<long, int>	num_fds;
		int	count = 0;
		for (std::map<long, Server>::iterator iter = this->_listen_servers.begin();iter != this->_listen_servers.end(); ++iter) {
			all_fds[iter->first].fd = iter->first;
			all_fds[iter->first].events = POLLIN;
			num_fds[iter->first] = count;
			++count;
		}
		struct timeval	start2, end2;
		while (true) {
			bool	pool = true;
			count = 0;
			for (std::map<long, struct pollfd>::iterator iter = all_fds.begin(); iter != all_fds.end(); ++iter) {
				poll_fds[count].fd = iter->second.fd;
				poll_fds[count].events = iter->second.events;
				poll_fds[count].revents = iter->second.revents;
				++count;
			}
			while (pool) {
				int ret = poll(poll_fds, all_fds.size(), 10000);
				if (ret < 0)
					this->poll_error(sockets_size, poll_fds, num_fds);
				if (ret > 0)
					pool = false;
			}
			for (int i = 0; i < count; ++i) {
				long fd = poll_fds[i].fd;
				all_fds[fd].fd = poll_fds[i].fd;
				all_fds[fd].events = poll_fds[i].events;
				all_fds[fd].revents = poll_fds[i].revents;
			}
			std::map<long, Server*>::iterator iter;
			for (iter = this->_recv_servers.begin(); iter != this->_recv_servers.end() && !pool; ++iter) {
				long	fd = iter->first;
				if (all_fds[fd].revents & POLLOUT) {
					all_fds[fd].revents = 0;
					pool = true;
					this->_accept_servers.erase(fd);
					try {
						iter->second->send(fd);
					}
					catch (cmalt::BaseException &e) {
						if (e.getErrorNumber() == 0) {
							std::cerr << e.what() << std::endl;
							all_fds.erase(fd);
							this->_recv_servers.erase(fd);
							this->_accept_servers.erase(fd);
							if (fd > 0)
								close(fd);
						}
						else {
							this->_accept_servers[fd] = iter->second;
							this->_recv_servers.erase(fd);
							gettimeofday(&end2, NULL);
							long long t1 = (end2.tv_sec * 1000 + end2.tv_usec / 1000) - (start2.tv_sec * 1000 + start2.tv_usec / 1000);
							std::cout << CIAN << "Response-full:" << t1 / 1000 << " " << fd << RESET << std::endl;
							std::cout << RED << "Time: " << t1 << "ms" << RESET << std::endl;
							gettimeofday(&start2, NULL);
						}
					}
					break;
				}
			}

			for (iter = this->_accept_servers.begin(); !pool && iter != this->_accept_servers.end(); ++iter) {
				long	fd = iter->first;
				if (all_fds[fd].revents & POLLIN) {
					all_fds[fd].revents = 0;
					pool = true;
					try {
						std::cout << CIAN << "REQUEST" << RESET << std::endl;
						iter->second->recv(fd);
						if (iter->second->get_request_is_full(fd)) {
							std::cout << CIAN << "Request-full:" << fd << RESET << std::endl;
							this->_recv_servers[iter->first] = iter->second;
							iter->second->set_request_is_full(false, fd);
						}
					}
					catch (cmalt::BaseException &e) {
						std::cerr << e.what() << std::endl;
						all_fds.erase(fd);
						this->_accept_servers.erase(fd);
						this->_recv_servers.erase(fd);
						if (fd > 0)
							close(fd);
					}
					break;
				}
			}

			for (std::map<long, Server>::iterator it = this->_listen_servers.begin(); pool == false && it != this->_listen_servers.end(); ++it) {
				long	fd = it->first;
				if (all_fds[fd].revents & POLLIN) {
					all_fds[fd].revents = 0;
					pool = true;
					try {
						long fd = it->second.accept();
						all_fds[fd].fd = fd;
						all_fds[fd].events = POLLIN | POLLOUT;
						this->_accept_servers[fd] = &it->second;
					}
					catch (cmalt::BaseException &e) {
						std::cerr << e.what() << std::endl;
					}
					break;
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