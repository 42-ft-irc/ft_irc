#ifndef SERVER_HPP
#define SERVER_HPP
#include "libraries.hpp"
#include "message.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "constants.hpp"

class server;
typedef void (server::*CommandHandler)(int, message&);

class server
{
	private:
		int										_listener_fd;
		int										_port;
		std::string								_password;
		struct sockaddr_in						_address;
		std::vector<struct pollfd>				_fds;
		std::map<int, client*>					_clients;
		std::map<std::string, channel*>			_channels;
		std::map<std::string, CommandHandler>	_commands;

	public:
		server( int port, const std::string& password );
		~server();

		// main func
		int		startServer( void );
		int		runServerLoop( void );

		// message func
		message	parseMessage( const std::string &raw_data ) const;
		void	executeMessage( int sender_fd, message &msg);

		// helper func
		void		setAddress( void );
		void		addFD( int new_fd );
		int			removeClient( int fd, int i );
		void		processClientCommands( int fd );
		channel*	getChannelForMode(int fd, message &msg, client* sender);

		// command helpers
		void	welcomeClient( int fd );
		void	initCommands( void );
		void	sendReply(int fd, const std::string& msg);
		client* findClientByNick(const std::string& nick);

		// server commands
		void	handlePass( int fd, message &msg );
		void	handleCap( int fd, message &msg );
		void	handleNick( int fd, message &msg );
		void	handleUser( int fd, message &msg );
		void	handlePing( int fd, message &msg );
		void	handleQuit( int fd, message &msg );
		void	handlePrivmsg( int fd, message &msg );
		void	handleWho(int fd, message &msg);
		// channel commands
		void	handleJoin( int fd, message &msg );
		void	handleMode( int fd, message &msg );
		void	handlePart(int fd, message &msg);
		void	handleTopic(int fd, message &msg);

		class ServerException : public std::runtime_error {
			public:
				ServerException(const std::string &msg) 
					: std::runtime_error(msg + ": " + std::strerror(errno)) {}
				ServerException(const std::string &msg, bool useErrno) 
					: std::runtime_error(useErrno ? (msg + ": " + std::strerror(errno)) : msg) {}
		};
};

#endif