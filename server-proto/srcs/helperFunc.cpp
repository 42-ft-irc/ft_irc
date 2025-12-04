#include "server.hpp"

void server::initCommands( void ) {
	_commands["PASS"] = &server::handlePass;
	_commands["CAP"] = &server::handleCap;
	_commands["NICK"] = &server::handleNick;
	_commands["USER"] = &server::handleUser;
	_commands["PING"] = &server::handlePing;
	_commands["QUIT"] = &server::handleQuit;
	_commands["PRIVMSG"] = &server::handlePrivmsg;
	_commands["JOIN"] = &server::handleJoin;
	_commands["MODE"] = &server::handleMode;
	_commands["PART"] = &server::handlePart;
	_commands["WHO"] = &server::handleWho;
}

void server::executeCommand(int fd, message &msg) {
	std::map<std::string, CommandHandler>::iterator it = _commands.find(msg.command);

	if (it != _commands.end()) {
		(this->*(it->second))(fd, msg);
	} else {
		if (!msg.command.empty()) {
			std::string err = ":server 421 " + msg.command + " :Unknown command";
			sendReply(fd, err);
		}
	}
}

void server::sendReply(int fd, const std::string& msg) {
	std::string full = msg + "\r\n";
	send(fd, full.c_str(), full.size(), 0);
}

void server::welcomeClient(int fd) {
	client* c = _clients[fd];
	std::string nick = c->getNickname();

	c->setRegistered(true);

	std::ostringstream userCount;
	userCount << _clients.size();

	sendReply(fd, ":server " RPL_WELCOME " " + nick + " :Welcome to the IRC Network " + nick);
	sendReply(fd, ":server " RPL_YOURHOST " " + nick + " :Your host is server, running version 1.0");
	sendReply(fd, ":server " RPL_CREATED " " + nick + " :This server was created today");
	sendReply(fd, ":server " RPL_MYINFO " " + nick + " server 1.0 o o");
	sendReply(fd, ":server " RPL_LUSERME " " + nick + " :I have " + userCount.str() + " clients and 1 servers");
	sendReply(fd, ":server " RPL_MOTDSTART " " + nick + " :- server Message of the Day -");
	sendReply(fd, ":server " RPL_MOTD " " + nick + " :- Welcome to ft_irc!");
	sendReply(fd, ":server " RPL_ENDOFMOTD " " + nick + " :End of /MOTD command.");
}

client* server::findClientByNick(const std::string& nick) {
	for (std::map<int, client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == nick)
			return it->second;
	}
	return NULL;
}

channel* server::getChannelForMode(int fd, message &msg, client* sender) {
	if (msg.params.empty()) {
		sendReply(fd, ":server " ERR_NOPARAMS " " + sender->getNickname() + " MODE :Not enough parameters");
		return NULL;
	}
	std::string target = msg.params[0];
	if (target[0] != '#' && target[0] != '&') {
		sendReply(fd, ":server " ERR_NOSUCHNICK " " + sender->getNickname() + " " + target + " :No such nick/channel");
		return NULL;
	}
	if (_channels.find(target) == _channels.end()) {
		sendReply(fd, ":server " ERR_NOCHAN " " + sender->getNickname() + " " + target + " :No such channel");
		return NULL;
	}
	return _channels[target];
}