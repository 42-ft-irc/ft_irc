#include "server.hpp"

void	server::executeMessage( int sender_fd, message &msg) {
	std::map<std::string, CommandHandler>::iterator it = _commands.find(msg.command);

	if (it != _commands.end())
		(this->*(it->second))(sender_fd, msg);
	else
		std::cout << "Unhandled command: " << msg.command << std::endl;
}