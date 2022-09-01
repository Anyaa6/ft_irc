#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(const char* port, const char* password)
:
	_server_info(port),
	_exec(password)
{
	// if (port > 1024)
		// exit zobi ?
	// if password ... exit ?
	_pollfd.reserve(10);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
	_close_all_fds();
}

/*
** ---------------------------------- MEMBERS ---------------------------------
*/

void	Server::init()
{
	_get_address_info();
	_get_listening_socket();
	_listen_for_incoming_connections();		
}

void	Server::run()
{
	while (1) 					// main loop
	{
		_poll_events(); 		// wait for events
		for (pollfd_iterator it = _pollfd.begin(); it < _pollfd.end(); it++)
		{
			switch (_find_event(*it))
			{
			case PENDING_CONNECTION:
				_log("PENDING CONNECTION");
				_accept_pending_connection();
				break;
			case CONNECTION_LOST:
				_log("CONNECTION LOST");
				_close_connection(it);
				break;
			case DATA_RECEIVED:
				_log("DATA RECEIVED");
				_process_data(it);
				break;
			case NO_EVENT:
				break;
			}
		}	
	}
}