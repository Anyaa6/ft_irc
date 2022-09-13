/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llecoq <llecoq@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/26 15:04:46 by llecoq            #+#    #+#             */
/*   Updated: 2022/09/13 15:59:24 by llecoq           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cout << "USAGE : ./ircserv <port number> <password>" << std::endl; 
		exit(EXIT_FAILURE);
	}
	Server	server(argv[1], argv[2]);

	// mini parsing port // mdp ?
	try
	{
		server.init();
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	exit (EXIT_SUCCESS);
}	