#include "server.hpp"

void	server::handleJoin( int fd, message &msg ) {
	std::string channelName = msg.params[0];
	std::string key = (msg.params.size() > 1) ? msg.params[1] : "";
	client* user = _clients[fd];

	if (msg.params.empty()) {
		sendReply(fd, " JOIN :Not enough parameters");
		return;
	}

	if (channelName[0] != '#') {
		sendReply(fd, " " + channelName + " :No such channel");
		return;
	}
}


