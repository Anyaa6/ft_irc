/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llecoq <llecoq@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/27 15:37:24 by llecoq            #+#    #+#             */
/*   Updated: 2022/09/01 10:26:50 by llecoq           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client() {}

Client::Client(int fd)
:
	_fd(fd),
	_authentification(false)
{}

Client::Client( const Client & src )
{
	(void)src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client(){}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &				Client::operator=( Client const & rhs )
{
	(void)rhs;
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Client const & i )
{
	o << "-------------------CLIENT--------------------" << std::endl;
	o << "fd = " << i.get_fd() << std::endl;
	o << "ipstr = " << i.get_ipstr() << std::endl;
	o << "username = " << i.get_username() << std::endl;
	o << "realname = " << i.get_realname() << std::endl;
	o << "nickname = " << i.get_nickname() << std::endl;
	i.display_recv_data();
	o << "---------------------------------------------" << std::endl;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

ssize_t	Client::read_data()
{
	_recv_data.nbytes = recv(_fd, _recv_data.buf, sizeof _recv_data.buf, 0);
	if (_recv_data.nbytes == FAILED)
		perror("Client: recv");
	else if (_recv_data.nbytes > 1)
		_recv_data.buf[_recv_data.nbytes] = '\0';
	return _recv_data.nbytes;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	Client::set_fd(int fd)
{
	_fd = fd;
}

void	Client::set_authentification(std::string server_pass, std::string client_pass)
{
	if (server_pass.compare(client_pass) == 0)
		_authentification = true;
}

void	Client::set_password(std::string password)
{
	_password = password;
}

void	Client::set_username(std::string username)
{
	_username = username;
}

void	Client::set_nickname(std::string nickname)
{
	_nickname = nickname;
}

void	Client::set_realname(std::string realname)
{
	_realname = realname;
}

void	Client::set_ipstr(std::string ipstr)
{
	_ipstr = ipstr;
}

int	Client::get_fd() const
{
	return _fd;
}

bool	Client::get_authentification() const
{
	return _authentification;
}

std::string	Client::get_password() const
{
	return _password;
}

std::string	Client::get_username() const
{
	return _username;
}

std::string	Client::get_nickname() const
{
	return _nickname;
}

std::string	Client::get_realname() const
{
	return _realname;
}

std::string	Client::get_ipstr() const
{
	return _ipstr;
}

std::string	Client::get_buf() const
{
	return _recv_data.buf;
}

/*
** ----------------------------------- DEBUG -----------------------------------
*/

void	Client::display_recv_data() const
{
	std::cout << "------------------recv_data------------------" << std::endl;
	std::cout << "buffer = '" << _recv_data.buf << "'" << std::endl;
	std::cout << "nbytes = " << _recv_data.nbytes << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
}

/* ************************************************************************** */