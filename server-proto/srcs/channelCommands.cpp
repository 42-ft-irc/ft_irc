#include "server.hpp"


void	server::handleJoin( int fd, message &msg ) {
	if (msg.params.empty()) {
		sendReply(fd, ":server " ERR_NOPARAMS " " + _clients[fd]->getNickname() + " JOIN :Not enough parameters");
		return;
	}

	std::string channelName = msg.params[0];
	std::string key = (msg.params.size() > 1) ? msg.params[1] : "";
	client* user = _clients[fd];

	if (channelName[0] != '#' && channelName[0] != '&') {
		sendReply(fd, ":server " ERR_NOCHAN " " + user->getNickname() + " " + channelName + " :No such channel");
		return;
	}

	if (_channels.find(channelName) == _channels.end()) {
		_channels[channelName] = new channel(channelName, msg.params[1]);
		_channels[channelName]->addClient(_clients[fd]);
		_channels[channelName]->addOperator(_clients[fd]);
	}
	else {
		channel* chan = _channels[channelName];
		if (chan->isMember(user))
			return;
		if (!chan->getKey().empty() && chan->getKey() != key) {
			sendReply(fd, ":server " ERR_CANTJOINK " " + user->getNickname() + " " + channelName + " :Cannot join channel (+k)");
			return;
		}
		if (chan->getLimit() > 0 && (int)chan->getClientCount() >= chan->getLimit()) {
			sendReply(fd, ":server " ERR_CANTJOINL " " + user->getNickname() + " " + channelName + " :Cannot join channel (+l)");
			return;
		}
		if (chan->getModes().find('i') != std::string::npos) {
			sendReply(fd, ":server " ERR_CANTJOINI " " + user->getNickname() + " " + channelName + " :Cannot join channel (+i)");
			return;
		}
		chan->addClient(user);
	}
	std::string joinMsg = ":" + user->getNickname() + "!" + user->getUsername() + "@localhost JOIN :" + channelName;
	
	_channels[channelName]->broadcast(joinMsg, NULL);

	if (!_channels[channelName]->getTopic().empty()) {
		sendReply(fd, ":server " RPL_YESTOPIC " " + user->getNickname() + " " + channelName + " :" + _channels[channelName]->getTopic());
	} else {
		sendReply(fd, ":server " RPL_NOTOPIC " " + user->getNickname() + " " + channelName + " :No topic is set");
	}
	std::string names;
	const std::vector<client*>& members = _channels[channelName]->getClients();

	for (size_t i = 0; i < members.size(); i++) {
		if (_channels[channelName]->isOperator(members[i]))
			names += "@";
		names += members[i]->getNickname() + " ";
	}
	
	sendReply(fd, ":server " RPL_LISTNAMES " " + user->getNickname() + " = " + channelName + " :" + names);
	sendReply(fd, ":server " RPL_ENDLISTN " " + user->getNickname() + " " + channelName + " :End of /NAMES list");
}

