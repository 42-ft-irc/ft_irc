#include "server.hpp"

void server::initCommands( void ) {
	_commands["CAP"] = &server::handleCap;
	_commands["NICK"] = &server::handleNick;
	_commands["USER"] = &server::handleUser;
	_commands["PING"] = &server::handlePing;
}

static void sendReply(int fd, const std::string& msg) {
	std::string full = msg + "\r\n";
	send(fd, full.c_str(), full.size(), 0);
}

void server::welcomeClient(int fd) {
	client* c = _clients[fd];
	std::string nick = c->getNickname();

	c->setRegistered(true);

	sendReply(fd, ":server 001 " + nick + " :Welcome to the IRC Network " + nick);
	sendReply(fd, ":server 002 " + nick + " :Your host is server, running version 1.0");
	sendReply(fd, ":server 003 " + nick + " :This server was created today");
	sendReply(fd, ":server 004 " + nick + " server 1.0 o o");
}

void server::handleCap(int fd, message &msg) {
	if (msg.params.size() > 0 && msg.params[0] == "LS") {
		sendReply(fd, "CAP * LS :");
	}
}

void server::handleNick(int fd, message &msg) {
	client* c = _clients[fd];

	if (msg.params.empty()) {
		sendReply(fd, ":server 431 * :No nickname given");
		return;
	}
	c->setNickname(msg.params[0]);

	if (!c->isRegistered() && !c->getNickname().empty() && !c->getUsername().empty()) {
		welcomeClient(fd);
	}
}

void server::handleUser(int fd, message &msg) {
	client* c = _clients[fd];

	if (msg.params.size() < 4) {
		sendReply(fd, ":server 461 * USER :Not enough parameters");
		return;
	}
	if (c->isRegistered()) {
		sendReply(fd, ":server 462 " + c->getNickname() + " :You may not reregister");
		return;
	}
	c->setUsername(msg.params[0]);

	if (!c->isRegistered() && !c->getNickname().empty() && !c->getUsername().empty()) {
		welcomeClient(fd);
	}
}

void server::handlePing(int fd, message &msg) {
	std::string response = "PONG :";
	if (!msg.params.empty())
		response += msg.params[0];
	sendReply(fd, response);
}
