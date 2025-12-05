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
		int										_port;
		std::string								_password;
		int										_listenerFd;

		std::vector<struct pollfd>				_pollFds;
		std::map<int, client*>					_clients;
		std::map<std::string, channel*>			_channels;
		std::map<std::string, CommandHandler>	_commands;

		server();
		server(const server &src);
		server &operator=(const server &src);

		void		createSocket();
		void		bindAndListen();
		void		acceptNewClient();
		void		handleClientData(int fd, size_t &i);
		void		removeFromPoll(int fd);

	public:
		server( int port, const std::string& password );
		~server();

		// core
		void		initCommands();
		void		run();

		// client/channel managing
		void		removeClient(int fd);
		client* 	findClientByNick(const std::string& nick);
		channel*	getChannel(const std::string& name);
		channel*	createChannel(const std::string& name, const std::string& key, client* creator);
		void		removeChannel(const std::string& name);

		// cmd exec
		message	parseMessage(const std::string &raw_data) const;
		void	executeCommand(int fd, message &msg);

		// utils
		void		setAddress(void);
		void		addFD(int new_fd);
		void		processClientCommands( int fd );
		channel*	getChannelForMode(int fd, message &msg, client* sender);

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
		void	handleKick(int fd, message &msg);
		void	handleInvite(int fd, message &msg);

		// command util
		void	welcomeClient( int fd );
		void	sendReply(int fd, const std::string& msg);

		// exception class
		class ServerException : public std::runtime_error {
			public:
				ServerException(const std::string &msg) 
					: std::runtime_error(msg + ": " + std::strerror(errno)) {}
				ServerException(const std::string &msg, bool useErrno) 
					: std::runtime_error(useErrno ? (msg + ": " + std::strerror(errno)) : msg) {}
		};
};

#endif