#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "libraries.hpp"

class client
{
	private:
		int			_fd;
		std::string _nickname;
		std::string _username;
		std::string	_recv_buffer;

		bool		_isRegistered;

	public:
		client( int fd );
		~client();

		int					getFD() const;
		const std::string&	getRecvBuffer( void ) const;
		void				appendToBuffer( const std::string &data );
		void				eraseFromBuffer( size_t len );

		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		bool				isRegistered() const;

		void				setNickname(const std::string& nick);
		void				setUsername(const std::string& user);
		void				setRegistered(bool status);
};

#endif
