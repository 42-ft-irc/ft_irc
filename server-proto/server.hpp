#pragma once
#include <iostream>
#include <sys/socket.h>	// socket(), bind(), listen(), accept()
#include <netinet/in.h>	// sockaddr_in, htons()
#include <poll.h>
#include <vector>
#include <unistd.h>		// close()
#include <fcntl.h>

class server
{
	private:
		int							_listener_fd;
		int							_port;
		struct sockaddr_in			_address;
		std::vector<struct pollfd>	_fds;
	public:
		server( void );
		server( int port );
		~server();

		// main func
		int		startServer( void );
		int		runServerLoop( void );

		// helper func
		void	setAdress( void );
		void	addFD( int new_fd );
};
