#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "libraries.hpp"

class client
{
	private:
		int			_fd;
		std::string _nickname;
		std::string _username;
		std::string	_recvBuffer;

		bool		_isAuthenticated;
		bool		_isRegistered;

		client();
		client(const client &src);
		client &operator=(const client &src);

	public:
		client( int fd );
		~client();

		int					getFD() const;
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		const std::string&	getRecvBuffer( void ) const;
		bool				isAuthenticated() const;
		bool				isRegistered() const;

		void				setNickname(const std::string& nick);
		void				setUsername(const std::string& user);
		void				setAuthenticated(bool status);
		void				setRegistered(bool status);

		void				appendToBuffer( const std::string &data );
		void				clearBuffer();
		void				eraseFromBuffer( size_t len );
};

#endif
