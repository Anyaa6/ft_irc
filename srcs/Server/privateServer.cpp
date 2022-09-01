/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_Server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llecoq <llecoq@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/26 10:57:01 by llecoq            #+#    #+#             */
/*   Updated: 2022/08/30 12:00:53 by llecoq           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

# include <sys/types.h>
// # include <sys/socket.h>
# include <netdb.h>
# include <cstring>
// # include <errno.h>  
# include <arpa/inet.h>

# define BACKLOG 	10 
# define NO_TIMEOUT -1

enum	e_errors
{
	PERROR,
	NO_PERROR,
	ERR_BIND = -2,
	ERR_SOCKET = -3,
	ERR_SETSOCKOPT = -4
};

/*
** ----------------------------------- INIT -----------------------------------
*/

void	Server::_get_address_info()
{
	int					status;
	struct addrinfo 	hints;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	status = getaddrinfo(NULL, _server_info.port, &hints, &_server_info.servinfo);
	if (status != 0)
	{
		// throw serverExceptions("getaddrinfo error: ", gai_strerror(status));
		std::string	err_msg("getaddrinfo error: ");
		
		err_msg.append(gai_strerror(status));
		_err_log(err_msg);
		exit(EXIT_FAILURE);
	}
}

void	Server::_get_listening_socket()
{
	struct addrinfo*	ptr;

    // loop through all the results and bind to the first we can
    for (ptr = _server_info.servinfo; ptr != NULL; ptr = ptr->ai_next)
	{
		if (_create_and_bind_socket(ptr) > 0)
		{
			_add_socket_to_pollfd(_server_info.listening_socket);
			break;
		}
	}
	freeaddrinfo(_server_info.servinfo);
	if (ptr == NULL)
		_error_exit(NO_PERROR, "Couldn't create socket and bind to the port. Server shutting down.");
}

int	Server::_create_and_bind_socket(addrinfo* ptr)
{
	int		opt = true;
	int		socket_fd = 0;

	if ((socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == FAILED)
	{
		perror("Server: socket");
		_err_log("Trying to create and bind another socket...");
		return ERR_SOCKET;
	}
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == FAILED)
	{
		perror("setsockopt");
		_err_log("Trying to bind to the socket anyway...");
	}
	if (bind(socket_fd, ptr->ai_addr, ptr->ai_addrlen) == FAILED)
	{
		close(socket_fd);
		perror("Server: bind");
		_err_log("Trying to create and bind another socket...");
		return ERR_BIND;
	}
	_server_info.listening_socket = socket_fd;
	return socket_fd;
}

void	Server::_add_socket_to_pollfd(int socket_fd)
{
	pollfd	new_pollfd = {socket_fd, POLLIN, 0};

	_pollfd.push_back(new_pollfd);
}

void	Server::_listen_for_incoming_connections()
{
	std::string		log_msg("Server: waiting for connections on port ");
	std::string		err_msg("Server: listen ");

	if (listen(_server_info.listening_socket, BACKLOG) == -1)
		_error_exit(NO_PERROR, err_msg.append(strerror(errno)));
	else
	{
		log_msg.append(_server_info.port);
		_log(log_msg);
	}
}

/*
** ----------------------------------- RUN -----------------------------------
*/

void	Server::_signal_handler(int signum)
{
	(void)signum;
	throw serverExceptions("Server: ", "shutting down...");
}

void	Server::_poll_events()
{
	int				pfds;

	signal(SIGINT, _signal_handler);
	pfds = poll(_pollfd.data(), _pollfd.size(), NO_TIMEOUT);
	if (pfds == FAILED)
		_error_exit(PERROR, "Server: poll");
}

int	Server::_find_event(struct pollfd current_pollfd)
{
	if (current_pollfd.revents & POLLIN)
	{
		if (current_pollfd.fd == _server_info.listening_socket)
			return PENDING_CONNECTION;
		else
		{
			Client*	client = _exec.get_client(current_pollfd.fd);
			ssize_t	nbytes = client->read_data();

			if (nbytes > 1) // ignore empty msg
				return DATA_RECEIVED;
			else if (nbytes == 0)
				return CONNECTION_LOST;
		}
	}
	return NO_EVENT;
}

void	Server::_accept_pending_connection()
{
	int						new_fd;
	sockaddr_storage		client_addr;
    socklen_t 				addrlen;
	char* 					ipstr;
    
	addrlen = sizeof client_addr;
	new_fd = accept(_server_info.listening_socket, (struct sockaddr *)&client_addr, &addrlen);
	fcntl(new_fd, F_SETFL, O_NONBLOCK); // set fd to non blocking
	if (new_fd > 0)
	{
		_log("New connection accepted !");
		_add_socket_to_pollfd(new_fd);
		ipstr = _sockaddr_to_string(client_addr);
		_exec.init_client(new_fd, ipstr);
		// ExecutionManager::exec.init_client(fd, ipstr);
		// _add_client_to_book(new_fd, ipstr);
	}
	else
		perror("Server: accept");
}

char*	Server::_sockaddr_to_string(sockaddr_storage client_addr)
{
	static char	ipstr[INET6_ADDRSTRLEN];

	// deal with both IPv4 and IPv6:
	if (client_addr.ss_family == AF_INET)
	{
		struct sockaddr_in *s = (struct sockaddr_in *)&client_addr;
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	}
	else
	{ // AF_INET6
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&client_addr;
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
	return ipstr;
}

void	Server::_process_data(pollfd_iterator it)
{
	(void)it;
	// std::cout << *(_client_book.find(it->fd)->second) << std::endl; // print client data for debug

	// std::string	nickname("llecoq");
	// std::string	msg(RPL_WELCOME(nickname));

	// send(it->fd, msg.c_str(), msg.size(), 0 );

	//command_book
	_exec.run(_exec.get_client(it->fd));
}

void	Server::_close_connection(pollfd_iterator it)
{
	std::cout << "Connection closed by client " << it.base()->fd << std::endl;
	close(it.base()->fd);
	_pollfd.erase(it);
}

/*
** --------------------------------- ERROR / LOG ---------------------------------
*/

void	Server::_error_exit(int error, std::string error_msg)
{
	if (error == PERROR)
		perror(error_msg.c_str());
	else if (error_msg.empty() != 1)
		_err_log(error_msg);
	exit(EXIT_FAILURE);
}

void	Server::_log(std::string log_msg)
{
	std::cout << log_msg << std::endl;
}

void	Server::_err_log(std::string err_msg)
{
	std::cerr << err_msg << std::endl;
}

void	Server::_close_all_fds()
{
	pollfd_iterator	it;

	for (it = _pollfd.begin(); it != _pollfd.end(); it++)
		close((*it).fd);	
}