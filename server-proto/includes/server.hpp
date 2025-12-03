#ifndef SERVER_HPP
#define SERVER_HPP
#include "libraries.hpp"
#include "message.hpp"
#include "client.hpp"
#include "channel.hpp"

class server
{
	private:
		int							_listener_fd;
		int							_port;
		struct sockaddr_in			_address;
		std::vector<struct pollfd>	_fds;
		std::map<int, client*>		_clients;

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

		class ServerException : public std::exception {
			private:
				const char* _msg;
			public:
				ServerException(const char* msg) throw() : _msg(msg) {}
				virtual ~ServerException() throw() {}
				virtual const char* what() const throw() {
					return _msg;
				}
		};
};

#endif