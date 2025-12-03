#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "libraries.hpp"

class client
{
	private:
		int			_fd;
		std::string _nickname;
		std::string _username;
		
		bool		_isRegistered;

	public:
		client( int fd );
		~client();

		int	getFD() const ;
};

#endif