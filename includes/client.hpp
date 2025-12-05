#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "libraries.hpp"

class client
{
	private:
		// Disabled
		client();
		client(const client& src);
		client& operator=(const client& src);

		// Client info
		int			_fd;
		std::string	_nickname;
		std::string	_username;
		std::string	_recvBuffer;

		// State flags
		bool	_isAuthenticated;
		bool	_isRegistered;

	public:
		// Constructors & Destructor
		client(int fd);
		~client();

		// Getters
		int					getFD() const;
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		const std::string&	getRecvBuffer() const;
		bool				isAuthenticated() const;
		bool				isRegistered() const;

		// Setters
		void	setNickname(const std::string& nick);
		void	setUsername(const std::string& user);
		void	setAuthenticated(bool status);
		void	setRegistered(bool status);

		// Buffer management
		void	appendToBuffer(const std::string& data);
		void	clearBuffer();
		void	eraseFromBuffer(size_t len);
};

#endif
