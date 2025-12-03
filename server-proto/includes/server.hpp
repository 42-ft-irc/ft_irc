#ifndef SERVER_HPP
#define SERVER_HPP
#include "libraries.hpp"
#include "message.hpp"

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

		// message func
		message	parseMessage( const std::string &raw_data ) const;
		void	executeMessage( int sender_fd, message &msg);

		// helper func
		void	setAddress( void );
		void	addFD( int new_fd );
		int		removeClient( int fd, int i );
};

#endif
