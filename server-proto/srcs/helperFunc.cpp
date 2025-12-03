#include "server.hpp"

void server::initCommands( void ) {
	_commands["PASS"] = &server::handlePass;
	_commands["CAP"] = &server::handleCap;
	_commands["NICK"] = &server::handleNick;
	_commands["USER"] = &server::handleUser;
	_commands["PING"] = &server::handlePing;
	_commands["QUIT"] = &server::handleQuit;
	_commands["PRIVMSG"] = &server::handlePrivmsg;
}

static void sendReply(int fd, const std::string& msg) {
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
	//sendReply(fd, ":server " RPL_ENDOFMOTD " " + nick + " :End of /MOTD command.");
}

client* server::findClientByNick(const std::string& nick) {
	for (std::map<int, client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNickname() == nick)
			return it->second;
	}
	return NULL;
}