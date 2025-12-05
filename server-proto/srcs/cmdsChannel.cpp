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

	channel* chan = getChannel(channelName);

	if (!chan) {
		chan = createChannel(channelName, key, user);
	}
	else {
		if (chan->isMember(user)) return;

		if (!chan->getKey().empty() && chan->getKey() != key) {
			sendReply(fd, ":server " ERR_CANTJOINK " " + user->getNickname() + " " + channelName + " :Cannot join channel (+k)");
			return;
		}
		if (chan->getLimit() > 0 && (int)chan->getClientCount() >= chan->getLimit()) {
			sendReply(fd, ":server " ERR_CANTJOINL " " + user->getNickname() + " " + channelName + " :Cannot join channel (+l)");
			return;
		}
		if (chan->getModes().find('i') != std::string::npos && !chan->isInvited(user)) {
			sendReply(fd, ":server " ERR_CANTJOINI " " + user->getNickname() + " " + channelName + " :Cannot join channel (+i)");
			return;
		}
		chan->removeInvite(user);
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

void	server::handleMode(int fd, message &msg) {
	client* c = _clients[fd];
	channel* chan = getChannelForMode(fd, msg, c);
	if (!chan) return ;
	std::string target = msg.params[0];

	// Mode for only reading, reply: modes + values from modes
	if (msg.params.size() == 1) {
		std::string modeString = chan->getModes();
		std::string args = "";
		if (!chan->getKey().empty()) 
			args += " " + chan->getKey();
		if (chan->getLimit() > 0) {
			std::stringstream ss;
			ss << chan->getLimit();
			args += " " + ss.str();
		}
		sendReply(fd, ":server " RPL_CHANMODEIS " " + c->getNickname() + " " + target + " " + modeString + args);
		return;
	}

	// Mode for changing Modes, only possible for admins
	if (!chan->isOperator(c)) {
		sendReply(fd, ":server " ERR_COPRIVSNEEDED " " + c->getNickname() + " " + target + " :You're not channel operator");
		return;
	}

	std::string modeStr = msg.params[1];
	std::string changes = ""; // String, um erfolgreiche Änderungen zu speichern (für Broadcast)
	std::string argsOut = "";
	
	bool adding = true;
	size_t argIndex = 2;

	for (size_t i = 0; i < modeStr.length(); ++i) {
		char mode = modeStr[i];
		if (mode == '+') {
			adding = true;
			changes += "+";
		} else if (mode == '-') {
			adding = false;
			changes += "-";
		} else {
			switch (mode) {
				case 'i':
					chan->setInviteOnly(adding);
					changes += "i";
					break;
				case 't':
					chan->setTopicRestricted(adding);
					changes += "t";
					break;
				case 'k':
					if (adding) {
						if (argIndex < msg.params.size()) {
							std::string key = msg.params[argIndex++];
							chan->setKey(key);
							changes += "k";
							argsOut += " " + key;
						}
					} else {
						chan->setKey("");
						changes += "k";
					}
					break;
				case 'l':
					if (adding) {
						if (argIndex < msg.params.size()) {
							int limit = atoi(msg.params[argIndex++].c_str());
							chan->setLimit(limit);
							changes += "l";
							std::stringstream ss; ss << limit;
							argsOut += " " + ss.str();
						}
					} else {
						chan->setLimit(0);
						changes += "l";
					}
					break;
				case 'o':
					if (argIndex < msg.params.size()) {
						std::string targetNick = msg.params[argIndex++];
						client* targetClient = findClientByNick(targetNick);
						
						if (targetClient && chan->isMember(targetClient)) {
							if (adding) chan->addOperator(targetClient);
							else chan->removeOperator(targetClient);
							
							changes += "o";
							argsOut += " " + targetNick;
						} else {
							sendReply(fd, ":server " ERR_NOSUCHNICK " " + c->getNickname() + " " + targetNick + " :No such nick/channel");
						}
					}
					break;
				default:
					sendReply(fd, ":server " ERR_UNKNOWNMODE " " + c->getNickname() + " " + std::string(1, mode) + " :is unknown mode char to me");
					break;
			}
		}
	}

	// broadcast changes
	if (!changes.empty() && changes != "+" && changes != "-") {
		std::string finalMsg = ":" + c->getNickname() + "!" + c->getUsername() + "@localhost MODE " + target + " " + changes + argsOut;
		chan->broadcast(finalMsg, NULL);
	}
}

void	server::handlePart(int fd, message &msg) {
	client* c = _clients[fd];

	if (msg.params.empty()) {
		sendReply(fd, ":server " ERR_NOPARAMS " " + c->getNickname() + " PART :Not enough parameters");
		return;
	}

	std::string channelName = msg.params[0];
	std::string reason = (msg.params.size() > 1) ? msg.params[1] : "Leaving";

	channel* chan = getChannel(channelName);	
	if (!chan) {
		sendReply(fd, ":server " ERR_NOCHAN " " + c->getNickname() + " " + channelName + " :No such channel");
		return;
	}
	if (!chan->isMember(c)) {
		sendReply(fd, ":server " ERR_USERNOTINCHAN " " + c->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	std::string partMsg = ":" + c->getNickname() + "!" + c->getUsername() + "@localhost PART " + channelName + " :" + reason;
	chan->broadcast(partMsg, NULL);

	chan->removeClient(c);
	if (chan->isEmpty()) {
		removeChannel(channelName);
		std::cout << "Channel " << channelName << " deleted (empty)." << std::endl;
	}
}

void	server::handleTopic(int fd, message &msg) {
	client* c = _clients[fd];

	// Check if channel name is provided
	if (msg.params.empty()) {
		sendReply(fd, ":server " ERR_NOPARAMS " " + c->getNickname() + " TOPIC :Not enough parameters");
		return;
	}

	std::string channelName = msg.params[0];

	// Check if channel exists
	if (_channels.find(channelName) == _channels.end()) {
		sendReply(fd, ":server " ERR_NOCHAN " " + c->getNickname() + " " + channelName + " :No such channel");
		return;
	}

	channel* chan = _channels[channelName];

	// Check if user is on the channel
	if (!chan->isMember(c)) {
		sendReply(fd, ":server " ERR_USERNOTINCHAN " " + c->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}

	// Query mode: no topic parameter, just return current topic
	if (msg.params.size() == 1) {
		if (chan->getTopic().empty()) {
			sendReply(fd, ":server " RPL_NOTOPIC " " + c->getNickname() + " " + channelName + " :No topic is set");
		} else {
			sendReply(fd, ":server " RPL_YESTOPIC " " + c->getNickname() + " " + channelName + " :" + chan->getTopic());
		}
		return;
	}

	// Set mode: topic parameter provided
	// Check permissions: if +t mode is set, only operators can change topic
	if (chan->isTopicRestricted() && !chan->isOperator(c)) {
		sendReply(fd, ":server " ERR_COPRIVSNEEDED " " + c->getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	// Set the new topic (can be empty to clear it)
	std::string newTopic = msg.params[1];
	chan->setTopic(newTopic);

	// Broadcast the topic change to all channel members
	std::string topicMsg = ":" + c->getNickname() + "!" + c->getUsername() + "@localhost TOPIC " + channelName + " :" + newTopic;
	chan->broadcast(topicMsg, NULL);
}

void	server::handleKick(int fd, message &msg) {
	client* c = _clients[fd];

	// Check for minimum parameters: channel and user
	if (msg.params.size() < 2) {
		sendReply(fd, ":server " ERR_NOPARAMS " " + c->getNickname() + " KICK :Not enough parameters");
		return;
	}

	std::string channelName = msg.params[0];
	std::string targetNick = msg.params[1];
	std::string reason = (msg.params.size() > 2) ? msg.params[2] : c->getNickname();

	channel* chan = getChannel(channelName);

	if (!chan) {
		sendReply(fd, ":server " ERR_NOCHAN " " + c->getNickname() + " " + channelName + " :No such channel");
		return;
	}
	if (!chan->isMember(c)) {
		sendReply(fd, ":server " ERR_NOTONCHANNEL " " + c->getNickname() + " " + channelName + " :You're not on that channel");
		return;
	}
	if (!chan->isOperator(c)) {
		sendReply(fd, ":server " ERR_COPRIVSNEEDED " " + c->getNickname() + " " + channelName + " :You're not channel operator");
		return;
	}

	// Find the target user
	client* target = findClientByNick(targetNick);
	if (!target) {
		sendReply(fd, ":server " ERR_NOSUCHNICK " " + c->getNickname() + " " + targetNick + " :No such nick/channel");
		return;
	}
	if (!chan->isMember(target)) {
		sendReply(fd, ":server " ERR_USERNOTINCHAN " " + c->getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	std::string kickMsg = ":" + c->getNickname() + "!" + c->getUsername() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason;
	chan->broadcast(kickMsg, NULL);

	chan->removeClient(target);
	if (chan->isEmpty()) {
		removeChannel(channelName);
		std::cout << "Channel " << channelName << " deleted (empty)." << std::endl;
	}
}

void	server::handleInvite(int fd, message &msg) {
	client* c = _clients[fd];

	// Check for required parameters: nickname and channel
	if (msg.params.size() < 2) {
		sendReply(fd, ":server " ERR_NOPARAMS " " + c->getNickname() + " INVITE :Not enough parameters");
		return;
	}

	std::string targetNick = msg.params[0];
	std::string channelName = msg.params[1];

	// Find the target user
	client* target = findClientByNick(targetNick);
	if (!target) {
		sendReply(fd, ":server " ERR_NOSUCHNICK " " + c->getNickname() + " " + targetNick + " :No such nick/channel");
		return;
	}

	// Check if channel exists - if it does, inviter must be a member
	if (_channels.find(channelName) != _channels.end()) {
		channel* chan = _channels[channelName];

		// Check if inviter is on the channel
		if (!chan->isMember(c)) {
			sendReply(fd, ":server " ERR_NOTONCHANNEL " " + c->getNickname() + " " + channelName + " :You're not on that channel");
			return;
		}

		// Check if target is already on the channel
		if (chan->isMember(target)) {
			sendReply(fd, ":server " ERR_USERONCHANNEL " " + c->getNickname() + " " + targetNick + " " + channelName + " :is already on channel");
			return;
		}

		// If channel is invite-only, only operators can invite
		if (chan->getModes().find('i') != std::string::npos && !chan->isOperator(c)) {
			sendReply(fd, ":server " ERR_COPRIVSNEEDED " " + c->getNickname() + " " + channelName + " :You're not channel operator");
			return;
		}

		// Add target to invite list
		chan->addInvite(target);
	}

	// Send RPL_INVITING to the inviter
	sendReply(fd, ":server " RPL_INVITING " " + c->getNickname() + " " + targetNick + " " + channelName);

	// Send INVITE message to the target
	std::string inviteMsg = ":" + c->getNickname() + "!" + c->getUsername() + "@localhost INVITE " + targetNick + " :" + channelName;
	sendReply(target->getFD(), inviteMsg);
}
