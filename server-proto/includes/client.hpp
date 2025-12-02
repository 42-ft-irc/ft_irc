#pragma once
#include "libraries.hpp"

class client
{
	private:
		int			_fd;
		std::string _nickname;
		std::string _username;
		
		bool		_isRegistered;

	public:
		client(/* args */);
		~client();
};