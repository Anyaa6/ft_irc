#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Exceptions.hpp"
// # include "Common.hpp"
# include <unistd.h>
# include <sys/socket.h>
# include <string>
# include <iostream>
# include <errno.h>
# include <map>
# include <cstdio>

# define CLOSED -1
# define FAILED -1

struct t_recv_data
{
	char 	buf[512]; //512 ? Check if msg is >512 what to do since buffer will already be full
	ssize_t nbytes;
};

class Client
{

	public:

		Client();
		Client( int fd );
		Client( Client const & src );
		~Client();

		Client &		operator=( Client const & rhs );
		// send_priv_msg(); //??

		// methods
		ssize_t				read_data();
		void				init_client(char* ipstr);

		// accessor
		void				set_fd(int fd);
		void				set_username(std::string username);
		void				set_realname(std::string realname);
		void				set_nickname(std::string nickname);
		void				set_ipstr(std::string ipstr);
		int					get_fd() const;
		std::string			get_username() const;
		std::string			get_realname() const;
		std::string			get_nickname() const;
		std::string			get_ipstr() const;

		// debug
		void				display_recv_data() const;

	private:

		int				_fd;
		std::string 	_username;
		std::string 	_realname;
		std::string 	_nickname;
		std::string		_ipstr;
		t_recv_data 	_recv_data;


		// std::deque<std::string pending_msg> _pending_priv_msg; //pending_msg must contain the sender's username //deque or other ? must be FIFO

		// std::map<std::string channel_name, Channel *> _joined_channels;
		// // struct pollfd	_pollfd;

};

std::ostream &			operator<<( std::ostream & o, Client const & i );

#endif /* ********************************************************** CLIENT_H */
