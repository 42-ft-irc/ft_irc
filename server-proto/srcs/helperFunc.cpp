#include "server.hpp"

void	server::initCommands( void ) {
	_commands["CAP"] = &server::handleCap;
	_commands["NICK"] = &server::handleNick;
	_commands["USER"] = &server::handleUser;
	_commands["PING"] = &server::handlePing;
	_commands["JOIN"] = &server::handleJoin;
}

void	sendReply(int fd, const std::string& msg) {
	std::string full = msg + "\r\n";
	send(fd, full.c_str(), full.size(), 0);
}

void	server::welcomeClient(int fd) {
	client* c = _clients[fd];
	std::string nick = c->getNickname();

	c->setRegistered(true);

	sendReply(fd, ":server 001 " + nick + " :Welcome to the IRC Network " + nick);
	sendReply(fd, ":server 002 " + nick + " :Your host is server, running version 1.0");
	sendReply(fd, ":server 003 " + nick + " :This server was created today");
	sendReply(fd, ":server 004 " + nick + " server 1.0 o o");
}

